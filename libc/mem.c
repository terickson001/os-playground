#include "mem.h"

void memory_copy(u8 *dest, u8 *src, int n)
{
    for (int i = 0; i < n; i++)
        *(dest++) = *(src++);
}

vod memory_set(u8 *dest, u8 val, int n)
{
    for (int i = 0; i < n; i++)
        *(dest++) = val;
}
