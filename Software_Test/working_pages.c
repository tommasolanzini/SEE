/******************************************************************************
 *  MULTI-PAGE RAW WRITE+READ TEST  (NAND F3 / MT29F32G08CBACA)
 *
 *  Purpose: the single-page test proved the physical layer is bit-perfect on
 *  page 0. This test now reproduces the ORIGINAL flow (incrementing page,
 *  write-then-immediate-read on each new page) but with a KNOWN raw pattern
 *  and an on-MCU comparison -- gf2 encode/decode and the PC big-integer
 *  unpacking are BYPASSED.
 *
 *  Flow:
 *    - trigger 0      : erase block 0, then write+read page 0
 *    - trigger 1..N   : write+read page 1, 2, 3, ... (sequential, in-order)
 *    Each page gets a DISTINCT known pattern; the MCU compares the readback
 *    against it and reports mismatches.
 *
 *  Verdict:
 *    - Any errors here  -> physical multi-page write/read problem
 *                          (page-dependent / write-disturb / fast cadence).
 *    - All clean        -> physical layer is fully fine; the corruption lives
 *                          in the gf2 encode/decode or the PC unpacking.
 *****************************************************************************/

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include "definitions.h"
#include "app.h"
#include "gf2_poly.h"     /* kept for build parity; unused here */

extern APP_DATA appData;

/* ---- Sizes ---- */
#define PAYLOAD_SIZE_BYTES    512u
#define PARITY_SIZE_BYTES     8u
#define CODEWORD_SIZE_BYTES   (PAYLOAD_SIZE_BYTES + PARITY_SIZE_BYTES)   /* 520 */
#define TRAILER_SIZE_BYTES    8u
#define RESPONSE_SIZE_BYTES   (CODEWORD_SIZE_BYTES + TRAILER_SIZE_BYTES) /* 528 */

#define TEST_BLOCK            0u
#define MAX_TEST_PAGE         255u   /* stay inside block 0 */

/* ---- Tunable bit-bang timing ---- */
#define T_WE_LOW_LOOPS        30u
#define T_WE_HIGH_LOOPS       10u
#define T_OE_LOW_LOOPS        30u
#define T_OE_HIGH_LOOPS       10u
#define T_TWB_LOOPS           1000u
#define T_TRR_LOOPS           50u
#define BUSY_TIMEOUT_LOOPS    300000u
#define READY_TIMEOUT_LOOPS   8000000u

// =============================================================================
// 1. BIT-BANGING ENGINES
// =============================================================================

void NAND_WriteByte(uint8_t data) {
    PIOD_REGS->PIO_OER = (1UL << 25) | (1UL << 26) | (1UL << 24) | (1UL << 23);
    PIOC_REGS->PIO_OER = (1UL << 6)  | (1UL << 5);
    PIOA_REGS->PIO_OER = (1UL << 24) | (1UL << 25);

    uint32_t pd_set = 0U, pd_clr = 0U;
    if (data & 0x01U) pd_set |= (1UL << 25); else pd_clr |= (1UL << 25);
    if (data & 0x02U) pd_set |= (1UL << 26); else pd_clr |= (1UL << 26);
    if (data & 0x08U) pd_set |= (1UL << 24); else pd_clr |= (1UL << 24);
    if (data & 0x20U) pd_set |= (1UL << 23); else pd_clr |= (1UL << 23);
    if (pd_set) PIOD_REGS->PIO_SODR = pd_set;
    if (pd_clr) PIOD_REGS->PIO_CODR = pd_clr;

    uint32_t pc_set = 0U, pc_clr = 0U;
    if (data & 0x04U) pc_set |= (1UL << 6); else pc_clr |= (1UL << 6);
    if (data & 0x40U) pc_set |= (1UL << 5); else pc_clr |= (1UL << 5);
    if (pc_set) PIOC_REGS->PIO_SODR = pc_set;
    if (pc_clr) PIOC_REGS->PIO_CODR = pc_clr;

    uint32_t pa_set = 0U, pa_clr = 0U;
    if (data & 0x10U) pa_set |= (1UL << 24); else pa_clr |= (1UL << 24);
    if (data & 0x80U) pa_set |= (1UL << 25); else pa_clr |= (1UL << 25);
    if (pa_set) PIOA_REGS->PIO_SODR = pa_set;
    if (pa_clr) PIOA_REGS->PIO_CODR = pa_clr;

    NAND_WE_Clear();
    for (volatile uint32_t d = 0; d < T_WE_LOW_LOOPS;  d++) { asm("nop"); }
    NAND_WE_Set();
    for (volatile uint32_t d = 0; d < T_WE_HIGH_LOOPS; d++) { asm("nop"); }
}

