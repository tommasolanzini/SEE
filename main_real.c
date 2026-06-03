#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include "definitions.h" 
#include "gf2_poly.h"

// --- Payload and Parity Sizes ---
#define PAYLOAD_SIZE_BYTES 512 
#define PARITY_SIZE_BYTES 8      // BCH + CRC
#define CODEWORD_SIZE_BYTES (PAYLOAD_SIZE_BYTES + PARITY_SIZE_BYTES) // 520 bytes total

volatile uint8_t TARGET_BIT_FLIPS = 2; 

// =============================================================================
// 1. BIT-BANGING ENGINES
// =============================================================================

void NAND_WriteByte(uint8_t data) {
    NAND_D0_OutputEnable(); NAND_D1_OutputEnable(); NAND_D2_OutputEnable(); NAND_D3_OutputEnable();
    NAND_D4_OutputEnable(); NAND_D5_OutputEnable(); NAND_D6_OutputEnable(); NAND_D7_OutputEnable();

    if (data & 0x01) NAND_D0_Set(); else NAND_D0_Clear();
    if (data & 0x02) NAND_D1_Set(); else NAND_D1_Clear();
    if (data & 0x04) NAND_D2_Set(); else NAND_D2_Clear();
    if (data & 0x08) NAND_D3_Set(); else NAND_D3_Clear();
    if (data & 0x10) NAND_D4_Set(); else NAND_D4_Clear();
    if (data & 0x20) NAND_D5_Set(); else NAND_D5_Clear();
    if (data & 0x40) NAND_D6_Set(); else NAND_D6_Clear();
    if (data & 0x80) NAND_D7_Set(); else NAND_D7_Clear();

    NAND_WE_Clear(); 
    asm("nop"); asm("nop"); 
    NAND_WE_Set();
}

uint8_t NAND_ReadByte(void) {
    uint8_t data = 0;

    NAND_D0_InputEnable(); NAND_D1_InputEnable(); NAND_D2_InputEnable(); NAND_D3_InputEnable();
    NAND_D4_InputEnable(); NAND_D5_InputEnable(); NAND_D6_InputEnable(); NAND_D7_InputEnable();

    NAND_RE_Clear();
    asm("nop"); asm("nop"); 

    if (NAND_D0_Get()) data |= 0x01;
    if (NAND_D1_Get()) data |= 0x02;
    if (NAND_D2_Get()) data |= 0x04;
    if (NAND_D3_Get()) data |= 0x08;
    if (NAND_D4_Get()) data |= 0x10;
    if (NAND_D5_Get()) data |= 0x20;
    if (NAND_D6_Get()) data |= 0x40;
    if (NAND_D7_Get()) data |= 0x80;

    NAND_RE_Set();
    return data;
}

// =============================================================================
// 2. ONFI PROTOCOL HELPERS
// =============================================================================

void NAND_Command(uint8_t cmd) {
    NAND_CLE_Set();    
    NAND_ALE_Clear();
    NAND_WriteByte(cmd);
    NAND_CLE_Clear();  
}

void NAND_Address(uint8_t addr) {
    NAND_CLE_Clear();
    NAND_ALE_Set();    
    NAND_WriteByte(addr);
    NAND_ALE_Clear();  
}

// =============================================================================
// 3. HARDWARE NAND DRIVERS 
// =============================================================================

void HW_NAND_Write_Codeword(uint32_t page_address, uint8_t* payload, uint8_t* parity) {
    FLASH1_CE_Clear(); 
    NAND_Command(0x80); 

    NAND_Address(0x00); 
    NAND_Address(0x00); 
    NAND_Address(page_address & 0xFF);         
    NAND_Address((page_address >> 8) & 0xFF);  
    NAND_Address((page_address >> 16) & 0xFF); 

    for(uint32_t i = 0; i < PAYLOAD_SIZE_BYTES; i++) {
        NAND_WriteByte(payload[i]);
    }

    NAND_Command(0x85); 
    NAND_Address(0x00); 
    NAND_Address(0x10); 

    for(uint32_t i = 0; i < PARITY_SIZE_BYTES; i++) {
        NAND_WriteByte(parity[i]);
    }

    NAND_Command(0x10); 
    while(F1_RB_Get() == 0) {} 
    FLASH1_CE_Set(); 
}

void HW_NAND_Read_Codeword(uint32_t page_address, uint8_t* payload, uint8_t* parity) {
    FLASH1_CE_Clear(); 
    NAND_Command(0x00); 

    NAND_Address(0x00); 
    NAND_Address(0x00); 
    NAND_Address(page_address & 0xFF);         
    NAND_Address((page_address >> 8) & 0xFF);  
    NAND_Address((page_address >> 16) & 0xFF); 

    NAND_Command(0x30); 
    while(F1_RB_Get() == 0) {} 

    for(uint32_t i = 0; i < PAYLOAD_SIZE_BYTES; i++) {
        payload[i] = NAND_ReadByte();
    }

    NAND_Command(0x05); 
    NAND_Address(0x00); 
    NAND_Address(0x10); 
    NAND_Command(0xE0); 

    while(F1_RB_Get() == 0) {} 

    for(uint32_t i = 0; i < PARITY_SIZE_BYTES; i++) {
        parity[i] = NAND_ReadByte();
    }

    FLASH1_CE_Set();
}

