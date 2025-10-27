#ifdef __cplusplus
extern "C" {
#endif
//------------------------------------------------------------------------------
#ifndef _BRIDGE_H_
#define _BRIDGE_H_
//------------------------------------------------------------------------------

#include "ring_buf.h"

#define CDC_RX_BUF_SIZE 32
#define CDC_TX_BUF_SIZE 256

RBUF_DECLARE(cdc_tx_buf, CDC_TX_BUF_SIZE);
extern struct cdc_tx_buf cdc_tx_buf;

#define CDC_TX(value)       RBUF_WRITESTR(cdc_tx_buf, (value))
#define CDC_TX_CHAR(value)  RBUF_WRITE(cdc_tx_buf, (value))
#define CDC_TX_IS_EMPTY()   RBUF_IS_EMPTY(cdc_tx_buf)
#define CDC_TX_COUNT()      RBUF_COUNT(cdc_tx_buf)

void ProcessCMD(uint8_t *cmd, uint8_t size);

//-----------------------------------------------------------------------------
#endif /* _BRIDGE_H_ */
//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif /* _cplusplus*/
