#include <kernel/paging.h>

#include <libc/mem.h>
#include <libc/string.h>
#include <libc/function.h>

#include <drivers/screen.h>

#include <kernel/kheap.h>

extern void load_page_directory(u32 *page_directory);
extern void enable_paging();
extern void copy_page_frame();

#define STRING_(x) #x
#define STRING(x) STRING_(x)
#define PANIC(msg) \
do {   \
kprint("PANIC("); \
kprint(msg);     \
kprint(") at "); \
kprint(__FILE__":"STRING(__LINE__)); \
kprint("\n"); \
while(true){}; \
} while (false);

extern Heap *kheap;

;
Page_Directory *kernel_directory = 0;
Page_Directory *current_directory = 0;
u32 *frames;
u32 nframes;

extern u32 end_kernel;
u32 placement_address = 0;

#define INDEX_FROM_BIT(a)  (a/(32))
#define OFFSET_FROM_BIT(a) (a%(32))

u32 kmalloc_int(u32 sz, b32 align, u32 *phys)
{
    if (kheap)
    {
        u32 addr = (u32)alloc(sz, align, kheap);
        if (phys)
        {
            Page *page = get_page((u32)addr, 0, kernel_directory);
            *phys = page->frame*0x1000 + ((u32)addr&0xFFF);
        }
        return addr;
    }
    
    if (align == 1 && (placement_address & 0xFFF)) // Not Aligned?
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
u32 kmalloc_a(u32 sz)             { return kmalloc_int(sz, true, 0);     }
u32 kmalloc_p(u32 sz, u32 *phys)  { return kmalloc_int(sz, false, phys); }
u32 kmalloc_ap(u32 sz, u32 *phys) { return kmalloc_int(sz, true, phys);  }
u32 kmalloc(u32 sz)               { return kmalloc_int(sz, false, 0);    }

void kfree(u32 p)
{
    if (kheap) free((void *)p, kheap);
}

// Bit Set
static void set_frame_used(u32 frame_addr)
{
    u32 frame = frame_addr/0x1000;
    u32 idx = INDEX_FROM_BIT(frame);
    u32 off = OFFSET_FROM_BIT(frame);
    frames[idx] |= (0x1 << off);
}

static void set_frame_free(u32 frame_addr)
{
    u32 frame = frame_addr/0x1000;
    u32 idx = INDEX_FROM_BIT(frame);
    u32 off = OFFSET_FROM_BIT(frame);
    frames[idx] &= ~(0x1 << off);
}

static u32 test_frame(u32 frame_addr)
{
    u32 frame = frame_addr/0x1000;
    u32 idx = INDEX_FROM_BIT(frame);
    u32 off = OFFSET_FROM_BIT(frame);
    return (frames[idx] & (0x1 << off));
}

static u32 first_free_frame()
{
    u32 i, j;
    for (i = 0; i < INDEX_FROM_BIT(nframes); i++)
    {
        if (frames[i] != 0xFFFFFFFF)
        {
            for (j = 0; j < 32; j++)
            {
                u32 toTest = 0x1 << j;
                if ( !(frames[i] & toTest) )
                    return i*32+j;
            }
        }
    }
    return (u32)-1;
}

void alloc_frame(Page *page, b32 is_kernel, b32 is_writeable)
{
    if (page->frame != 0) // Already allocated
        return;
    
    u32 idx = first_free_frame();
    if (idx == (u32)-1)
        PANIC("NO FREE PAGES");
    set_frame_used(idx*0x1000);
    page->present = true;
    page->rw = !!is_writeable;
    page->user = !is_kernel;
    page->frame = idx;
}

void free_frame(Page *page)
{
    u32 frame = page->frame;
    if (!frame) // Already free
        return;
    set_frame_free(frame);
    page->frame = 0x0;
}

#define FIELD_OFFSET(STRUCT, FIELD) ((u32)((STRUCT)->FIELD) - (u32)(STRUCT))

Page_Table *clone_page_table(Page_Table *src, u32 *phys)
{
    Page_Table *dest = (Page_Table *)kmalloc_ap(sizeof(Page_Table), phys);
    memory_set(dest, 0, sizeof(Page_Table));
    
    for (int i = 0; i < 1024; i++)
    {
        if (!src->pages[i].frame)
            continue;
        
        alloc_frame(&dest->pages[i], 0, 0);
        dest->pages[i].present  = src->pages[i].present;
        dest->pages[i].rw       = src->pages[i].rw;
        dest->pages[i].user     = src->pages[i].user;
        dest->pages[i].accessed = src->pages[i].accessed;
        dest->pages[i].dirty    = src->pages[i].dirty;
        
        copy_page_frame(src->pages[i].frame*0x1000, dest->pages[i].frame*0x1000);
    }
    return dest;
}

Page_Directory *clone_page_directory(Page_Directory *src)
{
    u32 phys;
    Page_Directory *dest = (Page_Directory *)kmalloc_ap(sizeof(Page_Directory), &phys);
    memory_set(dest, 0, sizeof(Page_Directory));
    
    u32 offset = (u32)dest->entries - (u32)dest; //FIELD_OFFSET(dest, entries);
    dest->address = phys + offset;
    for (int i = 0; i < 1024; i++)
    {
        if (!src->tables[i])
            continue;
        if (kernel_directory->tables[i] == src->tables[i]) // Leave Kernel Tables
        {
            dest->tables[i] = src->tables[i];
            dest->entries[i] = src->entries[i];
        }
        else // Copy Everything else
        {
            u32 phys;
            dest->tables[i] = clone_page_table(src->tables[i], &phys);
            dest->entries[i].raw = phys | 0x7;
        }
    }
    return dest;
}


Page *get_page(u32 address, b32 make, Page_Directory *dir)
{
    // Turn the address into an index.
    address /= 0x1000;
    // Find the page table containing this address.
    u32 table_idx = address / 1024;
    if (dir->tables[table_idx]) // If this table is already assigned
    {
        return &dir->tables[table_idx]->pages[address%1024];
    }
    else if(make)
    {
        u32 tmp;
        dir->tables[table_idx] = (Page_Table*)kmalloc_ap(sizeof(Page_Table), &tmp);
        memory_set(dir->tables[table_idx], 0, 0x1000);
        dir->entries[table_idx].raw = tmp | 0x3; // PRESENT, RW, US.
        return &dir->tables[table_idx]->pages[address%1024];
    }
    else
    {
        return 0;
    }
}

Page_Directory *create_address_space()
{
    Page_Directory *directory = (Page_Directory *)kmalloc(sizeof(Page_Directory));
    return directory;
}

void init_paging()
{
    if (!placement_address)
        placement_address = (u32)&end_kernel;
    
    u32 mem_end_page = 0x1000000; // End of memory (Assume 16MiB for now)
    
    nframes = mem_end_page / 0x1000;
    frames = (u32*)kmalloc(INDEX_FROM_BIT(nframes));
    memory_set(frames, 0, INDEX_FROM_BIT(nframes));
    
    kernel_directory = (Page_Directory *)kmalloc_a(sizeof(Page_Directory));
    memory_set(kernel_directory, 0, sizeof(Page_Directory));
    kernel_directory->address = (u32)kernel_directory->entries;
    
    u32 i;
    
    // Create the page tables for the kheap
    i = 0;
    while (i < KHEAP_INITIAL_SIZE)
    {
        get_page(KHEAP_START + i, 1, kernel_directory);
        i += 0x1000;
    }
    
    // Identity map the kernel
    i = 0;
    while (i < placement_address+0x1000)
    {
        alloc_frame(get_page(i, true, kernel_directory), false, false);
        i += 0x1000;
    }
    
    // Allocate frames for the kheap
    i = 0;
    while (i < KHEAP_INITIAL_SIZE)
    {
        alloc_frame(get_page(KHEAP_START + i, 1, kernel_directory), false, false);
        i += 0x1000;
    }
    
    register_interrupt_handler(14, page_fault);
    load_page_directory((u32*)kernel_directory->address);
    enable_paging();
    
    kheap = make_heap(KHEAP_START, KHEAP_START+KHEAP_INITIAL_SIZE, 0xCFFFF000, false, false);
    
    current_directory = clone_page_directory(kernel_directory);
    load_page_directory((u32*)current_directory->address);
}

void page_fault(Registers *regs)
{
    u32 faulting_address;
    asm("mov %%cr2, %0" : "=r" (faulting_address));
    
    b32 present = !(regs->err_code & 0x1); // Page not present
    b32 rw = regs->err_code & 0x02; // Write operation?
    b32 us = regs->err_code & 0x04; // User Mode?
    b32 reserved = regs->err_code & 0x08; // CPU-Reserved
    b32 id = regs->err_code & 0x10; // Instruction fetch?
    UNUSED(id);
    
    kprintf("Page fault! ( %s%s%s%s) at 0x%X\n",
            present ?"present "  :0,
            rw      ?"read-only ":0,
            us      ?"user-mode ":"",
            reserved?"reserved " :"",
            faulting_address);
    dump_registers(regs);
    PANIC("Page fault");
}