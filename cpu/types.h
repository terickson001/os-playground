#ifndef _CPU_TYPES_H_
#define _CPU_TYPES_H_

#include <stdint.h>
#include <stddef.h>

typedef uint32_t  u32;
typedef int32_t   i32;
typedef uint16_t  u16;
typedef int16_t   i16;
typedef uint8_t   u8;
typedef int8_t    i8;
typedef size_t    usize;
typedef ptrdiff_t isize;

#define low_16(address) (u16)((address) & 0xFFFF)
#define high_16(address) (u16)(((address) >> 16) & 0xFFFF)

#endif
