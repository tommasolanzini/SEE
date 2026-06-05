/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app.c

  Summary:
    COM1 loopback + COM2 NAND Flash (F3 / MT29F32G08CBACA) controller.

  Description:
    COM1 : unchanged echo/loopback.
    COM2 command interface (send a single ASCII character):
      'w'  – MCU replies "SEND32\r\n", then accepts the next 32 bytes and
             writes them to page 0 of block 0 of flash F3.
      'r'  – reads the first full page (4320 bytes, data + spare) from
             block 0 of F3 and streams it back over COM2.
      'e'  – erases block 0 of F3.
    On success / failure the MCU replies with a short status string, e.g.
    "OK:WRITE\r\n" or "ERR:WRITE\r\n".

  ── NAND bus wiring (from plib_pio.h) ─────────────────────────────────────
    Signal   Pin     Macro
    ──────   ───     ─────
    CE#      PD21    CE_F3_Set() / CE_F3_Clear()
    R/B#     PC1     F3_RB_Get()
    WE#      PD19    NAND_WE_Set() / NAND_WE_Clear()
    RE# (OE) PA10    NAND_OE_Set() / NAND_OE_Clear()
    CLE      PA12    NAND_CLE_Set() / NAND_CLE_Clear()
    ALE      PD18    NAND_ALE_Set() / NAND_ALE_Clear()
    WP#      PA27    NAND_WP_Set() / NAND_WP_Clear()

    Data bus (D[7:0]) is scattered across three ports:
      D0 → PD25     D1 → PD26     D2 → PC6      D3 → PD24
      D4 → PA24     D5 → PD23     D6 → PC5      D7 → PA25

  ── NAND geometry (MT29F32G08CBACA, x8 async) ─────────────────────────────
    Page size  : 4096 + 224 bytes (4320 total)
    Block size : 256 pages
    5-cycle addressing:
      CA0 = col[7:0]    CA1 = col[12:8]
      RA0 = row[7:0]    RA1 = row[15:8]    RA2 = row[23:16]
      row = (block << 8) | page

  ── Important notes ────────────────────────────────────────────────────────
    • No ECC is applied. For production, enable PMECC or software BCH/Hamming.
    • Pages must be programmed sequentially within a block (0 → 255).
      Always erase ('e') before writing ('w') to a previously used block.
    • The NAND_Reset() call at the end of APP_Initialize() assumes PIO is
      already configured by the Harmony system layer. The PIO_Initialize()
      call in SYS_Initialize() must run first.
 *******************************************************************************/

#include "app.h"
#include "peripheral/pio/plib_pio.h"
#include <string.h>

// *****************************************************************************
// NAND timing delays
// Loop counts are conservative for a 300 MHz ATSAMV71Q21B (one loop ≈ 3–4 ns).
// Increase if you clock the core higher or if the compiler optimises the loops.
// *****************************************************************************
#define DELAY_NS(ns)   do { for (volatile uint32_t _d = 0; _d < ((ns)/4U + 1U); _d++) {} } while(0)

/* Key datasheet minimums (asynchronous mode 0, tRC/tWC = 100 ns) */
#define tCS_NS      70U   /* CE# setup before WE#/RE# falls         */
#define tCH_NS      20U   /* CE# setup before WE#/RE# falls         */
#define tCLS_NS     50U   /* CLE setup before WE# falls             */
#define tALS_NS     50U   /* ALE setup before WE# falls             */
#define tDS_NS      40U   /* Data setup before WE# rises            */
#define tWP_NS      50U   /* WE# pulse width (low)                  */
#define tWH_NS      30U   /* WE# hold after rising edge             */
#define tRP_NS      50U   /* RE# pulse width (low)                  */
#define tREH_NS     30U   /* RE# hold after rising edge             */
#define tADL_NS     200U   /* Address-to-data loading delay          */
#define tWHR_NS     120U   /* WE# high to RE# low (status read)      */
#define tRR_NS      40U   /* R/B# high to RE# low                   */
#define tWB_NS     200U   /* WE# high to R/B# low (after cmd)       */

/* Busy-wait loop limit (≈ 100 ms at 300 MHz – covers worst-case tBERS) */
#define NAND_TIMEOUT_LOOPS   25000000UL

/* NAND geometry */
#define NAND_PAGE_DATA       4096U
#define NAND_PAGE_SPARE       224U
#define NAND_PAGE_TOTAL      (NAND_PAGE_DATA + NAND_PAGE_SPARE)

