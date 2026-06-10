#include <stddef.h>                     
#include <stdbool.h>                    
#include <stdlib.h>                     
#include <stdint.h>
#include "definitions.h" 
#include "app.h"         // Consente di leggere i flag USB di app.c
#include "gf2_poly.h"

// Recupera la struttura appData definita in app.c per gestirne lo stato
extern APP_DATA appData;

// --- Dimensioni Payload e Parità ---
#define PAYLOAD_SIZE_BYTES 512 
#define PARITY_SIZE_BYTES 8      // BCH + CRC
#define CODEWORD_SIZE_BYTES (PAYLOAD_SIZE_BYTES + PARITY_SIZE_BYTES) // 520 byte total

volatile uint8_t TARGET_BIT_FLIPS = 0; 

// --- Prototipi delle funzioni per evitare dichiarazioni implicite ---
void NAND_WriteByte(uint8_t data);
uint8_t NAND_ReadByte(void);
void NAND_Command(uint8_t cmd);
void NAND_Address(uint8_t addr);
void HW_NAND_Wait_Ready(bool restore_read_mode);
void HW_NAND_Erase_Block(uint32_t block_address);
void HW_NAND_Write_Codeword(uint32_t page_address, uint8_t* payload, uint8_t* parity);
void HW_NAND_Read_Codeword(uint32_t page_address, uint8_t* payload, uint8_t* parity);
void inject_errors(uint8_t* codeword, uint32_t byte_length, uint8_t num_flips);
/* gf2_extract_payload è dichiarata in gf2_poly.h (linkage C). */

// =============================================================================
// 1. BIT-BANGING ENGINES (OPTIMIZED DIRECT REGISTER ACCESS)
// =============================================================================

void NAND_WriteByte(uint8_t data) {
    /* 1. Forza i pin in modalità OUTPUT scavalcando le astrazioni di Harmony */
    PIOD_REGS->PIO_OER = (1UL << 25) | (1UL << 26) | (1UL << 24) | (1UL << 23);
    PIOC_REGS->PIO_OER = (1UL << 6) | (1UL << 5);
    PIOA_REGS->PIO_OER = (1UL << 24) | (1UL << 25);

    /* 2. Scrittura atomica e veloce sui registri per impostare il bus dati */
    uint32_t pd_set = 0U, pd_clr = 0U;
    if (data & 0x01U) pd_set |= (1UL << 25); else pd_clr |= (1UL << 25); 
    if (data & 0x02U) pd_set |= (1UL << 26); else pd_clr |= (1UL << 26); 
    if (data & 0x08U) pd_set |= (1UL << 24); else pd_clr |= (1UL << 24); 
    if (data & 0x20U) pd_set |= (1UL << 23); else pd_clr |= (1UL << 23); 
    if (pd_set) PIOD_REGS->PIO_SODR = pd_set;
    if (pd_clr) PIOD_REGS->PIO_CODR = pd_clr;

    uint32_t pc_set = 0U, pc_clr = 0U;
    if (data & 0x04U) pc_set |= (1UL << 6);  else pc_clr |= (1UL << 6);  
    if (data & 0x40U) pc_set |= (1UL << 5);  else pc_clr |= (1UL << 5);  
    if (pc_set) PIOC_REGS->PIO_SODR = pc_set;
    if (pc_clr) PIOC_REGS->PIO_CODR = pc_clr;

    uint32_t pa_set = 0U, pa_clr = 0U;
    if (data & 0x10U) pa_set |= (1UL << 24); else pa_clr |= (1UL << 24); 
    if (data & 0x80U) pa_set |= (1UL << 25); else pa_clr |= (1UL << 25); 
    if (pa_set) PIOA_REGS->PIO_SODR = pa_set;
    if (pa_clr) PIOA_REGS->PIO_CODR = pa_clr;

    /* 3. Tira WE# con isolamento degli interrupt ARM */
    __disable_irq();
    NAND_WE_Clear(); 
    for(volatile int d = 0; d < 30; d++); 
    NAND_WE_Set();
    for(volatile int d = 0; d < 10; d++);  
    __enable_irq();
}

