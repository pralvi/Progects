#ifdef __cplusplus
extern "C" {
#endif
//------------------------------------------------------------------------------
#ifndef _CDC_H_
#define _CDC_H_
//------------------------------------------------------------------------------

#include "usb.h"
#include "usb_cdc.h"

//#define CDC_DATA_SZ     0x08

#ifndef CDC_RX_DATA_SZ
#define CDC_RX_DATA_SZ  8
#endif

#ifndef CDC_TX_DATA_SZ
#define CDC_TX_DATA_SZ  32
#endif

#ifndef USB_LANGID
#define USB_LANGID USB_ARRAY_DESC(USB_LANGID_ENG_US)
#endif

#ifndef USB_MANUF_DESC
#define USB_MANUF_DESC USB_STRING_DESC("Breeze Electronics")
#endif

#ifndef USB_PRODUCT_DESC
#define USB_PRODUCT_DESC USB_STRING_DESC("STM32 Virtual COM Port")
#endif

void RestartCDC();
usbd_device* InitCDC();
uint8_t USBIsAvailable();
int32_t cdc_read_size();
int32_t cdc_read(void *buf, uint16_t blen);
int32_t cdc_write(void *buf, uint16_t blen);

__WEAK void cdc_rx_callback();
__WEAK void cdc_tx_callback();


//-----------------------------------------------------------------------------
#endif /* _CDC_H_ */
//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif /* _cplusplus*/