/* COM2 write payload size */
#define COM2_WRITE_LEN        32U

/* NAND status bits */
#define NAND_SR_FAIL          (1U << 0)
#define NAND_SR_READY         (1U << 6)

// *****************************************************************************
// Global application data
// *****************************************************************************

APP_DATA appData;

uint8_t com1ReadBuffer[APP_READ_BUFFER_SIZE]  USB_ALIGN;
uint8_t com1WriteBuffer[APP_READ_BUFFER_SIZE] USB_ALIGN;
uint8_t com2ReadBuffer[APP_READ_BUFFER_SIZE]  USB_ALIGN;
uint8_t com2WriteBuffer[APP_READ_BUFFER_SIZE] USB_ALIGN;

/* Full page buffer for NAND read result */
static uint8_t nandPageBuf[NAND_PAGE_TOTAL];

/* COM2 protocol state machine */
typedef enum {
    COM2_IDLE,           /* Waiting for 'w', 'r', or 'e'          */
    COM2_COLLECT_WRITE,  /* Accumulating 32 bytes for page write   */
    COM2_SEND_READ,      /* Streaming page data back to host       */
} COM2_STATE;

static COM2_STATE com2State       = COM2_IDLE;
static uint16_t   com2WriteOffset = 0;
static uint16_t   com2SendOffset  = 0;
static uint8_t    nandWriteBuf[COM2_WRITE_LEN];


// *****************************************************************************
// ── Low-level NAND bit-bang driver ──────────────────────────────────────────
// *****************************************************************************

/* ---- Data bus helpers ---------------------------------------------------- */

/*
 * Drive D[7:0] onto the scattered GPIO pins.
 * Uses SODR/CODR for atomic set/clear without read-modify-write.
 *
 * Bit → Pin mapping:
 *   D0→PD25  D1→PD26  D2→PC6   D3→PD24
 *   D4→PA24  D5→PD23  D6→PC5   D7→PA25
 */
static inline void NAND_BusWrite(uint8_t byte)
{
    /* ── PIOD: bits 23,24,25,26 ── */
    /* Build the four-bit pattern in its place then set/clear atomically */
    uint32_t pd_set = 0U, pd_clr = 0U;
    if (byte & 0x01U) pd_set |= (1UL << 25); else pd_clr |= (1UL << 25); /* D0 */
    if (byte & 0x02U) pd_set |= (1UL << 26); else pd_clr |= (1UL << 26); /* D1 */
    if (byte & 0x08U) pd_set |= (1UL << 24); else pd_clr |= (1UL << 24); /* D3 */
    if (byte & 0x20U) pd_set |= (1UL << 23); else pd_clr |= (1UL << 23); /* D5 */
    if (pd_set) PIOD_REGS->PIO_SODR = pd_set;
    if (pd_clr) PIOD_REGS->PIO_CODR = pd_clr;

    /* ── PIOC: bits 5,6 ── */
    uint32_t pc_set = 0U, pc_clr = 0U;
    if (byte & 0x04U) pc_set |= (1UL << 6);  else pc_clr |= (1UL << 6);  /* D2 */
    if (byte & 0x40U) pc_set |= (1UL << 5);  else pc_clr |= (1UL << 5);  /* D6 */
    if (pc_set) PIOC_REGS->PIO_SODR = pc_set;
    if (pc_clr) PIOC_REGS->PIO_CODR = pc_clr;

    /* ── PIOA: bits 24,25 ── */
    uint32_t pa_set = 0U, pa_clr = 0U;
    if (byte & 0x10U) pa_set |= (1UL << 24); else pa_clr |= (1UL << 24); /* D4 */
    if (byte & 0x80U) pa_set |= (1UL << 25); else pa_clr |= (1UL << 25); /* D7 */
    if (pa_set) PIOA_REGS->PIO_SODR = pa_set;
    if (pa_clr) PIOA_REGS->PIO_CODR = pa_clr;
}

/*
 * Sample D[7:0] from the GPIO input registers and assemble the byte.
 */
