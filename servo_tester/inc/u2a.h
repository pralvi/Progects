#ifdef __cplusplus
extern "C" {
#endif
//------------------------------------------------------------------------------
#ifndef _U2A_H_
#define _U2A_H_
//------------------------------------------------------------------------------
#include <stdint.h>

unsigned char * utoa(uint32_t value, unsigned char *buffer);
unsigned char * itoa(int32_t value, unsigned char *buffer);
unsigned char * utoaLTR(uint32_t value, unsigned char *buffer);
unsigned char * itoaLTR(int32_t value, unsigned char *buffer);
unsigned char utoaLTRCB(uint32_t value, void CB(char));
unsigned char itoaLTRCB(int32_t value, void CB(char));

char * ftoa(float value, char *result);
void ftoaCB(float value, void CB(char));

float parseFloat(char *ptr);

//-----------------------------------------------------------------------------
#endif /* _U2A_H_ */
//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif /* _cplusplus*/
