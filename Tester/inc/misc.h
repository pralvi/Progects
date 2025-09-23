#ifndef _MISC_H_
#define _MISC_H_

#ifdef __IAR_SYSTEMS_ICC__
    #define INLINE    _Pragma("inline=forced")
    #define NOINLINE  _Pragma("inline=never")
    #define OS_MAIN   __task
    #define __WEAK    __weak
#else
    #define __WEAK    __attribute__((weak))
#endif

#ifndef assert_param
    #define assert_param(param)
#endif

#define ABS(a)   ((a) >= 0  ? (a) : (-(a)))
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MEDIAN(a, b, c) MAX(MAX(MIN(a, b), MIN(a, c)), MIN(b, c))

#define ITEM_COUNT(A) (sizeof(A)/sizeof(A[0]))

#define _CONCAT(A, B) (A ## B)
#define CONCAT(A, B) _CONCAT(A, B)

/* modify bitfield */
#define _BMD(reg, msk, val)     (reg) = (((reg) & ~(msk)) | (val))
/* set bitfield */
#define _BST(reg, bits)         (reg) = ((reg) | (bits))
/* clear bitfield */
#define _BCL(reg, bits)         (reg) = ((reg) & ~(bits))
/* wait until bitfield set */
#define _WBS(reg, bits)         while(((reg) & (bits)) == 0)
/* wait until bitfield clear */
#define _WBC(reg, bits)         while(((reg) & (bits)) != 0)
/* wait for bitfield value */
#define _WVL(reg, msk, val)     while(((reg) & (msk)) != (val))
/* bit value */
#define _BV(bit)                (0x01 << (bit))

#endif // _MISC_H_