static inline uint8_t NAND_BusRead(void)
{
    uint32_t pd = PIOD_REGS->PIO_PDSR;
    uint32_t pc = PIOC_REGS->PIO_PDSR;
    uint32_t pa = PIOA_REGS->PIO_PDSR;

    uint8_t byte = 0U;
    if (pd & (1UL << 25)) byte |= 0x01U; /* D0 */
    if (pd & (1UL << 26)) byte |= 0x02U; /* D1 */
    if (pc & (1UL << 6))  byte |= 0x04U; /* D2 */
    if (pd & (1UL << 24)) byte |= 0x08U; /* D3 */
    if (pa & (1UL << 24)) byte |= 0x10U; /* D4 */
    if (pd & (1UL << 23)) byte |= 0x20U; /* D5 */
    if (pc & (1UL << 5))  byte |= 0x40U; /* D6 */
    if (pa & (1UL << 25)) byte |= 0x80U; /* D7 */
    return byte;
}

/* Switch all 8 data pins to output (for write) */
static inline void NAND_BusOutputEnable(void)
{
    NAND_D0_OutputEnable();
    NAND_D1_OutputEnable();
    NAND_D2_OutputEnable();
    NAND_D3_OutputEnable();
    NAND_D4_OutputEnable();
    NAND_D5_OutputEnable();
    NAND_D6_OutputEnable();
    NAND_D7_OutputEnable();
}

/* Switch all 8 data pins to input (for read) */
static inline void NAND_BusInputEnable(void)
{
    NAND_D0_InputEnable();
    NAND_D1_InputEnable();
    NAND_D2_InputEnable();
    NAND_D3_InputEnable();
    NAND_D4_InputEnable();
    NAND_D5_InputEnable();
    NAND_D6_InputEnable();
    NAND_D7_InputEnable();
}

/* ---- Single-byte bus cycles ---------------------------------------------- */

/*
 * Latch one command byte (CLE high, ALE low, one WE# pulse).
 */
static void NAND_WriteCmd(uint8_t cmd)
{
    NAND_CLE_Set();
    NAND_ALE_Clear();
    DELAY_NS(tCLS_NS);

    NAND_BusOutputEnable();
    NAND_BusWrite(cmd);
    DELAY_NS(tDS_NS);

    NAND_WE_Clear();
    DELAY_NS(tWP_NS);
    NAND_WE_Set();
    DELAY_NS(tWH_NS);

    NAND_CLE_Clear();
    DELAY_NS(tCLS_NS);
}

/*
 * Latch one address byte (ALE high, CLE low, one WE# pulse).
 */
static void NAND_WriteAddr(uint8_t addr)
{
    NAND_ALE_Set();
    NAND_CLE_Clear();
    DELAY_NS(tALS_NS);

    NAND_BusOutputEnable();
    NAND_BusWrite(addr);
    DELAY_NS(tDS_NS);

    NAND_WE_Clear();
    DELAY_NS(tWP_NS);
    NAND_WE_Set();
    DELAY_NS(tWH_NS);

    NAND_ALE_Clear();
    DELAY_NS(tALS_NS);
}

/*
 * Write one data byte (CLE low, ALE low, one WE# pulse).
 * Bus must already be in output mode.
 */
static inline void NAND_WriteDataByte(uint8_t data)
{
    NAND_BusWrite(data);
    DELAY_NS(tDS_NS);
    NAND_WE_Clear();
    DELAY_NS(tWP_NS);
    NAND_WE_Set();
    DELAY_NS(tWH_NS);
}

/*
 * Read one data byte (CLE low, ALE low, one RE# pulse).
 * Bus must already be in input mode.
 */
static inline uint8_t NAND_ReadDataByte(void)
{
    NAND_OE_Clear();
    DELAY_NS(tRP_NS);
    uint8_t data = NAND_BusRead();
    NAND_OE_Set();
    DELAY_NS(tREH_NS);
    return data;
}

/* ---- Ready/Busy polling -------------------------------------------------- */

/*
 * Wait for R/B# (F3_RB, PC1) to go HIGH.
 * Returns true on ready, false on timeout.
 */
static bool NAND_WaitReady(void)
{
    DELAY_NS(tWB_NS); /* tWB: WE# high to R/B# going low */
    uint32_t timeout = NAND_TIMEOUT_LOOPS;
    while (timeout--)
    {
        if (F3_RB_Get())
            return true;
    }
    return false;
}

/* ---- Chip-select helpers ------------------------------------------------- */

static inline void NAND_Select(void)
{
    CE_F3_Clear(); /* CE# active low */
    DELAY_NS(tCS_NS);
}

static inline void NAND_Deselect(void)
{
    DELAY_NS(tCH_NS);
    CE_F3_Set();
}

/* ---- Read STATUS (70h) --------------------------------------------------- */