uint8_t NAND_ReadByte(void) {
    uint8_t data = 0;

    PIOD_REGS->PIO_ODR = (1UL << 25) | (1UL << 26) | (1UL << 24) | (1UL << 23);
    PIOC_REGS->PIO_ODR = (1UL << 6)  | (1UL << 5);
    PIOA_REGS->PIO_ODR = (1UL << 24) | (1UL << 25);

    NAND_OE_Clear();
    for (volatile uint32_t d = 0; d < T_OE_LOW_LOOPS; d++) { asm("nop"); }

    uint32_t pd = PIOD_REGS->PIO_PDSR;
    uint32_t pc = PIOC_REGS->PIO_PDSR;
    uint32_t pa = PIOA_REGS->PIO_PDSR;

    if (pd & (1UL << 25)) data |= 0x01U;
    if (pd & (1UL << 26)) data |= 0x02U;
    if (pc & (1UL << 6))  data |= 0x04U;
    if (pd & (1UL << 24)) data |= 0x08U;
    if (pa & (1UL << 24)) data |= 0x10U;
    if (pd & (1UL << 23)) data |= 0x20U;
    if (pc & (1UL << 5))  data |= 0x40U;
    if (pa & (1UL << 25)) data |= 0x80U;

    NAND_OE_Set();
    for (volatile uint32_t d = 0; d < T_OE_HIGH_LOOPS; d++) { asm("nop"); }

    return data;
}

// =============================================================================
// 2. ONFI HELPERS
// =============================================================================

void NAND_Command(uint8_t cmd) {
    NAND_CLE_Set(); NAND_ALE_Clear(); NAND_WriteByte(cmd); NAND_CLE_Clear();
}
void NAND_Address(uint8_t addr) {
    NAND_CLE_Clear(); NAND_ALE_Set(); NAND_WriteByte(addr); NAND_ALE_Clear();
}

// =============================================================================
// 3. NAND DRIVERS (edge-aware ready)
// =============================================================================

void HW_NAND_Wait_Ready(void) {
    for (volatile uint32_t d = 0; d < T_TWB_LOOPS; d++) { asm("nop"); }
    uint32_t to = BUSY_TIMEOUT_LOOPS;
    while (F3_RB_Get() && to) { to--; }
    to = READY_TIMEOUT_LOOPS;
    while (!F3_RB_Get() && to) { to--; }
    for (volatile uint32_t d = 0; d < T_TRR_LOOPS; d++) { asm("nop"); }
}

void HW_NAND_Erase_Block(uint32_t block_address) {
    CE_F3_Clear();
    NAND_Command(0x60);
    uint32_t row = (block_address << 8);
    NAND_Address(row & 0xFF);
    NAND_Address((row >> 8) & 0xFF);
    NAND_Address((row >> 16) & 0xFF);
    NAND_Command(0xD0);
    HW_NAND_Wait_Ready();
    CE_F3_Set();
}

void HW_NAND_Write_Codeword(uint32_t page_address, uint8_t* payload, uint8_t* parity) {
    CE_F3_Clear();
    NAND_Command(0x80);
    NAND_Address(0x00); NAND_Address(0x00);
    NAND_Address(page_address & 0xFF);
    NAND_Address((page_address >> 8) & 0xFF);
    NAND_Address((page_address >> 16) & 0xFF);
    for (uint32_t i = 0; i < PAYLOAD_SIZE_BYTES; i++) NAND_WriteByte(payload[i]);
    NAND_Command(0x85);
    NAND_Address(0x00); NAND_Address(0x10);
    for (uint32_t i = 0; i < PARITY_SIZE_BYTES; i++) NAND_WriteByte(parity[i]);
    NAND_Command(0x10);
    HW_NAND_Wait_Ready();
    CE_F3_Set();
}

void HW_NAND_Read_Codeword(uint32_t page_address, uint8_t* payload, uint8_t* parity) {
    CE_F3_Clear();
    NAND_Command(0x00);
    NAND_Address(0x00); NAND_Address(0x00);
    NAND_Address(page_address & 0xFF);
    NAND_Address((page_address >> 8) & 0xFF);
    NAND_Address((page_address >> 16) & 0xFF);
    NAND_Command(0x30);
    HW_NAND_Wait_Ready();
    for (uint32_t i = 0; i < PAYLOAD_SIZE_BYTES; i++) payload[i] = NAND_ReadByte();
    NAND_Command(0x05);
    NAND_Address(0x00); NAND_Address(0x10);
    NAND_Command(0xE0);
    for (volatile uint32_t d = 0; d < 200u; d++) { asm("nop"); }
    for (uint32_t i = 0; i < PARITY_SIZE_BYTES; i++) parity[i] = NAND_ReadByte();
    CE_F3_Set();
}

// =============================================================================
// 4. BUFFERS + PATTERN
// =============================================================================

uint8_t CACHE_ALIGN known_codeword[CODEWORD_SIZE_BYTES];
uint8_t CACHE_ALIGN flash_read_buffer[CODEWORD_SIZE_BYTES];
uint8_t CACHE_ALIGN rx_buffer[PAYLOAD_SIZE_BYTES];
uint8_t CACHE_ALIGN tx_buffer[RESPONSE_SIZE_BYTES];

uint32_t current_page = 0u;
bool block_erased = false;

