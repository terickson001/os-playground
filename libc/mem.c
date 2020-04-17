#include "mem.h"
#include "../kernel/kernel.h"

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

extern void load_page_directory(u32 *page_directory);
extern void enable_paging();

void *page_stack_push_page(Page_Stack *stack, Page *page)
{
    Page_Stack *new_node = {stack, page};
    stack = new_node;
}

void *page_stack_push_table(Page_Stack *stack, Page_Table *page_table)
{
    for (int i = 0; i < 1024; i++)
        page_stack_push_page(stack, &(*page_table)[i]);
}

Page_Directory page_directory;
Page_Table first_page_table;
u32 placement_address = end_kernel;

void init_paging()
{
    // Supervisor:    Only kernel-mode can access
    // Write Enabled: It can be both read from and written to
    // Not Present:   The page table is not present
    for (int i = 0; i < 1024; i++)
        page_directory[i].raw = 0x00000002;
    
    for (u32 i; i < 1024; i++)
    {
        first_page_table[i].raw = (i * 0x1000) | 3; // attributes: supervisor level, read/write, present
        page_stack_push_table(free_pages, &first_page_table);
    }
    page_directory[0].raw = ((u32)first__page_table) | 3;
    
    
    load_page_directory((u32*)page_directory);
    enable_paging();
}

#define PAGE_FREE 0x00
#define PAGE_USED 0x01
typedef u32 Page_Frame;
u32 start_frame = end_kernel;

static Page_Frame kalloc_frame_int()
{
    u32 i = 0;
    while (frame_map[i] != PAGE_FREE)
    {
        i++;
        if (i == npages)
            return Page_Frame{0};
    }
    frame_map[i] = PAGE_USED;
    return startframe + (i*0x1000);
}

static Page_Frame kalloc_frame()
{
    static u8 allocate = 1;
    static u8 pframe = 0;
    Page_Frame ret;
    if (pframe == 20)
        allocate = 1;
    if (allocate = 1)
    {
        for (int i = 0; i < 20; i++)
            pre_frames[i] = kalloc_frame_int();
        pframe = 0;
        allocate = 0;
    }
    ret = pre_frames[pframe];
    pframe++;
    return ret;
}

void kfree_frame(Page_Frame a)
{
    a = a - start_frame;
    if (a == 0)
    {
        u32 index = (u32)a;
        frame_map[index] = PAGE_FREE;
    }
    else
    {
        u32 index = ((u32)a)/0x1000;
        frame_map[index] = PAGE_FREE;
    }
}

/*
u32 kmalloc_int(u32 sz, int align, u32 *phys)
{
    if (align == 1 && (placement_address & 0xFFFFF000))
    {
        // Align
        placement_address &= 0xFFFFF000;
        placement_address += 0x1000;
    }
    if (phys)
        *phys = placement_address;
    u32 tmp = placement_address;
    placement_address += sz;
    return tmp;
}

u32 kmalloc(u32 sz)
{
    return kmalloc_int(sz, 0, 0);
}
*/