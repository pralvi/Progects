

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
#include "556RTx.h"


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

#define MODE_565RU6 1

#define MODE_556RT4 2
#define MODE_556RT5 3
#define MODE_556RT17 4
#define MODE_556RT18 5

#define ver 590kn6

//DATA IN/OUT
#define DATA0      B, 11
#define DATA1      B, 10
#define DATA2      B, 9
#define DATA3      B, 8
#define DATA4      B, 7
#define DATA5      B, 6
#define DATA6      B, 4
#define DATA7      B, 3


//ADR AP6 OUT          //AP6: 1,20 to 5V  10,18 to GND
#define ADR0      A, 0 //DD2 AP6 9in 11out
#define ADR1      A, 1 //DD2 AP6 8in 12out
#define ADR2      A, 2 //DD2 AP6 7in 13out
#define ADR3      A, 3 //DD2 AP6 6in 14out
#define ADR4      A, 4 //DD2 AP6 5in 15out
#define ADR5      A, 5 //DD2 AP6 4in 16out
#define ADR6      A, 6 //DD2 AP6 3in 17out
#define ADR7      A, 7 //DD2 AP6 2in 18out

//ADR CTL AP6 OUT
#define ADR8      B, 0 //DD1 AP6 9in 11out
#define ADR9      B, 1 //DD1 AP6 8in 12out
#define ADR10     B, 5 //DD1 AP6 7in 13out
//CONTROL OUT
#define CTL0      B, 12 //DD1 AP6 2in 18out
#define CTL1      B, 13 //DD1 AP6 3in 17out
#define CTL2      B, 14 //DD1 AP6 4in 16out
#define CTL3      B, 15 //DD1 AP6 5in 15out
#define CTL4      A, 8  //DD1 AP6 6in 14out

//USART Pins
#define RX_PIN          A, 10   //USART1 RX
#define TX_PIN          A, 9    //USART1 TX

//USB
#define USBD1_PIN          A, 11
#define USBD2_PIN          A, 12

#define LED_PIN          C, 13

extern void InitHardware();
uint8_t tester_mode;
uint8_t device_buffer[2048];
extern volatile uint32_t tick;
static inline uint32_t GetSysTick() { return tick; }
uint64_t GetMicroseconds();
void Delay_us(uint16_t Delay);
void Clear_Buffer(void);
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