uint8_t NAND_ReadByte(void) {
    uint8_t data = 0;

    /* 1. Forza i pin in modalità INPUT */
    PIOD_REGS->PIO_ODR = (1UL << 25) | (1UL << 26) | (1UL << 24) | (1UL << 23);
    PIOC_REGS->PIO_ODR = (1UL << 6) | (1UL << 5);
    PIOA_REGS->PIO_ODR = (1UL << 24) | (1UL << 25);

    /* 2. Porta OE# LOW e scherma gli interrupt */
    __disable_irq();
    NAND_OE_Clear();
    for(volatile int d = 0; d < 30; d++); 

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
    for(volatile int d = 0; d < 10; d++); 
    __enable_irq();
    
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

void HW_NAND_Wait_Ready(bool restore_read_mode) {
    /* 1. Ritardo tWB: Attesa per consentire alla NAND di entrare in BUSY */
    for(volatile uint32_t d = 0; d < 1000; d++) { asm("nop"); }
    
    /* 2. Polling del Registro di Stato 0x70 (Risolve l'instabilità del pin R/B#) */
    uint8_t status = 0;
    do {
        NAND_Command(0x70);
        status = NAND_ReadByte();
    } while ((status & 0x40) == 0);
    
    /* 3. Se eseguito durante una lettura dati, riposiziona il puntatore interno (0x00) */
    if (restore_read_mode) {
        NAND_Command(0x00);
    }
    
    /* 4. Ritardo tRR */
    for(volatile uint32_t d = 0; d < 50; d++) { asm("nop"); }
}

void HW_NAND_Erase_Block(uint32_t block_address) {
    CE_F3_Clear();
    
    NAND_Command(0x60); 
    
    /* Calcolo dell'indirizzo di riga (Assumendo 256 pagine per blocco) */
    uint32_t row = (block_address << 8); 
    
    NAND_Address(row & 0xFF);         
    NAND_Address((row >> 8) & 0xFF);  
    NAND_Address((row >> 16) & 0xFF); 
    
    NAND_Command(0xD0); 
    
    HW_NAND_Wait_Ready(false); 
    CE_F3_Set();
}

void HW_NAND_Write_Codeword(uint32_t page_address, uint8_t* payload, uint8_t* parity) {
    CE_F3_Clear(); 
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
    HW_NAND_Wait_Ready(false); 
    CE_F3_Set(); 
}

void HW_NAND_Read_Codeword(uint32_t page_address, uint8_t* payload, uint8_t* parity) {
    CE_F3_Clear(); 
    NAND_Command(0x00); 

    NAND_Address(0x00); 
    NAND_Address(0x00); 
    NAND_Address(page_address & 0xFF);         
    NAND_Address((page_address >> 8) & 0xFF);  
    NAND_Address((page_address >> 16) & 0xFF); 

    NAND_Command(0x30); 
    HW_NAND_Wait_Ready(true); // Ripristina la modalità lettura inviando 0x00

    for(uint32_t i = 0; i < PAYLOAD_SIZE_BYTES; i++) {
        payload[i] = NAND_ReadByte();
    }

    NAND_Command(0x05); 
    NAND_Address(0x00); 
    NAND_Address(0x10); 
    NAND_Command(0xE0); 

    /* NOTA CRITICA: Nessun Wait_Ready dopo E0h perché il chip non va in BUSY.
       È sufficiente un piccolissimo ritardo hardware per stabilizzare la linea. */
    for(volatile uint32_t d = 0; d < 50; d++) { asm("nop"); }

    for(uint32_t i = 0; i < PARITY_SIZE_BYTES; i++) {
        parity[i] = NAND_ReadByte();
    }

    CE_F3_Set();
}

// =============================================================================
// 4. MAIN ENTRY POINT & CUSTOM STATE MACHINE
// =============================================================================

uint8_t CACHE_ALIGN rx_buffer[PAYLOAD_SIZE_BYTES];
uint8_t CACHE_ALIGN tx_buffer[CODEWORD_SIZE_BYTES + 2]; 
uint8_t codeword_buffer[CODEWORD_SIZE_BYTES];
uint8_t flash_read_buffer[CODEWORD_SIZE_BYTES];
uint32_t current_nand_page = 0x00000000;
uint32_t current_block     = 0x00000000;
bool block_erased = false;

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
    /* Inizializzazione globale di tutti i moduli di sistema */
    SYS_Initialize ( NULL );
    
    /* Configurazione e attivazione alimentazione NAND tramite pin PB1 */
    PMC_REGS->PMC_PCER0 = (1UL << 11);
    PIOB_REGS->PIO_WPMR = (0x50494FUL << 8);
    PIOB_REGS->PIO_OER = (1UL << 1);
    PIOB_REGS->PIO_SODR = (1UL << 1);
    for(volatile int d = 0; d < 100000; d++) { asm("nop"); }
    
    /* Sveglia il chip subito dopo aver dato corrente */
    NAND_Command(0xFF);
    HW_NAND_Wait_Ready(false);

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
        /* Esegue i task in background dei moduli gestiti da Harmony */
        SYS_Tasks ( );

        if (appData.isConfigured == true)
        {
            /* HIJACK DEL PROTOCOLLO: Blocca lo stato di app.c per impedire
               che sottragga i pacchetti USB dalla nostra routine personalizzata */
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
                    /* La NAND va cancellata prima di (ri)programmare. current_nand_page
                       e' un row address completo -> block = page >> 8 (256 pagine per
                       blocco). Cancella il blocco la prima volta che lo si tocca, anche
                       dopo il wrap a pagina 256 (altrimenti uno stress test lungo ricomincia
                       a corrompere appena entra in un blocco non cancellato). */
                    if (!block_erased || (current_nand_page >> 8) != current_block) {
                        current_block = current_nand_page >> 8;
                        HW_NAND_Erase_Block(current_block);
                        block_erased  = true;
                    }

                    gf2_encode_data(rx_buffer, PAYLOAD_SIZE_BYTES, codeword_buffer);
                    inject_errors(codeword_buffer, CODEWORD_SIZE_BYTES, TARGET_BIT_FLIPS);

                    HW_NAND_Write_Codeword(current_nand_page, codeword_buffer, &codeword_buffer[PAYLOAD_SIZE_BYTES]);
                    HW_NAND_Read_Codeword(current_nand_page, flash_read_buffer, &flash_read_buffer[PAYLOAD_SIZE_BYTES]);

                    /* Avanza alla pagina successiva del blocco per il prossimo pacchetto */
                    current_nand_page++; 

                    uint8_t crc_status = 0;
                    int bch_status = gf2_correct_errors(flash_read_buffer, CODEWORD_SIZE_BYTES, &crc_status);

                    /* Estrae il payload corretto nei primi 512 byte della risposta.
                       ATTENZIONE: il payload NON sono i primi 512 byte del codeword
                       (e' spostato in alto di 58 bit = 26 BCH + 32 CRC); senza questo
                       passaggio l'host vedeva "corruzione" totale anche con 0 errori. */
                    gf2_extract_payload(flash_read_buffer, CODEWORD_SIZE_BYTES,
                                        tx_buffer, PAYLOAD_SIZE_BYTES);

                    /* Gli 8 byte di parita' = i byte bassi del codeword (regione BCH+CRC). */
                    for(int i = 0; i < PARITY_SIZE_BYTES; i++) {
                        tx_buffer[PAYLOAD_SIZE_BYTES + i] = flash_read_buffer[i];
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

    return ( EXIT_FAILURE );
}