// =============================================================================
// 4. USB AND APPLICATION STATE MACHINE
// =============================================================================

static bool is_usb_configured = false;
static bool is_read_complete = false;
static bool is_write_complete = false;
static USB_DEVICE_CDC_TRANSFER_HANDLE readTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
static USB_DEVICE_CDC_TRANSFER_HANDLE writeTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
static USB_DEVICE_HANDLE usbDeviceHandle = USB_DEVICE_HANDLE_INVALID; 

uint8_t CACHE_ALIGN rx_buffer[PAYLOAD_SIZE_BYTES];
uint8_t CACHE_ALIGN tx_buffer[CODEWORD_SIZE_BYTES + 2]; 
uint8_t codeword_buffer[CODEWORD_SIZE_BYTES];
uint8_t flash_read_buffer[CODEWORD_SIZE_BYTES];

uint32_t current_nand_page = 0x00000000; 

// --- The Windows/PySerial Kernel Patches ---
uint8_t CACHE_ALIGN line_coding_dummy[7]; 
uint8_t CACHE_ALIGN get_line_coding_data[7] = {0x00, 0xC2, 0x01, 0x00, 0x00, 0x00, 0x08}; 

void inject_errors(uint8_t* codeword, uint32_t byte_length, uint8_t num_flips) {
    if (num_flips == 0) return;
    uint32_t total_bits = byte_length * 8;
    uint32_t flipped_positions[4] = {0}; 
    uint8_t flips_applied = 0;

    while (flips_applied < num_flips) {
        uint32_t bit_idx = rand() % total_bits;
        bool already_flipped = false;
        for (uint8_t i = 0; i < flips_applied; i++) {
            if (flipped_positions[i] == bit_idx) { already_flipped = true; break; }
        }
        if (!already_flipped) {
            flipped_positions[flips_applied] = bit_idx;
            codeword[bit_idx / 8] ^= (1 << (bit_idx % 8));
            flips_applied++;
        }
    }
}

// --- Bulletproof CDC Event Handler ---
USB_DEVICE_CDC_EVENT_RESPONSE APP_USBDeviceCDCEventHandler(USB_DEVICE_CDC_INDEX index, USB_DEVICE_CDC_EVENT event, void * pData, uintptr_t userData) {
    switch (event) {
        case USB_DEVICE_CDC_EVENT_GET_LINE_CODING:
            USB_DEVICE_ControlSend(usbDeviceHandle, get_line_coding_data, 7);
            break;
        case USB_DEVICE_CDC_EVENT_SET_LINE_CODING:
            USB_DEVICE_ControlReceive(usbDeviceHandle, line_coding_dummy, 7);
            break;
        case USB_DEVICE_CDC_EVENT_CONTROL_TRANSFER_DATA_RECEIVED:
            USB_DEVICE_ControlStatus(usbDeviceHandle, USB_DEVICE_CONTROL_STATUS_OK);
            break;
        case USB_DEVICE_CDC_EVENT_SET_CONTROL_LINE_STATE:
            USB_DEVICE_ControlStatus(usbDeviceHandle, USB_DEVICE_CONTROL_STATUS_OK);
            break;
        case USB_DEVICE_CDC_EVENT_READ_COMPLETE:
            is_read_complete = true;
            break;
        case USB_DEVICE_CDC_EVENT_WRITE_COMPLETE:
            is_write_complete = true;
            break;
        default:
            break;
    }
    return USB_DEVICE_CDC_EVENT_RESPONSE_NONE;
}

void APP_USBDeviceEventHandler(USB_DEVICE_EVENT event, void * eventData, uintptr_t context) {
    switch(event) {
        case USB_DEVICE_EVENT_CONFIGURED:
            is_usb_configured = true;
            USB_DEVICE_CDC_EventHandlerSet(USB_DEVICE_CDC_INDEX_0, APP_USBDeviceCDCEventHandler, 0);
            break;
        case USB_DEVICE_EVENT_SUSPENDED:
        case USB_DEVICE_EVENT_RESET: 
        case USB_DEVICE_EVENT_DECONFIGURED:
            is_usb_configured = false;
            break;
        default: break;
    }
}

