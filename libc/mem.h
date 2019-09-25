#ifndef _KERNEL_MEM_H_
#define _KERNEL_MEM_H_

#include "../cpu/types.h"

void memory_copy(u8 *dest, u8 *src, int n);
void memory_set(u8 *dest, u8 val, int n);

#endif
