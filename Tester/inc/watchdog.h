#ifdef __cplusplus
extern "C" {
#endif
//------------------------------------------------------------------------------
#ifndef _WATCHDOG_H_
#define _WATCHDOG_H_
//------------------------------------------------------------------------------

#include <stdint.h>    
#include <stm32.h>    

    
void IWDG_InitEx(uint32_t us);
void IWDG_Init(uint32_t psc, uint32_t rlr);
void IWDG_Reset();
    
//-----------------------------------------------------------------------------
#endif /* _WATCHDOG_H_ */
//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif /* _cplusplus*/