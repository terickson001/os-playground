#ifndef _KERNEL_MEM_H_
#define _KERNEL_MEM_H_

#include "../cpu/types.h"

void memory_copy(void *dest, void *src, int n);
void memory_set(void *dest, u8 val, int n);
u32 kmalloc(u32 size, int align, u32 *phys_addr);

#endif
