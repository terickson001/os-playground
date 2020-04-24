#ifndef KERNEL_KHEAP_H
#define KERNEL_KHEAP_H

#include "../cpu/types.h"
#include "ordered_array.h"

#define KHEAP_START        0xC0000000
#define KHEAP_INITIAL_SIZE 0x100000
#define HEAP_INDEX_SIZE    0x20000
#define HEAP_MIN_SIZE      0x70000
#define HEAP_MAGIC         0xDEADBEEF

typedef struct Alloc_Header
{
    u32 magic;
    b32 used;
    u32 size;
} Alloc_Header;

typedef struct Alloc_Footer
{
    u32 magic;
    Alloc_Header *header;
} Alloc_Footer;

typedef struct Heap
{
    Ordered_Array index;
    u32 start;
    u32 end;
    u32 max;
    b32 supervisor;
    b32 readonly;
} Heap;

Heap *make_heap(u32 start, u32 end, u32 max, b32 supervisor, b32 readonly);
void *alloc(u32 size, b32 page_align, Heap *heap);
void free(void *p, Heap *heap);

#endif // KERNEL_KHEAP_H
