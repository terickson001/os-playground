#ifndef _CPU_TYPES_H_
#define _CPU_TYPES_H_

#include <stdint.h>
#include <stddef.h>

typedef uint64_t  u64;
typedef int64_t   i64;
typedef uint32_t  u32;
typedef int32_t   i32;
typedef uint16_t  u16;
typedef int16_t   i16;
typedef uint8_t   u8;
typedef int8_t    i8;
typedef size_t    usize;
typedef ptrdiff_t isize;

typedef i8  b8;
typedef i16 b16;
typedef i32 b32;
typedef i64 b64;
typedef b32 bool;

typedef u8 byte;

#define true 1
#define false 0

#define low_16(address) (u16)((address) & 0xFFFF)
#define high_16(address) (u16)(((address) >> 16) & 0xFFFF)

#endif