// =============================================================================
// MAIN ENTRY POINT
// =============================================================================
int main(void) {
    SYS_Initialize(NULL); 
    srand(12345); 
    gf2_initialize(); 

    // Integrated State Machine 
    enum { 
        WAIT_FOR_CONFIG, 
        WAIT_FOR_DEBOUNCE, 
        WAIT_FOR_READY, 
        WAIT_FOR_READ_TRIGGER, 
        WAIT_FOR_READ_COMPLETE, 
        PROCESS_DATA, 
        SEND_MESSAGE, 
        WAIT_FOR_WRITE_COMPLETE 
    } appState = WAIT_FOR_CONFIG;
    
    uint32_t debounce_counter = 0;

    while (1) {
        SYS_Tasks();

        switch(appState) {
            case WAIT_FOR_CONFIG:
                if (usbDeviceHandle == USB_DEVICE_HANDLE_INVALID) {
                    usbDeviceHandle = USB_DEVICE_Open(USB_DEVICE_INDEX_0, DRV_IO_INTENT_READWRITE);
                    if (usbDeviceHandle != USB_DEVICE_HANDLE_INVALID) {
                        USB_DEVICE_EventHandlerSet(usbDeviceHandle, APP_USBDeviceEventHandler, 0);
                        appState = WAIT_FOR_DEBOUNCE;
                        debounce_counter = 0;
                    }
                }
                break;

            case WAIT_FOR_DEBOUNCE:
                debounce_counter++;
                if (debounce_counter > 1000000) {
                    USB_DEVICE_Attach(usbDeviceHandle);
                    appState = WAIT_FOR_READY;
                }
                break;

            case WAIT_FOR_READY:
                if (is_usb_configured) {
                    appState = WAIT_FOR_READ_TRIGGER;
                }
                break;

            case WAIT_FOR_READ_TRIGGER:
                if (!is_usb_configured) { appState = WAIT_FOR_CONFIG; break; }
                
                is_read_complete = false;
                // STUBBORN FIX: Ensure the DMA controller actually queues the 512-byte read
                if (USB_DEVICE_CDC_Read(USB_DEVICE_CDC_INDEX_0, &readTransferHandle, rx_buffer, PAYLOAD_SIZE_BYTES) == USB_DEVICE_CDC_RESULT_OK) {
                    appState = WAIT_FOR_READ_COMPLETE;
                }
                break;

            case WAIT_FOR_READ_COMPLETE:
                if (!is_usb_configured) { appState = WAIT_FOR_CONFIG; break; } 
                if (is_read_complete) {
                    appState = PROCESS_DATA;
                }
                break;

            case PROCESS_DATA:
                // 1. Encode Data
                gf2_encode_data(rx_buffer, PAYLOAD_SIZE_BYTES, codeword_buffer);
                
                // 2. Inject Simulated Radiation Errors
                inject_errors(codeword_buffer, CODEWORD_SIZE_BYTES, TARGET_BIT_FLIPS);

                // 3. Hardware Write 
                HW_NAND_Write_Codeword(current_nand_page, codeword_buffer, &codeword_buffer[PAYLOAD_SIZE_BYTES]);

                // 4. Hardware Read 
                HW_NAND_Read_Codeword(current_nand_page, flash_read_buffer, &flash_read_buffer[PAYLOAD_SIZE_BYTES]);

                // 5. Decode and Correct
                uint8_t crc_status = 0;
                int bch_status = gf2_correct_errors(flash_read_buffer, CODEWORD_SIZE_BYTES, &crc_status);

                // 6. Build the 522-byte Response Packet
                for(int i = 0; i < CODEWORD_SIZE_BYTES; i++) {
                    tx_buffer[i] = flash_read_buffer[i];
                }
                tx_buffer[CODEWORD_SIZE_BYTES]     = (uint8_t)bch_status; 
                tx_buffer[CODEWORD_SIZE_BYTES + 1] = crc_status;
                
                // (Optional increment)
                // current_nand_page++;

                appState = SEND_MESSAGE;
                break;

            case SEND_MESSAGE:
                if (!is_usb_configured) { appState = WAIT_FOR_CONFIG; break; }
                
                is_write_complete = false;
                // STUBBORN FIX: Ensure the DMA controller actually queues the 522-byte write
                if (USB_DEVICE_CDC_Write(USB_DEVICE_CDC_INDEX_0, &writeTransferHandle, tx_buffer, CODEWORD_SIZE_BYTES + 2, USB_DEVICE_CDC_TRANSFER_FLAGS_DATA_COMPLETE) == USB_DEVICE_CDC_RESULT_OK) {
                    appState = WAIT_FOR_WRITE_COMPLETE;
                }
                break;

            case WAIT_FOR_WRITE_COMPLETE:
                if (!is_usb_configured) { appState = WAIT_FOR_CONFIG; break; }
                if (is_write_complete) {
                    // Loop cleanly back to wait for the next Python payload
                    appState = WAIT_FOR_READ_TRIGGER;
                }
                break;
        }
    }
    return 0;
}