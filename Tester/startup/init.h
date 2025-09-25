

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
#define F_CPU          72000000UL	        // SYSCLK Frequency, Hz (internal generator)
#define SysTimerTick   (F_CPU / 1000) - 1	// Sys Timer Tick is 1000 Hz (1mS)

#define CMD_MAX_SIZE 16

#define MODE_NONE 0
#define MODE_556RU6 1

//DATA IN/OUT
#define DATA0      B, 2
#define DATA1      B, 3
#define DATA2      B, 4
#define DATA3      B, 6
#define DATA4      B, 7
#define DATA5      B, 8
#define DATA6      B, 9
#define DATA7      B, 10


//ADR AP6 OUT
#define ADR0      A, 0
#define ADR1      A, 1
#define ADR2      A, 2
#define ADR3      A, 3
#define ADR4      A, 4
#define ADR5      A, 5
#define ADR6      A, 6
#define ADR7      A, 7

//ADR CTL AP6 OUT
#define ADR8      A, 8
#define ADR9      B, 11
#define ADR10     B, 12
//CONTROL OUT
#define CTL0      B, 0
#define CTL1      B, 1
#define CTL2      B, 5
#define CTL3      B, 13
#define CTL4      B, 14

//USART Pins
#define RX_PIN          A, 10   //USART1 RX
#define TX_PIN          A, 9    //USART1 TX

//USB
#define USBD1_PIN          A, 11
#define USBD2_PIN          A, 12

#define LED_PIN          C, 13

extern void InitHardware();
uint8_t tester_mode;
extern volatile uint32_t tick;
static inline uint32_t GetSysTick() { return tick; }
uint64_t GetMicroseconds();
void Delay_us(uint16_t Delay);

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