static uint8_t NAND_ReadStatus(void)
{
    NAND_WriteCmd(0x70U);
    DELAY_NS(tWHR_NS);
    NAND_BusInputEnable();
    uint8_t sr = NAND_ReadDataByte();
    return sr;
}

/* ---- Public NAND operations --------------------------------------------- */

/*
 * NAND_Reset – send FFh and wait for device to become ready.
 * Must be the first command after power-on.
 */
static void NAND_Reset(void)
{
    NAND_WP_Set();      /* WP# high: writes/erases enabled */
    NAND_WE_Set();
    NAND_OE_Set();
    NAND_CLE_Clear();
    NAND_ALE_Clear();

    NAND_Select();
    NAND_WriteCmd(0xFFU);
    NAND_WaitReady();
    NAND_Deselect();
}

/*
 * NAND_ReadPage
 *   block  : block address (0 … 4095 for MT29F32G08CBACA)
 *   page   : page within block (0 … 255)
 *   buf    : output buffer, must be at least NAND_PAGE_TOTAL bytes
 *
 * Command sequence: 00h – 5× addr – 30h – wait R/B# – read bytes
 * Row address (24-bit): bits[7:0] = page, bits[19:8] = block
 */
static bool NAND_ReadPage(uint32_t block, uint32_t page, uint8_t *buf)
{
    uint32_t row = (block << 8U) | (page & 0xFFU);
    uint16_t col = 0U;

    NAND_Select();

    NAND_WriteCmd(0x00U);
    NAND_WriteAddr((uint8_t)(col & 0xFFU));           /* CA0 */
    NAND_WriteAddr((uint8_t)((col >> 8U) & 0x1FU));   /* CA1 */
    NAND_WriteAddr((uint8_t)(row & 0xFFU));            /* RA0 */
    NAND_WriteAddr((uint8_t)((row >> 8U) & 0xFFU));   /* RA1 */
    NAND_WriteAddr((uint8_t)((row >> 16U) & 0xFFU));  /* RA2 */
    NAND_WriteCmd(0x30U);

    bool ready = NAND_WaitReady();
    if (!ready)
    {
        NAND_Deselect();
        return false;
    }

    DELAY_NS(tRR_NS); /* tRR: R/B# high to RE# low */

    NAND_BusInputEnable();
    NAND_CLE_Clear();
    NAND_ALE_Clear();

    for (uint32_t i = 0U; i < NAND_PAGE_TOTAL; i++)
        buf[i] = NAND_ReadDataByte();

    NAND_Deselect();
    return true;
}

/*
 * NAND_WritePage
 *   block  : block address
 *   page   : page within block (must be 0 for first write in freshly erased block)
 *   buf    : data to write (len bytes); remainder of page padded with 0xFF
 *
 * Command sequence: 80h – 5× addr – data bytes – 10h – wait R/B# – check FAIL
 */
static bool NAND_WritePage(uint32_t block, uint32_t page,
                           const uint8_t *buf, uint16_t len)
{
    uint32_t row = (block << 8U) | (page & 0xFFU);
    uint16_t col = 0U;

    NAND_Select();

    NAND_WriteCmd(0x80U);
    NAND_WriteAddr((uint8_t)(col & 0xFFU));
    NAND_WriteAddr((uint8_t)((col >> 8U) & 0x1FU));
    NAND_WriteAddr((uint8_t)(row & 0xFFU));
    NAND_WriteAddr((uint8_t)((row >> 8U) & 0xFFU));
    NAND_WriteAddr((uint8_t)((row >> 16U) & 0xFFU));

    DELAY_NS(tADL_NS); /* tADL: last address to first data */

    NAND_CLE_Clear();
    NAND_ALE_Clear();
    NAND_BusOutputEnable();

    for (uint16_t i = 0U; i < len; i++)
        NAND_WriteDataByte(buf[i]);

    /* Pad the rest of the page with 0xFF to leave unprogrammed bits untouched */
    for (uint32_t i = (uint32_t)len; i < NAND_PAGE_TOTAL; i++)
        NAND_WriteDataByte(0xFFU);

    NAND_WriteCmd(0x10U); /* confirm / commit */

    bool ready = NAND_WaitReady();
    if (!ready)
    {
        NAND_Deselect();
        return false;
    }

    uint8_t sr = NAND_ReadStatus();

    /* Re-enter data-output mode so the bus is in a clean state */
    NAND_WriteCmd(0x00U);

    NAND_Deselect();
    return ((sr & NAND_SR_FAIL) == 0U);
}

