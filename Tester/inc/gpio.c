#include "gpio.h"

void SetPin(GPIO_TypeDef *port, uint_fast8_t pin, uint_fast8_t value) {
  uint32_t bit = 1UL << pin;
  if (value == 0)
    bit <<= 16;

  port->BSRR = bit;
}

uint_fast8_t GetPin(GPIO_TypeDef *port, uint_fast8_t pin) {
  return (port->IDR & (1UL << pin)) != 0;
}

void TogglePin(GPIO_TypeDef *port, uint_fast8_t pin) {
    SetPin(port, pin, !GetPin(port, pin));
}

uint32_t SerialIn(GPIO_TypeDef *portSCK, uint_fast8_t pinSCK, GPIO_TypeDef *portDT, uint_fast8_t pinDT, uint_fast8_t count) {
  uint32_t bit = 1UL << pinSCK;
  uint32_t currentValue = portSCK->IDR & bit, toggleValue;
  if (currentValue != 0)
    toggleValue = bit << 16;
  else {
    currentValue = bit << 16;
    toggleValue = bit;
  }

  volatile uint32_t *BSRR = &portSCK->BSRR;
  volatile uint32_t *IDR = &portDT->IDR;
  bit = 1UL << pinDT;
  uint32_t result = 0;
  while(count--) {
    *BSRR = toggleValue;

    result <<= 1;
    if ((*IDR & bit) != 0)
      result++;

    *BSRR = currentValue;
  }

  return result;
}
