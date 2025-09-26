#include "atomic.h"

uint_fast8_t CompareExchange8(uint8_t * ptr, uint8_t oldValue, uint8_t newValue) {
    if(__LDREXB(ptr) == oldValue)
        return __STREXB(newValue, ptr) == 0;
    __CLREX();
    return 0;
}

uint_fast8_t CompareExchange16(uint16_t * ptr, uint16_t oldValue, uint16_t newValue) {
    if(__LDREXH(ptr) == oldValue)
        return __STREXH(newValue, ptr) == 0;
    __CLREX();
    return 0;
}

uint_fast8_t CompareExchange32(uint32_t * ptr, uint32_t oldValue, uint32_t newValue) {
    if(__LDREXW(ptr) == oldValue)
        return __STREXW(newValue, ptr) == 0;
    __CLREX();
    return 0;
}

uint32_t AtomicFetchAndAdd(uint32_t * ptr, uint32_t value) {
    uint32_t oldValue, newValue;
    do {
        oldValue = __LDREXW(ptr);
        newValue = oldValue + value;
    } while(__STREXW(newValue, ptr));
    return oldValue;
}

uint32_t AtomicAddAndFetch(uint32_t * ptr, uint32_t value) {
    uint32_t oldValue, newValue;
    do {
        oldValue = __LDREXW(ptr);
        newValue = oldValue + value;
    } while(__STREXW(newValue, ptr));
    return newValue;
}

//// переменная, которую нужно атомарно модифицировать
//volatile uint32_t value;
//...
//uint32_t oldValue;
//uint32_t newValue;
//do
//{
//// запоминаем старое значение
//        oldValue = value;
//// производим длинное вычисление
//        newValue = SomeLongOperation(oldValue);
//// пытаемся атомарно заменить исходное значение
//// если его кто-то изменил без нас - перечитываем и вычисляем снова
//}while(!CompareExchange((uint32_t *)&value, oldValue, newValue));
