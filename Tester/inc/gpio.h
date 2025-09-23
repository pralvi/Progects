#ifdef __cplusplus
extern "C" {
#endif
//------------------------------------------------------------------------------
#ifndef _GPIO_H_
#define _GPIO_H_
//------------------------------------------------------------------------------

#include "stm32.h"

//Port directions
#define OUTPUT        OUTPUT
#define ALTERNATE     ALTERNATE
#define INPUT         INPUT

//Output/alterate modes are always a cobination of one the following mode and speed
#define PUSHPULL      0
#define OPENDRAIN     4

#define SLOPE_2MHZ    2
#define SLOPE_10MHZ   1
#define SLOPE_50MHZ   3

//Input modes
#define ANALOG        ANALOG
#define FLOATING      FLOATING
#define PULLUP        PULLUP
#define PULLDOWN      PULLDOWN

#define CFG_PIN_(PORT, PIN, MODE) *((volatile uint32_t*)GPIO ## PORT + 1 * (PIN > 7)) = (*((volatile uint32_t*)GPIO ## PORT + 1 * (PIN > 7)) & ~((unsigned long)(0xF) << (PIN % 8 * 4))) | ((unsigned long)(MODE) << (PIN % 8 * 4))

#define CFG_PIN_OUTPUT(PORT, PIN, DIRECTION, MODE) CFG_PIN_(PORT, PIN, MODE)
#define CFG_PIN_ALTERNATE(PORT, PIN, DIRECTION, MODE) CFG_PIN_(PORT, PIN, 8 | MODE)
#define CFG_PIN_INPUT(PORT, PIN, DIRECTION, MODE) CFG_PIN_INPUT_##MODE(PORT, PIN)

#define CFG_PIN_INPUT_ANALOG(PORT, PIN) CFG_PIN_(PORT, PIN, 0)
#define CFG_PIN_INPUT_FLOATING(PORT, PIN) CFG_PIN_(PORT, PIN, 4)
#define CFG_PIN_INPUT_PULLUP(PORT, PIN) CFG_PIN_(PORT, PIN, 8); GPIO##PORT->BSRR |= (1UL << PIN)
#define CFG_PIN_INPUT_PULLDOWN(PORT, PIN) CFG_PIN_(PORT, PIN, 8); GPIO##PORT->BSRR |= (1UL << PIN) << 16

#define CFG_PIN_DIRECTION(PORT, PIN, DIRECTION, MODE) CFG_PIN_##DIRECTION(PORT, PIN, DIRECTION, MODE)
#define ENABLE_PORT_CLOCK_(PORT, ...) RCC->APB2ENR |= RCC_APB2ENR_IOP##PORT##EN
#define SET_PIN_(PORT, PIN, VAL) (GPIO ## PORT->BSRR = ((VAL) ? (1UL << PIN) : ((1UL << PIN) << 16)))
#define GET_PIN_(PORT, PIN) ((GPIO ## PORT->IDR & (1UL << PIN)) != 0)

#define ENABLE_PORT_CLOCK(...) ENABLE_PORT_CLOCK_(__VA_ARGS__)
#define CFG_PIN(...) CFG_PIN_DIRECTION(__VA_ARGS__)
#define SET_PIN(...) SET_PIN_(__VA_ARGS__)
#define GET_PIN(...) GET_PIN_(__VA_ARGS__)
#define TOGGLE_PIN(...) {if (GET_PIN(__VA_ARGS__)) SET_PIN(__VA_ARGS__, 0); else SET_PIN(__VA_ARGS__, 1); }

#define _GET_PORT(PORT, PIN) (PORT)
#define _GET_PINNO(PORT, PIN) (PIN)
#define _GPIO(PORT, PIN) GPIO ## PORT, PIN

#define GET_PORT(...) _GET_PORT(__VA_ARGS__)
#define GET_PINNO(...) _GET_PINNO(__VA_ARGS__)
#define GPIO(...) _GPIO(__VA_ARGS__)

#define _PERIF(P, N) P ## N

#define GET_PERIF_NAME(P, N) P
#define GET_PERIF_NUMBER(P, N) N
#define PERIF(...) _PERIF(__VA_ARGS__)

void SetPin(GPIO_TypeDef *port, uint_fast8_t pin, uint_fast8_t value);
uint_fast8_t GetPin(GPIO_TypeDef *port, uint_fast8_t pin);
void TogglePin(GPIO_TypeDef *port, uint_fast8_t pin);
uint32_t SerialIn(GPIO_TypeDef *portSCK, uint_fast8_t pinSCK, GPIO_TypeDef *portDT, uint_fast8_t pinDT, uint_fast8_t count);
//-----------------------------------------------------------------------------
#endif /* _GPIO_H_ */
//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif /* _cplusplus*/