static void build_known_pattern(uint32_t page) {
    for (uint32_t k = 0; k < PAYLOAD_SIZE_BYTES; k++)
        known_codeword[k] = (uint8_t)((k * 0x9Du) + (page * 0x11u) + 0x3Bu);
    for (uint32_t k = 0; k < PARITY_SIZE_BYTES; k++)
        known_codeword[PAYLOAD_SIZE_BYTES + k] =
            (uint8_t)(0xC3u ^ (k * 0x55u) ^ (uint8_t)page);
}

// =============================================================================
// 5. MAIN
// =============================================================================

int main(void) {
    SYS_Initialize(NULL);

    PMC_REGS->PMC_PCER0 = (1UL << 11);
    PIOB_REGS->PIO_WPMR = (0x50494FUL << 8);
    PIOB_REGS->PIO_OER  = (1UL << 1);
    PIOB_REGS->PIO_SODR = (1UL << 1);
    for (volatile int d = 0; d < 100000; d++) { asm("nop"); }

    NAND_Command(0xFF);
    HW_NAND_Wait_Ready();
    gf2_initialize();

    enum {
        WAIT_FOR_READ_TRIGGER,
        WAIT_FOR_READ_COMPLETE,
        PROCESS_DATA,
        SEND_MESSAGE,
        WAIT_FOR_WRITE_COMPLETE
    } customState = WAIT_FOR_READ_TRIGGER;

    while (true) {
        SYS_Tasks();

        if (appData.isConfigured == true) {
            appData.state = APP_STATE_ERROR;

            switch (customState) {
                case WAIT_FOR_READ_TRIGGER:
                    appData.appCOMPortObjects[0].isReadComplete = false;
                    if (USB_DEVICE_CDC_Read(USB_DEVICE_CDC_INDEX_0,
                            &appData.appCOMPortObjects[0].readTransferHandle,
                            rx_buffer, PAYLOAD_SIZE_BYTES) == USB_DEVICE_CDC_RESULT_OK) {
                        customState = WAIT_FOR_READ_COMPLETE;
                    }
                    break;

                case WAIT_FOR_READ_COMPLETE:
                    if (appData.appCOMPortObjects[0].isReadComplete)
                        customState = PROCESS_DATA;
                    break;

                case PROCESS_DATA: {
                    uint8_t  flags = 0u;
                    uint32_t page  = current_page;

                    if (!block_erased) {
                        HW_NAND_Erase_Block(TEST_BLOCK);
                        block_erased = true;
                        flags |= 0x01u;     /* erase happened on this trigger */
                    }

                    build_known_pattern(page);

                    HW_NAND_Write_Codeword(page, known_codeword,
                                           &known_codeword[PAYLOAD_SIZE_BYTES]);
                    HW_NAND_Read_Codeword(page, flash_read_buffer,
                                          &flash_read_buffer[PAYLOAD_SIZE_BYTES]);

                    uint32_t mism_bytes = 0u, mism_bits = 0u;
                    int32_t  first_off  = -1;
                    for (uint32_t i = 0; i < CODEWORD_SIZE_BYTES; i++) {
                        uint8_t x = (uint8_t)(flash_read_buffer[i] ^ known_codeword[i]);
                        if (x) {
                            mism_bytes++;
                            if (first_off < 0) first_off = (int32_t)i;
                            while (x) { mism_bits++; x &= (uint8_t)(x - 1u); }
                        }
                    }

                    for (uint32_t i = 0; i < CODEWORD_SIZE_BYTES; i++)
                        tx_buffer[i] = flash_read_buffer[i];

                    tx_buffer[520] = (mism_bytes > 255u) ? 255u : (uint8_t)mism_bytes;
                    tx_buffer[521] = (uint8_t)(mism_bits & 0xFFu);
                    tx_buffer[522] = (uint8_t)((mism_bits >> 8) & 0xFFu);
                    if (first_off < 0) { tx_buffer[523] = 0xFFu; tx_buffer[524] = 0xFFu; }
                    else { tx_buffer[523] = (uint8_t)(first_off & 0xFFu);
                           tx_buffer[524] = (uint8_t)((first_off >> 8) & 0xFFu); }
                    tx_buffer[525] = (uint8_t)(page & 0xFFu);
                    tx_buffer[526] = (uint8_t)((page >> 8) & 0xFFu);
                    tx_buffer[527] = flags;

                    if (current_page < MAX_TEST_PAGE) current_page++;

                    customState = SEND_MESSAGE;
                    break;
                }

                case SEND_MESSAGE:
                    appData.appCOMPortObjects[0].isWriteComplete = false;
                    if (USB_DEVICE_CDC_Write(USB_DEVICE_CDC_INDEX_0,
                            &appData.appCOMPortObjects[0].writeTransferHandle,
                            tx_buffer, RESPONSE_SIZE_BYTES,
                            USB_DEVICE_CDC_TRANSFER_FLAGS_DATA_COMPLETE) == USB_DEVICE_CDC_RESULT_OK) {
                        customState = WAIT_FOR_WRITE_COMPLETE;
                    }
                    break;

                case WAIT_FOR_WRITE_COMPLETE:
                    if (appData.appCOMPortObjects[0].isWriteComplete)
                        customState = WAIT_FOR_READ_TRIGGER;
                    break;
            }
        }
    }
    return EXIT_FAILURE;
}