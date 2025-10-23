#ifdef __cplusplus
extern "C" {
#endif
//------------------------------------------------------------------------------
#ifndef _556RTX_H_
#define _556RTX_H_
//------------------------------------------------------------------------------
#include <stdint.h>
#include "firmwares.h"



#define CS1         CTL0 //
#define CS2         CTL1 //
#define CS3         CTL2 //
#define CS4         ADR10 //
#define ON12V       CTL3 //
#define CS1_RT4     DATA4 //

// DATA0 - DATA7 - output to LN3
// DIN0 - DIN7 - input from diodes

#define RT4BUFSIZE 256
#define RT5BUFSIZE 512
#define RT17BUFSIZE 512
#define RT18BUFSIZE 2048
// RT4 254x4
// RT17 512x8
// RT18 2048x8

void read_RTx(void);
void init_556RTx(void);
void program_byte_RTx(uint16_t adr, uint8_t data);
void task_program_RTx(uint8_t tag);
uint8_t read_byte_RTx(uint16_t adr);
uint16_t write_adr;
uint8_t write_data;
//------------------------------------------------------------------------------
#endif /* _556RTX_H_ */
//------------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif /* _cplusplus*/
