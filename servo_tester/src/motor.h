#ifdef __cplusplus
extern "C" {
#endif
//------------------------------------------------------------------------------
#ifndef _MOTOR_H_
#define _MOTOR_H_
//------------------------------------------------------------------------------

#include <stdint.h>
#include "misc.h"

void InitPWM();

uint8_t SetPWMDuty(uint32_t duty);
uint8_t SetDirection(uint8_t dir);
uint8_t SetEnabled(uint8_t en);
void    GetStatus(void CB(char));
uint8_t IsForward();
uint8_t IsMoving();

__WEAK uint_fast16_t ValidateDuty(uint32_t duty);

//-----------------------------------------------------------------------------
#endif /* _MOTOR_H_ */
//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif /* _cplusplus*/