/*
 * NAND_EraseBlock
 *   block : block address to erase
 *
 * Command sequence: 60h – 3× row addr (page bits ignored) – D0h – wait R/B# – check FAIL
 */
static bool NAND_EraseBlock(uint32_t block)
{
    /* Row address for erase: page field is don't-care; fill with 0 */
    uint32_t row = (block << 8U);

    NAND_Select();

    NAND_WriteCmd(0x60U);
    NAND_WriteAddr((uint8_t)(row & 0xFFU));
    NAND_WriteAddr((uint8_t)((row >> 8U) & 0xFFU));
    NAND_WriteAddr((uint8_t)((row >> 16U) & 0xFFU));
    NAND_WriteCmd(0xD0U);

    bool ready = NAND_WaitReady();
    if (!ready)
    {
        NAND_Deselect();
        return false;
    }

    uint8_t sr = NAND_ReadStatus();
    NAND_WriteCmd(0x00U);

    NAND_Deselect();
    return ((sr & NAND_SR_FAIL) == 0U);
}


// *****************************************************************************
// ── USB CDC Event Handler ────────────────────────────────────────────────────
// *****************************************************************************

USB_DEVICE_CDC_EVENT_RESPONSE APP_USBDeviceCDCEventHandler(
    USB_DEVICE_CDC_INDEX index,
    USB_DEVICE_CDC_EVENT event,
    void *pData,
    uintptr_t userData)
{
    APP_DATA *appDataObject = (APP_DATA *)userData;
    USB_CDC_CONTROL_LINE_STATE *controlLineStateData;
    uint16_t *breakData;
    USB_DEVICE_CDC_EVENT_DATA_READ_COMPLETE *eventDataRead;

    switch (event)
    {
        case USB_DEVICE_CDC_EVENT_GET_LINE_CODING:
            USB_DEVICE_ControlSend(appDataObject->deviceHandle,
                    &appDataObject->appCOMPortObjects[index].getLineCodingData,
                    sizeof(USB_CDC_LINE_CODING));
            break;

        case USB_DEVICE_CDC_EVENT_SET_LINE_CODING:
            USB_DEVICE_ControlReceive(appDataObject->deviceHandle,
                    &appDataObject->appCOMPortObjects[index].setLineCodingData,
                    sizeof(USB_CDC_LINE_CODING));
            break;

        case USB_DEVICE_CDC_EVENT_SET_CONTROL_LINE_STATE:
            controlLineStateData = (USB_CDC_CONTROL_LINE_STATE *)pData;
            appDataObject->appCOMPortObjects[index].controlLineStateData.dtr     = controlLineStateData->dtr;
            appDataObject->appCOMPortObjects[index].controlLineStateData.carrier = controlLineStateData->carrier;
            USB_DEVICE_ControlStatus(appDataObject->deviceHandle,
                                     USB_DEVICE_CONTROL_STATUS_OK);
            break;

        case USB_DEVICE_CDC_EVENT_SEND_BREAK:
            breakData = (uint16_t *)pData;
            appDataObject->appCOMPortObjects[index].breakData = *breakData;
            USB_DEVICE_ControlStatus(appDataObject->deviceHandle,
                                     USB_DEVICE_CONTROL_STATUS_OK);
            break;

        case USB_DEVICE_CDC_EVENT_READ_COMPLETE:
            eventDataRead = (USB_DEVICE_CDC_EVENT_DATA_READ_COMPLETE *)pData;
            if (eventDataRead->status != USB_DEVICE_CDC_RESULT_ERROR)
            {
                appDataObject->appCOMPortObjects[index].readDataLength =
                        eventDataRead->length;
                appDataObject->appCOMPortObjects[index].isReadComplete = true;
            }
            break;

        case USB_DEVICE_CDC_EVENT_CONTROL_TRANSFER_DATA_RECEIVED:
            USB_DEVICE_ControlStatus(appDataObject->deviceHandle,
                                     USB_DEVICE_CONTROL_STATUS_OK);
            break;

        case USB_DEVICE_CDC_EVENT_CONTROL_TRANSFER_DATA_SENT:
            break;

        case USB_DEVICE_CDC_EVENT_WRITE_COMPLETE:
            appDataObject->appCOMPortObjects[index].isWriteComplete = true;
            break;

        default:
            break;
    }
    return USB_DEVICE_CDC_EVENT_RESPONSE_NONE;
}


