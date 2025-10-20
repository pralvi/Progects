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





#define RT4BUFSIZE 256
#define RT5BUFSIZE 512
#define RT17BUFSIZE 512
#define RT18BUFSIZE 2048
// RT4 254x4
// RT17 512x8
// RT18 2048x8

void read_RTx(void);
void init_556RTx(void);
//------------------------------------------------------------------------------
#endif /* _556RTX_H_ */
//------------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif /* _cplusplus*/
