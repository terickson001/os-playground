#ifndef KERNEL_PAGING_H
#define KERNEL_PAGING_H

#include <cpu/types.h>
#include <cpu/isr.h>

// Paging Types
typedef u32 Page_Frame;
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

typedef struct Page_Table
{
    Page pages[1024];
} Page_Table;

typedef struct Page_Directory
{
    Page_Table *tables[1024];
    Page_Directory_Entry entries[1024];
} Page_Directory;

u32 kmalloc_a(u32 sz);
u32 kmalloc_p(u32 sz, u32 *phys);
u32 kmalloc_ap(u32 sz, u32 *phys);
u32 kmalloc(u32 sz);

void kfree(u32 p);

void init_paging();
Page *get_page(u32 address, b32 make, Page_Directory *dir);
void alloc_frame(Page *page, b32 is_kernel, b32 is_writeable);
void free_frame(Page *page);
void page_fault(Registers *regs);

extern Page_Directory *kernel_directory;
extern Page_Directory *current_directory;
#endif // KERNEL_PAGING_H