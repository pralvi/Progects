#ifdef __cplusplus
extern "C" {
#endif
//------------------------------------------------------------------------------
#ifndef _SPI_H_
#define _SPI_H_
//------------------------------------------------------------------------------
#include <stdint.h>
#include "misc.h"

void    InitSPI(uint8_t useDMA);

void    SPI_Send(uint8_t data);
uint8_t SPI_Receive();
uint8_t SPI_SendReceive(uint8_t data);

void    SPI_SendDMA(void* tx_data, uint32_t size);
void    SPI_ReceiveDMA(void* rx_buf, uint32_t size);
void    SPI_SendReceiveDMA(void* tx_data, void* rx_buf, uint32_t size);

__WEAK void SPI_DMA_RXC(void); //Receive over DMA completed
__WEAK void SPI_DMA_TXC(void); //Transmit over DMA completed

uint8_t SPI_IsDMAEnabled();

//void    SPI_Test();

//-----------------------------------------------------------------------------
#endif /* _SPI_H_ */
//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif /* _cplusplus*/
