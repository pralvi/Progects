#ifdef __cplusplus
extern "C" {
#endif
//------------------------------------------------------------------------------
#ifndef _565RU6_H_
#define _565RU6_H_
//------------------------------------------------------------------------------

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


void start_556RU6(void);

//------------------------------------------------------------------------------
#endif /* _565RU6_H_ */
//------------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif /* _cplusplus*/
