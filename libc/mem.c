#include "mem.h"

void memory_copy(void *dest, void *src, int n)
{
    for (int i = 0; i < n; i++)
        *((u8 *)dest++) = *((u8 *)src++);
}

void memory_set(void *dest, u8 val, int n)
{
    
    for (int i = 0; i < n; i++)
        *((u8 *)dest++) = val;
}

/* This should be computed at link time, but a hardcoded
 * value is fine for now. Remember that our kernel starts
 * at 0x1000 as defined on the Makefile */
u32 free_mem_addr = 0x10000;
/* Implementation is just a pointer to some free memory which
 * keeps growing */
u32 kmalloc(usize size, int align, u32 *phys_addr) {
    /* Pages are aligned to 4K, or 0x1000 */
    if (align == 1 && (free_mem_addr & 0xFFFFF000)) {
        free_mem_addr &= 0xFFFFF000;
        free_mem_addr += 0x1000;
    }
    /* Save also the physical address */
    if (phys_addr) *phys_addr = free_mem_addr;

    u32 ret = free_mem_addr;
    free_mem_addr += size; /* Remember to increment the pointer */
    return ret;
}
