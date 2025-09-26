#ifdef __cplusplus
extern "C" {
#endif
//------------------------------------------------------------------------------
#ifndef _AS5047_H_
#define _AS5047_H_
//------------------------------------------------------------------------------
#include <stdint.h>

#define MAX_ENC_POS 0x4000  // Actually 2^14 - 1
    
typedef uint8_t PositionCB(uint16_t pos, uint32_t err);
    
void AS_Init();
uint32_t AS_Diagnostic();
char* AS_DiagnosticText();
uint16_t AS_ReadPosition();
void AS_ReadPositionCB(PositionCB CB);
void AS_StopPositionCB(PositionCB CB);
uint16_t AS_ReadPositionAVG();
    
//-----------------------------------------------------------------------------
#endif /* _AS5047_H_ */
//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif /* _cplusplus*/