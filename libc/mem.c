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

/* typedef struct Alloc_Header */
/* { */
/*     u8 free : 1; */
/*     u8  */
/* } Alloc_Header; */

/* u32 kmalloc(usize size) */
/* { */
    
/* } */

extern void load_page_directory(u32 *page_directory);
extern void enable_paging();

u32 page_directory[1024] __attribute__((aligned(4096)));
void init_paging()
{
    // Supervisor:    Only kernel-mode can access
    // Write Enabled: It can be both read from and written to
    // Not Present:   The page table is not present
    for (int i = 0; i < 1024; i++)
        page_directory[i] = 0x00000002;

    u32 first_page_table[1024] __attribute__((aligned(4096)));
    for (u32 i; i < 1024; i++)
        first_page_table [i] = (i * 0x1000) | 3; // attributes: supervisor level, read/write, present
    page_directory[0] = ((u32)first_page_table) | 3;


    load_page_directory(page_directory);
    enable_paging();
}

static Page_Frame kalloc_frame_int()
{
    u32 i = 0;
    while (!(page_directory[i] & 0x1))
    {
        i++;
        if (i == npages)
        {
            return {Page_Frame}{0};
        }
    }
    
}
