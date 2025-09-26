#ifdef __cplusplus
extern "C" {
#endif
//------------------------------------------------------------------------------
#ifndef _INIT_H_
#define _INIT_H_
//------------------------------------------------------------------------------

#include "stm32.h"
//#include "intrinsics.h"
#include "misc.h"
#include "gpio.h"
#include "pid.h"

#define TIM4_BASE                   (APB1PERIPH_BASE + 0x00000800U)
#define TIM4                        ((TIM_TypeDef *)TIM4_BASE)
#define TIM4_IRQn                   30

#define RCC_APB1ENR_TIM4EN_Pos      (2U)
#define RCC_APB1ENR_TIM4EN_Msk      (0x1U << RCC_APB1ENR_TIM4EN_Pos)  /*!< 0x00000004 */
#define RCC_APB1ENR_TIM4EN          RCC_APB1ENR_TIM4EN_Msk            /*!< Timer 4 clock enable */

#define DBGMCU_CR_DBG_TIM4_STOP_Pos (13U)
#define DBGMCU_CR_DBG_TIM4_STOP_Msk (0x1U << DBGMCU_CR_DBG_TIM4_STOP_Pos) /*!< 0x00002000 */
#define DBGMCU_CR_DBG_TIM4_STOP     DBGMCU_CR_DBG_TIM4_STOP_Msk        /*!< TIM4 counter stopped when core is halted */

#define ABS(a) ((a) >= 0  ? (a) : (-(a)))

//General definitions
#define F_CPU          72000000UL	        // SYSCLK Frequency, Hz
#define SysTimerTick   (F_CPU / 1000) - 1	// Sys Timer Tick is 1000 Hz (1mS)

//GPIO PINS

//  {PA1, PA0} - mode
//   0    0 - Manual
//   0    1 - Center
//   1    0 - Auto
//   1    1 - Manual

//PA3 - ADC
//PB9 - Output Servo Control

//Led Pin
#define LED_PIN         C, 13
#define LED_ON          SET_PIN(LED_PIN, 0);
#define LED_OFF         SET_PIN(LED_PIN, 1);
#define LED_TOGGLE      SET_PIN(LED_PIN, !GET_PIN(LED_PIN));

extern volatile uint32_t tick;

//-----------------------------------------------------------------------------
#endif /* _INIT_H_ */
//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif /* _cplusplus*/
