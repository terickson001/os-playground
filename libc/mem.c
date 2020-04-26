#include <libc/mem.h>
#include <kernel/kernel.h>

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