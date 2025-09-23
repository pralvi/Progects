

#ifdef __cplusplus
extern "C" {
#endif
//------------------------------------------------------------------------------
#ifndef _INIT_H_
#define _INIT_H_
//------------------------------------------------------------------------------

#include "stm32.h"
#include "misc.h"

#include "gpio.h"
#include "usart.h"
#include "ring_buf.h"
#include "dispatcher.h"
#include "commands.h"

#include "565RU6.h"

#define USART3_BASE                 (APB1PERIPH_BASE + 0x00004800U)
#define USART3                      ((USART_TypeDef *)USART3_BASE)
#define USART3_IRQn                 39

#define RCC_APB1ENR_USART3EN_Pos             (18U)
#define RCC_APB1ENR_USART3EN_Msk             (0x1U << RCC_APB1ENR_USART3EN_Pos)
#define RCC_APB1ENR_USART3EN                 RCC_APB1ENR_USART3EN_Msk          /*!< USART 3 clock enable */


#define ABS(a) ((a) >= 0  ? (a) : (-(a)))

//General definitions
#define F_CPU          36000000UL	        // SYSCLK Frequency, Hz (internal generator)
#define SysTimerTick   (F_CPU / 1000) - 1	// Sys Timer Tick is 1000 Hz (1mS)

#define CMD_MAX_SIZE 16

#define MODE_NONE 0
#define MODE_556RU6 1

//DATA
#define DATA0      A, 0
#define DATA1      A, 1
#define DATA2      A, 2
#define DATA3      A, 3
#define DATA4      A, 4
#define DATA5      A, 5
#define DATA6      A, 6
#define DATA7      A, 7


//ADR
#define ADR0      B, 0
#define ADR1      B, 1
#define ADR2      B, 2
#define ADR3      B, 3
#define ADR4      B, 4
#define ADR5      B, 5
#define ADR6      B, 6
#define ADR7      B, 7
#define ADR8      B, 8
#define ADR9      B, 9
#define ADR10     B, 10

//CONTROL
#define CTL0      B, 15
#define CTL1      B, 13
#define CTL2      B, 14
#define CTL3      B, 12
#define CTL4      A, 8

//USART Pins
#define RX_PIN          A, 10   //USART1 RX
#define TX_PIN          A, 9    //USART1 TX

//USB
#define USBD1_PIN          A, 11
#define USBD2_PIN          A, 12

extern void InitHardware();
uint8_t tester_mode;
extern volatile uint32_t tick;
static inline uint32_t GetSysTick() { return tick; }
uint64_t GetMicroseconds();
void Delay_us(uint32_t Delay);

#define USART_TX_BUF_SIZE 128
RBUF_DECLARE(usart_tx_buffer, USART_TX_BUF_SIZE);
extern struct usart_tx_buffer usart_tx_buf;

#define USART_TX(value)       RBUF_WRITESTR(usart_tx_buf, (value))
#define USART_TX_CHAR(value)  RBUF_WRITE(usart_tx_buf, (value))
#define USART_TX_IS_EMPTY()   RBUF_IS_EMPTY(usart_tx_buf)
#define USART_TX_COUNT()      RBUF_COUNT(usart_tx_buf)


//-----------------------------------------------------------------------------
#endif /* _INIT_H_ */
//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif /* _cplusplus*/