// *****************************************************************************
// ── USB Device Layer Event Handler ──────────────────────────────────────────
// *****************************************************************************

void APP_USBDeviceEventHandler(USB_DEVICE_EVENT event, void *pData,
                               uintptr_t context)
{
    uint8_t configurationValue;

    switch (event)
    {
        case USB_DEVICE_EVENT_RESET:
        case USB_DEVICE_EVENT_DECONFIGURED:
            appData.isConfigured = false;
            break;

        case USB_DEVICE_EVENT_CONFIGURED:
            configurationValue =
                    ((USB_DEVICE_EVENT_DATA_CONFIGURED *)pData)->configurationValue;
            if (configurationValue == 1U)
            {
                USB_DEVICE_CDC_EventHandlerSet(COM1, APP_USBDeviceCDCEventHandler,
                                               (uintptr_t)&appData);
                USB_DEVICE_CDC_EventHandlerSet(COM2, APP_USBDeviceCDCEventHandler,
                                               (uintptr_t)&appData);
                appData.isConfigured = true;
            }
            break;

        case USB_DEVICE_EVENT_POWER_DETECTED:
            USB_DEVICE_Attach(appData.deviceHandle);
            break;

        case USB_DEVICE_EVENT_POWER_REMOVED:
            USB_DEVICE_Detach(appData.deviceHandle);
            appData.isConfigured = false;
            break;

        case USB_DEVICE_EVENT_SUSPENDED:
        case USB_DEVICE_EVENT_RESUMED:
        case USB_DEVICE_EVENT_ERROR:
        default:
            break;
    }
}


// *****************************************************************************
// ── Local helpers ────────────────────────────────────────────────────────────
// *****************************************************************************

static void APP_StateReset(void)
{
    appData.appCOMPortObjects[COM1].isReadComplete  = false;
    appData.appCOMPortObjects[COM1].isWriteComplete = false;
    appData.appCOMPortObjects[COM2].isReadComplete  = false;
    appData.appCOMPortObjects[COM2].isWriteComplete = false;
    com2State       = COM2_IDLE;
    com2WriteOffset = 0U;
    com2SendOffset  = 0U;
}

/* Queue a short status string onto COM2. */
static void COM2_SendString(const char *str)
{
    uint16_t len = (uint16_t)strlen(str);
    if (len > APP_READ_BUFFER_SIZE)
        len = APP_READ_BUFFER_SIZE;
    memcpy(com2WriteBuffer, str, len);
    appData.appCOMPortObjects[COM2].isWriteComplete = false;
    USB_DEVICE_CDC_Write(COM2,
            &appData.appCOMPortObjects[COM2].writeTransferHandle,
            com2WriteBuffer, len,
            USB_DEVICE_CDC_TRANSFER_FLAGS_DATA_COMPLETE);
}

/*
 * Feed one received byte into the COM2 state machine.
 */
static void COM2_ProcessByte(uint8_t byte)
{
    switch (com2State)
    {
        /* ── IDLE: decode command character ── */
        case COM2_IDLE:
            if (byte == (uint8_t)'w' || byte == (uint8_t)'W')
            {
                com2WriteOffset = 0U;
                memset(nandWriteBuf, 0xFFU, COM2_WRITE_LEN);
                com2State = COM2_COLLECT_WRITE;
                COM2_SendString("SEND32\r\n");
            }
            else if (byte == (uint8_t)'r' || byte == (uint8_t)'R')
            {
                bool ok = NAND_ReadPage(0U, 0U, nandPageBuf);
                if (ok)
                {
                    com2SendOffset = 0U;
                    com2State = COM2_SEND_READ;
                    /* Send the first chunk; subsequent chunks are driven by
                       the isWriteComplete flag in APP_Tasks. */
                    uint16_t chunk = (NAND_PAGE_TOTAL < (uint32_t)APP_READ_BUFFER_SIZE)
                                     ? (uint16_t)NAND_PAGE_TOTAL
                                     : (uint16_t)APP_READ_BUFFER_SIZE;
                    memcpy(com2WriteBuffer, nandPageBuf, chunk);
                    com2SendOffset = chunk;
                    appData.appCOMPortObjects[COM2].isWriteComplete = false;
                    USB_DEVICE_CDC_Write(COM2,
                            &appData.appCOMPortObjects[COM2].writeTransferHandle,
                            com2WriteBuffer, chunk,
                            USB_DEVICE_CDC_TRANSFER_FLAGS_DATA_COMPLETE);
                }
                else
                {
                    COM2_SendString("ERR:READ\r\n");
                }
            }
            else if (byte == (uint8_t)'e' || byte == (uint8_t)'E')
            {
                bool ok = NAND_EraseBlock(0U);
                COM2_SendString(ok ? "OK:ERASE\r\n" : "ERR:ERASE\r\n");
            }
            break;

        /* ── COLLECT_WRITE: gather 32 bytes then write ── */
        case COM2_COLLECT_WRITE:
            nandWriteBuf[com2WriteOffset++] = byte;
            if (com2WriteOffset >= COM2_WRITE_LEN)
            {
                bool ok = NAND_WritePage(0U, 0U, nandWriteBuf, COM2_WRITE_LEN);
                COM2_SendString(ok ? "OK:WRITE\r\n" : "ERR:WRITE\r\n");
                com2State = COM2_IDLE;
            }
            break;

        /* ── SEND_READ: ignore input while streaming ── */
        case COM2_SEND_READ:
            break;
    }
}


