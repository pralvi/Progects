#ifndef __USART_H_
#define __USART_H_

#include <stdint.h>
#include "init.h"

#define USART_RX_EVENT 1
#define USART_TX_EVENT 2

typedef uint8_t (*USART_HANDLER)(USART_TypeDef* usart, uint8_t event, uint8_t* data);

//ASSUMPTIONS: APB1 clk = F_CLK/2, APB2 clk = F_CLK
uint8_t init_usart(USART_TypeDef* usart, uint32_t baud, USART_HANDLER handler);

void usart_transmit(USART_TypeDef* usart);

#endif /* __USART_H_ */
