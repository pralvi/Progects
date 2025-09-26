#ifdef __cplusplus
extern "C" {
#endif
//------------------------------------------------------------------------------
#ifndef _RING_BUF_H_
#define _RING_BUF_H_
//------------------------------------------------------------------------------

#include "misc.h"

//Fast ring buffer
#define STATIC_ASSERT(expr) typedef char CONCAT(static_assert_failed_at_line_, __LINE__) [(expr) ? 1 : -1]

#define RBUF_TYPE char

//declares fast ring buffer
//buf_size must be a power of 2
#define RBUF_DECLARE(buf_name, buf_size) \
struct buf_name { \
   volatile RBUF_TYPE wCnt; \
   volatile RBUF_TYPE rCnt; \
   RBUF_TYPE buf[buf_size]; \
}; \
STATIC_ASSERT((buf_size&(buf_size-1))==0 && buf_size > 1) \

#define RBUF_IS_EMPTY(rbuf) (rbuf.wCnt == rbuf.rCnt)
#define RBUF_IS_FULL(rbuf) (((RBUF_TYPE)(rbuf.wCnt - rbuf.rCnt) & ~(RBUF_TYPE)(sizeof(rbuf.buf)-1)) != 0)

#define RBUF_COUNT(rbuf) ((RBUF_TYPE)(rbuf.wCnt - rbuf.rCnt) & (RBUF_TYPE)(sizeof(rbuf.buf)-1))
#define RBUF_SIZE(rbuf) (RBUF_TYPE)sizeof(rbuf.buf)

#define RBUF_WRITE(rbuf, value) do { rbuf.buf[rbuf.wCnt & (RBUF_TYPE)(sizeof(rbuf.buf)-1)] = (value); rbuf.wCnt++; } while(0)
#define RBUF_READ(rbuf, dst) do { dst = rbuf.buf[rbuf.rCnt & (RBUF_TYPE)(sizeof(rbuf.buf)-1)]; rbuf.rCnt++; } while(0)

#define RBUF_WRITESTR(rbuf, value) {char* __msg = (value); while(*__msg) RBUF_WRITE(rbuf, *__msg++); }

#define RBUF_CLEAR(rbuf) {rbuf.wCnt = 0; rbuf.rCnt = 0;}

//Arbitrary size ring buffer
//declares ring buffer buf_type with elements type buf_type and count buf_count
#define RING_BUF_DECLARE( buf_name, buf_type, buf_count) \
struct { \
        buf_type *rd; \
        buf_type *wr; \
        buf_type buf[buf_count]; \
} buf_name

//initializes read and write pointers
#define RING_BUF_CLEAR( buf_ptr )  { (buf_ptr)->rd = (buf_ptr)->wr = RING_BUF_FIRST(buf_ptr); }

//returns pointer to first element
#define RING_BUF_FIRST( buf_ptr ) ( &(buf_ptr)->buf[0] )
//returns pointer to last element
#define RING_BUF_LAST( buf_ptr ) ( &(buf_ptr)->buf[ RING_BUF_COUNT(buf_ptr) - 1 ] )

//returns buffer suze
#define RING_BUF_SIZE( buf_ptr )  ( sizeof((buf_ptr)->buf) )
//returns elements count
#define RING_BUF_COUNT( buf_ptr ) ( sizeof((buf_ptr)->buf) / sizeof((buf_ptr)->buf[0]) )

//checks if buffer is empty
#define RING_BUF_EMPTY( buf_ptr ) ( (buf_ptr)->rd == (buf_ptr)->wr )

//increment read pointer
#define RING_BUF_INC_RD(buf_ptr) { ((buf_ptr)->rd)++; if ((buf_ptr)->rd > RING_BUF_LAST(buf_ptr)) (buf_ptr)->rd = RING_BUF_FIRST(buf_ptr); }
//read without increment
#define RING_BUF_GET(buf_ptr) (*((buf_ptr)->rd))
//read with increment
#define RING_BUF_RD(buf_ptr) RING_BUF_GET(buf_ptr); RING_BUF_INC_RD(buf_ptr);

//increment write pointer
#define RING_BUF_INC_WR(buf_ptr) { ((buf_ptr)->wr)++; if ((buf_ptr)->wr > RING_BUF_END(buf_ptr)) (buf_ptr)->wr = RING_BUF_START(buf_ptr); }
//write last element without increment
#define RING_BUF_PUT(buf_ptr, data) *((buf_ptr)->wr) = data;
//write to buffer with increment
#define RING_BUF_WR( buf_ptr, data ) { RING_BUF_PUT(buf_ptr, data); RING_BUF_INC_WR(buf_ptr); }

//write string str to buffer, write0==TRUE to write last '0'
#define RING_BUF_WR_STR( buf_ptr, str, write0 ) { \
        for (U8 *s1=(U8 *)str; *s1!=0; s1++) RING_BUF_WR( buf_ptr, *s1 ); \
        if (write0) RING_BUF_WR( buf_ptr, 0 ); \
}


//-----------------------------------------------------------------------------
#endif /* _RING_BUF_H_ */
//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif /* _cplusplus*/