// *****************************************************************************
// ── Initialization ───────────────────────────────────────────────────────────
// *****************************************************************************

void APP_Initialize(void)
{
    appData.deviceHandle  = USB_DEVICE_HANDLE_INVALID;
    appData.isConfigured  = false;
    appData.state         = APP_STATE_INIT;

    /* Line coding for both ports */
    for (int i = 0; i < 2; i++)
    {
        appData.appCOMPortObjects[i].getLineCodingData.dwDTERate   = 9600;
        appData.appCOMPortObjects[i].getLineCodingData.bDataBits   = 8;
        appData.appCOMPortObjects[i].getLineCodingData.bParityType = 0;
        appData.appCOMPortObjects[i].getLineCodingData.bCharFormat = 0;
        appData.appCOMPortObjects[i].readTransferHandle  =
                USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
        appData.appCOMPortObjects[i].writeTransferHandle =
                USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
        appData.appCOMPortObjects[i].isReadComplete  = true;
        appData.appCOMPortObjects[i].isWriteComplete = false;
        appData.appCOMPortObjects[i].readDataLength  = 1;
    }

    com2State       = COM2_IDLE;
    com2WriteOffset = 0U;
    com2SendOffset  = 0U;

    /* Initialise control lines to idle state before reset */
    CE_F3_Set();         /* CE# deasserted (high) */
    NAND_WE_Set();       /* WE# idle (high)       */
    NAND_OE_Set();       /* RE# idle (high)       */
    NAND_CLE_Clear();
    NAND_ALE_Clear();
    NAND_WP_Set();       /* WP# high: write/erase enabled */

    /* Data bus to output mode with 0xFF driven out (safe idle) */
    NAND_BusOutputEnable();
    NAND_BusWrite(0xFFU);

    /* Issue mandatory power-on RESET to flash F3.
       PIO_Initialize() must have already been called by SYS_Initialize(). */
    NAND_Reset();
}


// *****************************************************************************
// ── Application State Machine ────────────────────────────────────────────────
// *****************************************************************************

