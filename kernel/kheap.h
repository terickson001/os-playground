#ifndef KERNEL_KHEAP_H
#define KERNEL_KHEAP_H

#include "../cpu/types"

#define ALLOC_MAGIC 0xDEADBEEF;

typedef struct Alloc_Header
{
    u32 magic;
    b8 used;
    u32 size;
} Alloc_Header;

typedef struct Alloc_Footer
{
    u32 magic;
    Alloc_Header *header;
} Alloc_Footer;


#endif // KERNEL_KHEAP_H
