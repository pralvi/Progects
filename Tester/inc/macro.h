#ifdef __cplusplus
extern "C" {
#endif
//------------------------------------------------------------------------------
#ifndef _MACRO_H_
#define _MACRO_H_
//------------------------------------------------------------------------------

#define INLINE    _Pragma("inline=forced")
#define NOINLINE  _Pragma("inline=never")
#define OS_MAIN   __task

#define ABS(a)   ((a) >= 0  ? (a) : (-(a)))
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MEDIAN(a, b, c) MAX(MAX(MIN(a, b), MIN(a, c)), MIN(b, c))

//------------------------------------------------------------------------------
#endif /* _MACRO_H_ */
//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif /* _cplusplus*/
