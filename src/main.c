#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include "definitions.h"

// --- USB State Variables ---
static bool is_usb_configured = false;
static bool is_read_complete = false;
static bool is_write_complete = false;
static USB_DEVICE_CDC_TRANSFER_HANDLE readTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
static USB_DEVICE_CDC_TRANSFER_HANDLE writeTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
static USB_DEVICE_HANDLE usbDeviceHandle = USB_DEVICE_HANDLE_INVALID;

// --- Buffers & Hardware Fixes ---
uint8_t CACHE_ALIGN rx_buffer[32]; 
uint8_t CACHE_ALIGN tx_buffer[] = "Hello from MCU!\r\n";
uint8_t CACHE_ALIGN line_coding_dummy[7]; 
uint8_t CACHE_ALIGN get_line_coding_data[7] = {0x00, 0xC2, 0x01, 0x00, 0x00, 0x00, 0x08}; 

// --- CDC Event Callback ---
USB_DEVICE_CDC_EVENT_RESPONSE APP_USBDeviceCDCEventHandler(USB_DEVICE_CDC_INDEX index, USB_DEVICE_CDC_EVENT event, void *pData, uintptr_t userData) {
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

// --- USB Device Event Callback ---
void APP_USBDeviceEventHandler(USB_DEVICE_EVENT event, void *eventData, uintptr_t context) {
    switch (event) {
        case USB_DEVICE_EVENT_CONFIGURED:
            is_usb_configured = true;
            USB_DEVICE_CDC_EventHandlerSet(USB_DEVICE_CDC_INDEX_0, APP_USBDeviceCDCEventHandler, 0);
            break;
        case USB_DEVICE_EVENT_SUSPENDED:
        case USB_DEVICE_EVENT_RESET:
        case USB_DEVICE_EVENT_DECONFIGURED:
            is_usb_configured = false;
            break;
        default:
            break;
    }
}

// --- Main Loop ---
int main(void)
{
    SYS_Initialize(NULL);

    enum { WAIT_FOR_CONFIG, WAIT_FOR_DEBOUNCE, WAIT_FOR_READY, WAIT_FOR_TRIGGER, WAIT_FOR_READ_COMPLETE, SEND_MESSAGE, WAIT_FOR_TRANSFER } appState = WAIT_FOR_CONFIG;
    uint32_t debounce_counter = 0;

    while (true)
    {
        SYS_Tasks();

        switch (appState)
        {
            case WAIT_FOR_CONFIG:
                if (usbDeviceHandle == USB_DEVICE_HANDLE_INVALID)
                {
                    usbDeviceHandle = USB_DEVICE_Open(USB_DEVICE_INDEX_0, DRV_IO_INTENT_READWRITE);
                    if (usbDeviceHandle != USB_DEVICE_HANDLE_INVALID)
                    {
                        USB_DEVICE_EventHandlerSet(usbDeviceHandle, APP_USBDeviceEventHandler, 0);
                        appState = WAIT_FOR_DEBOUNCE;
                        debounce_counter = 0;
                    }
                }
                break;

            case WAIT_FOR_DEBOUNCE:
                debounce_counter++;
                if (debounce_counter > 1000000) 
                {
                    USB_DEVICE_Attach(usbDeviceHandle);
                    appState = WAIT_FOR_READY;
                }
                break;

            case WAIT_FOR_READY:
                if (is_usb_configured)
                {
                    appState = WAIT_FOR_TRIGGER;
                }
                break;

            case WAIT_FOR_TRIGGER:
                if (!is_usb_configured) { appState = WAIT_FOR_CONFIG; break; }
                
                is_read_complete = false;
                // STUBBORN FIX: Only advance if the hardware guarantees the read was queued successfully.
                if (USB_DEVICE_CDC_Read(USB_DEVICE_CDC_INDEX_0, &readTransferHandle, rx_buffer, sizeof(rx_buffer)) == USB_DEVICE_CDC_RESULT_OK)
                {
                    appState = WAIT_FOR_READ_COMPLETE;
                }
                break;

            case WAIT_FOR_READ_COMPLETE:
                if (!is_usb_configured) { appState = WAIT_FOR_CONFIG; break; }
                
                if (is_read_complete)
                {
                    appState = SEND_MESSAGE;
                }
                break;

            case SEND_MESSAGE:
                if (!is_usb_configured) { appState = WAIT_FOR_CONFIG; break; }
                
                is_write_complete = false;
                // STUBBORN FIX: Only advance if the hardware guarantees the write was queued.
                if (USB_DEVICE_CDC_Write(USB_DEVICE_CDC_INDEX_0, &writeTransferHandle, tx_buffer, sizeof(tx_buffer) - 1, USB_DEVICE_CDC_TRANSFER_FLAGS_DATA_COMPLETE) == USB_DEVICE_CDC_RESULT_OK)
                {
                    appState = WAIT_FOR_TRANSFER;
                }
                break;

            case WAIT_FOR_TRANSFER:
                if (!is_usb_configured) { appState = WAIT_FOR_CONFIG; break; }
                
                if (is_write_complete)
                {
                    appState = WAIT_FOR_TRIGGER;
                }
                break;
        }
    }
    return (EXIT_FAILURE);
}