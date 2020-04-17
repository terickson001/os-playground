#ifndef _KERNEL_MEM_H_
#define _KERNEL_MEM_H_

#include "../cpu/types.h"

// Paging Types

typedef union Page_Table_Entry
{
    struct
    {
        u32 present       : 1;   // Page present in memory
        u32 rw            : 1;   // Read-only if clear, readwrite if set
        u32 user          : 1;   // Supervisor level only if clear
        u32 write_through : 1;
        u32 cache_disable : 1;
        u32 accessed      : 1;   // Has the page been accessed since last refresh?
        u32 dirty         : 1;   // Has the page been written to since last refresh?
        u32 memory_type   : 1;
        u32 global        : 1;
        u32 unused        : 3;   // Amalgamation of unused and reserved bits
        u32 frame         : 20;  // Frame address (shifted right 12 bits)
    };
    u32 raw;
} Page_Table_Entry;
typedef Page_Table_Entry Page;

typedef union Page_Directory_Entry
{
    struct
    {
        u32 present       : 1;   // Page present in memory
        u32 rw            : 1;   // Read-only if clear, readwrite if set
        u32 user          : 1;   // Supervisor level only if clear
        u32 write_through : 1;
        u32 cache_disable : 1;
        u32 accessed      : 1;   // Has the page been accessed since last refresh?
        u32 _pad          : 1;
        u32 size          : 1;   // Has the page been written to since last refresh?
        u32 ignored       : 1;   // Amalgamation of unused and reserved bits
        u32 unused        : 3;
        u32 table         : 20;  // Table address (shifted right 12 bits)
    };
    u32 raw;
} Page_Directory_Entry;

void memory_copy(void *dest, void *src, int n);
void memory_set(void *dest, u8 val, int n);
u32 kmalloc_int(u32 sz, int align, u32 *phys_addr);
u32 kmalloc(u32 sz);

typedef Page_Directory_Entry Page_Directory[1024] __attribute__((aligned(4096)));
typedef Page_Table_Entry     Page_Table    [1024] __attribute__((aligned(4096)));

typedef struct Page_Stack
{
    struct Page_Stack *next;
    Page *page;
} Page_Stack;

static Page_Stack *free_pages;
#endif
