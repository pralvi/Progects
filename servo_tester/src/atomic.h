#ifndef _ATOMIC_H_
#define _ATOMIC_H_

#include "stm32f10x.h"

uint_fast8_t CompareExchange8(uint8_t * ptr, uint8_t oldValue, uint8_t newValue);
uint_fast8_t CompareExchange16(uint16_t * ptr, uint16_t oldValue, uint16_t newValue);
uint_fast8_t CompareExchange32(uint32_t * ptr, uint32_t oldValue, uint32_t newValue);

uint32_t AtomicFetchAndAdd32(uint32_t * ptr, uint32_t value);
uint32_t AtomicAddAndFetch32(uint32_t * ptr, uint32_t value);

#endif /* _ATOMIC_H_ */
