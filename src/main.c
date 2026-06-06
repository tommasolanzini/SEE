#include <stddef.h>                     
#include <stdbool.h>                    
#include <stdlib.h>                     
#include <stdint.h>
#include "definitions.h" 
#include "app.h"         // Allows us to read app.c's USB flags
#include "gf2_poly.h"

// Bring in the appData structure from app.c so we can safely hijack it
extern APP_DATA appData;

// --- Payload and Parity Sizes ---
#define PAYLOAD_SIZE_BYTES 512 
#define PARITY_SIZE_BYTES 8      // BCH + CRC
#define CODEWORD_SIZE_BYTES (PAYLOAD_SIZE_BYTES + PARITY_SIZE_BYTES) // 520 bytes total

volatile uint8_t TARGET_BIT_FLIPS = 0; 

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

    NAND_OE_Clear();
    asm("nop"); asm("nop"); 

    if (NAND_D0_Get()) data |= 0x01;
    if (NAND_D1_Get()) data |= 0x02;
    if (NAND_D2_Get()) data |= 0x04;
    if (NAND_D3_Get()) data |= 0x08;
    if (NAND_D4_Get()) data |= 0x10;
    if (NAND_D5_Get()) data |= 0x20;
    if (NAND_D6_Get()) data |= 0x40;
    if (NAND_D7_Get()) data |= 0x80;

    NAND_OE_Set();
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
    CE_F1_Clear(); 
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
    CE_F1_Set(); 
}

void HW_NAND_Read_Codeword(uint32_t page_address, uint8_t* payload, uint8_t* parity) {
    CE_F1_Clear(); 
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

    CE_F1_Set();
}

// =============================================================================
// 4. MAIN ENTRY POINT & CUSTOM STATE MACHINE
// =============================================================================

uint8_t CACHE_ALIGN rx_buffer[PAYLOAD_SIZE_BYTES];
uint8_t CACHE_ALIGN tx_buffer[CODEWORD_SIZE_BYTES + 2]; 
uint8_t codeword_buffer[CODEWORD_SIZE_BYTES];
uint8_t flash_read_buffer[CODEWORD_SIZE_BYTES];
uint32_t current_nand_page = 0x00000000; 

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

int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );

    srand(12345);
    gf2_initialize();

    enum {
        WAIT_FOR_READ_TRIGGER,
        WAIT_FOR_READ_COMPLETE,
        PROCESS_DATA,
        SEND_MESSAGE,
        WAIT_FOR_WRITE_COMPLETE
    } customState = WAIT_FOR_READ_TRIGGER;

    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );

        /* Let app.c handle the physical USB connection to Windows.
           Once it successfully configures the COM ports, we hijack the data flow. */
        if (appData.isConfigured == true)
        {
            /* FREEZE APP.C: We force app.c's background task into an error state.
               This prevents it from stealing our USB payload, but leaves its safe
               Windows kernel handlers perfectly intact! */
            appData.state = APP_STATE_ERROR;

            switch(customState) {
                case WAIT_FOR_READ_TRIGGER:
                    appData.appCOMPortObjects[0].isReadComplete = false;
                    if (USB_DEVICE_CDC_Read(USB_DEVICE_CDC_INDEX_0, &appData.appCOMPortObjects[0].readTransferHandle, rx_buffer, PAYLOAD_SIZE_BYTES) == USB_DEVICE_CDC_RESULT_OK) {
                        customState = WAIT_FOR_READ_COMPLETE;
                    }
                    break;

                case WAIT_FOR_READ_COMPLETE:
                    if (appData.appCOMPortObjects[0].isReadComplete) {
                        customState = PROCESS_DATA;
                    }
                    break;

                case PROCESS_DATA:
                    gf2_encode_data(rx_buffer, PAYLOAD_SIZE_BYTES, codeword_buffer);
                    inject_errors(codeword_buffer, CODEWORD_SIZE_BYTES, TARGET_BIT_FLIPS);

                    HW_NAND_Write_Codeword(current_nand_page, codeword_buffer, &codeword_buffer[PAYLOAD_SIZE_BYTES]);
                    HW_NAND_Read_Codeword(current_nand_page, flash_read_buffer, &flash_read_buffer[PAYLOAD_SIZE_BYTES]);

                    uint8_t crc_status = 0;
                    int bch_status = gf2_correct_errors(flash_read_buffer, CODEWORD_SIZE_BYTES, &crc_status);

                    for(int i = 0; i < CODEWORD_SIZE_BYTES; i++) {
                        tx_buffer[i] = flash_read_buffer[i];
                    }
                    tx_buffer[CODEWORD_SIZE_BYTES]     = (uint8_t)bch_status; 
                    tx_buffer[CODEWORD_SIZE_BYTES + 1] = crc_status;
                    
                    customState = SEND_MESSAGE;
                    break;

                case SEND_MESSAGE:
                    appData.appCOMPortObjects[0].isWriteComplete = false;
                    if (USB_DEVICE_CDC_Write(USB_DEVICE_CDC_INDEX_0, &appData.appCOMPortObjects[0].writeTransferHandle, tx_buffer, CODEWORD_SIZE_BYTES + 2, USB_DEVICE_CDC_TRANSFER_FLAGS_DATA_COMPLETE) == USB_DEVICE_CDC_RESULT_OK) {
                        customState = WAIT_FOR_WRITE_COMPLETE;
                    }
                    break;

                case WAIT_FOR_WRITE_COMPLETE:
                    if (appData.appCOMPortObjects[0].isWriteComplete) {
                        customState = WAIT_FOR_READ_TRIGGER;
                    }
                    break;
            }
        }
    }

    /* Execution should not come here during normal operation */
    return ( EXIT_FAILURE );
}