void APP_Tasks(void)
{
    switch (appData.state)
    {
        /* ── Open the USB device layer ── */
        case APP_STATE_INIT:
            appData.deviceHandle = USB_DEVICE_Open(USB_DEVICE_INDEX_0,
                                                    DRV_IO_INTENT_READWRITE);
            if (appData.deviceHandle != USB_DEVICE_HANDLE_INVALID)
            {
                USB_DEVICE_EventHandlerSet(appData.deviceHandle,
                                           APP_USBDeviceEventHandler, 0);
                appData.state = APP_STATE_WAIT_FOR_CONFIGURATION;
            }
            break;

        /* ── Wait until USB host configures the device ── */
        case APP_STATE_WAIT_FOR_CONFIGURATION:
            if (appData.isConfigured)
            {
                appData.state = APP_STATE_CHECK_IF_CONFIGURED;

                appData.appCOMPortObjects[COM1].isReadComplete = false;
                appData.appCOMPortObjects[COM2].isReadComplete = false;

                USB_DEVICE_CDC_Read(COM1,
                        &appData.appCOMPortObjects[COM1].readTransferHandle,
                        com1ReadBuffer, APP_READ_BUFFER_SIZE);
                USB_DEVICE_CDC_Read(COM2,
                        &appData.appCOMPortObjects[COM2].readTransferHandle,
                        com2ReadBuffer, APP_READ_BUFFER_SIZE);
            }
            break;

        /* ── Verify still configured ── */
        case APP_STATE_CHECK_IF_CONFIGURED:
            if (appData.isConfigured)
                appData.state = APP_STATE_CHECK_FOR_READ_COMPLETE;
            else
            {
                APP_StateReset();
                appData.state = APP_STATE_WAIT_FOR_CONFIGURATION;
            }
            break;

        /* ── Handle received data ── */
        case APP_STATE_CHECK_FOR_READ_COMPLETE:

            /* COM1 – loopback: echo received bytes straight back */
            if (appData.appCOMPortObjects[COM1].isReadComplete)
            {
                appData.appCOMPortObjects[COM1].isReadComplete  = false;
                appData.appCOMPortObjects[COM1].isWriteComplete = false;

                USB_DEVICE_CDC_Write(COM1,
                        &appData.appCOMPortObjects[COM1].writeTransferHandle,
                        com1ReadBuffer,
                        appData.appCOMPortObjects[COM1].readDataLength,
                        USB_DEVICE_CDC_TRANSFER_FLAGS_DATA_COMPLETE);
            }

            /* COM2 – NAND command interpreter */
            if (appData.appCOMPortObjects[COM2].isReadComplete)
            {
                appData.appCOMPortObjects[COM2].isReadComplete = false;

                uint16_t rxLen =
                        appData.appCOMPortObjects[COM2].readDataLength;

                /* Process incoming bytes only when:
                   – we are collecting write data (bytes are data, not commands), OR
                   – the previous USB write has completed (prevents queueing two
                     writes at once and overwriting com2WriteBuffer mid-flight). */
                if (com2State == COM2_COLLECT_WRITE ||
                    appData.appCOMPortObjects[COM2].isWriteComplete)
                {
                    for (uint16_t i = 0U; i < rxLen; i++)
                        COM2_ProcessByte(com2ReadBuffer[i]);
                }

                /* Re-arm the COM2 receive */
                USB_DEVICE_CDC_Read(COM2,
                        &appData.appCOMPortObjects[COM2].readTransferHandle,
                        com2ReadBuffer, APP_READ_BUFFER_SIZE);
            }

            appData.state = APP_STATE_CHECK_FOR_WRITE_COMPLETE;
            break;

        /* ── Handle write-complete events ── */
        case APP_STATE_CHECK_FOR_WRITE_COMPLETE:

            /* COM1: re-arm read once write has completed */
            if (appData.appCOMPortObjects[COM1].isWriteComplete)
            {
                appData.appCOMPortObjects[COM1].isWriteComplete = false;
                appData.appCOMPortObjects[COM1].isReadComplete  = false;

                USB_DEVICE_CDC_Read(COM1,
                        &appData.appCOMPortObjects[COM1].readTransferHandle,
                        com1ReadBuffer, APP_READ_BUFFER_SIZE);
            }

            /* COM2: handle ongoing page-read streaming */
            if (appData.appCOMPortObjects[COM2].isWriteComplete)
            {
                appData.appCOMPortObjects[COM2].isWriteComplete = false;

                if (com2State == COM2_SEND_READ)
                {
                    if (com2SendOffset < (uint16_t)NAND_PAGE_TOTAL)
                    {
                        uint16_t remaining =
                                (uint16_t)NAND_PAGE_TOTAL - com2SendOffset;
                        uint16_t chunk =
                                (remaining < (uint16_t)APP_READ_BUFFER_SIZE)
                                ? remaining
                                : (uint16_t)APP_READ_BUFFER_SIZE;

                        memcpy(com2WriteBuffer,
                               nandPageBuf + com2SendOffset, chunk);
                        com2SendOffset += chunk;

                        appData.appCOMPortObjects[COM2].isWriteComplete = false;
                        USB_DEVICE_CDC_Write(COM2,
                                &appData.appCOMPortObjects[COM2].writeTransferHandle,
                                com2WriteBuffer, chunk,
                                USB_DEVICE_CDC_TRANSFER_FLAGS_DATA_COMPLETE);
                    }
                    else
                    {
                        /* Entire page has been streamed */
                        com2State = COM2_IDLE;
                    }
                }
                /* For status strings (IDLE state), write is simply done. */
            }

            appData.state = APP_STATE_CHECK_IF_CONFIGURED;
            break;

        case APP_STATE_ERROR:
        default:
            break;
    }
}

/*******************************************************************************
 End of File
 */