#ifdef __cplusplus
extern "C" {
#endif
//------------------------------------------------------------------------------
#ifndef _565RU6_H_
#define _565RU6_H_
//------------------------------------------------------------------------------
#include <stdint.h>

#define DO DATA0 // 14pin
#define DI DATA1 // 2pin

#define RAS CTL2 // 4pin
#define CAS CTL0 // 15pin
#define WR  CTL1 // 3pin
//ADR0 5pin
//ADR1 7pin
//ADR2 6pin
//ADR3 12pin
//ADR4 11pin
//ADR5 10pin
//ADR6 13pin

#define RU6_NONE  0
#define RU6_READ  1
#define RU6_WRITE 2
#define RU6_SEND  3

#define RU6_ADR_MAX 0x3FFF



void start_556RU6(void);
uint8_t RU6_mode;
uint8_t RU6_write_mode;
uint8_t cycle_test;

//565RU5
//            8 - +5V
//           16 - GND
//      -----------------
//  2 -| DI  | RAM | DO  |- 14
//     |-----|     |     |
//  5 -| A0  |     |     |
//  7 -| A1  |     |     |
//  6 -| A2  |     |     |
// 12 -| A3  |     |     |
// 11 -| A4  |     |     |
// 10 -| A5  |     |     |
// 13 -| A6  |     |     |
//  9 -| A7  |     |     |
//     |-----|     |     |
//  4 -o RAS |     |     |
// 15 -o CAS |     |     |
//  3 -o WE  |     |     |
//      -----------------

//565RU6
//            8 - +5V
//           16 - GND
//      -----------------
//  2 -| DI  | RAM | DO  |- 14
//     |-----|     |     |
//  5 -| A0  |     |     |
//  7 -| A1  |     |     |
//  6 -| A2  |     |     |
// 12 -| A3  |     |     |
// 11 -| A4  |     |     |
// 10 -| A5  |     |     |
// 13 -| A6  |     |     |
//  9 -| A7  |     |     |
//     |-----|     |     |
//  4 -o RAS |     |     |
// 15 -o CAS |     |     |
//  3 -o WE  |     |     |
//      -----------------


//------------------------------------------------------------------------------
#endif /* _565RU6_H_ */
//------------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif /* _cplusplus*/
