#include "kheap.h"
#include "paging.h"
#include "../drivers/screen.h"

Heap *kheap = 0;

static i32 find_smallest_hole(u32 size, b8 page_align, Heap *heap)
{
    u32 i = 0;
    while (i < heap->index.count)
    {
        Alloc_Header *header = ordered_array_get(&heap->index, i);
        if (page_align)
        {
            u32 location = (u32)header;
            i32 offset = 0;
            if (((location+sizeof(Alloc_Header)) & 0xFFF) != 0)
                offset = 0x1000 - (location + sizeof(Alloc_Header)) % 0x1000;
            i32 hole_size = (i32)header->size - offset;
            if (hole_size >= (i32)size)
                break;
        }
        else if (header->size >= size)
        {
            break;
        }
        i++;
    }
    
    if (i == heap->index.count)
        return -1; // Didn't find a hole
    else
        return i;
}

static b32 heap_header_compare(void *a, void *b)
{
    Alloc_Header *A = (Alloc_Header *)a;
    Alloc_Header *B = (Alloc_Header *)b;
    return (i32)(A->size) - (i32)(B->size);
}

Heap *make_heap(u32 start, u32 end, u32 max, b32 supervisor, b32 readonly)
{
    Heap *heap = (Heap *)kmalloc(sizeof(Heap));
    
    if (start & 0xFFF || end & 0xFFF)
        return 0;
    
    heap->index = place_ordered_array((void *)start, HEAP_INDEX_SIZE, heap_header_compare);
    start += sizeof(void *)*HEAP_INDEX_SIZE;
    
    if (start & 0xFFF)
    {
        start &= 0xFFFFF000;
        start += 0x1000;
    }
    
    heap->start = start;
    heap->end = end;
    heap->max = max;
    heap->supervisor = supervisor;
    heap->readonly = readonly;
    
    Alloc_Header *hole = (Alloc_Header *)start;
    hole->size = end - start;
    hole->magic = HEAP_MAGIC;
    hole->used = false;
    ordered_array_insert(&heap->index, hole);
    
    return heap;
}

static void expand_heap(u32 new_size, Heap *heap)
{
    if (new_size & 0xFFF)
    {
        new_size &= 0xFFFFF000;
        new_size += 0x1000;
    }
    
    u32 old_size = heap->end - heap->start;
    u32 i = old_size;
    while (i < new_size)
    {
        alloc_frame(get_page(heap->start+i, 1, kernel_directory),
                    heap->supervisor, !heap->readonly);
        i += 0x1000;
    }
    heap->end = heap->start + new_size;
}

static u32 contract_heap(u32 new_size, Heap *heap)
{
    if (new_size & 0xFFF)
    {
        new_size &= 0xFFFFF000;
        new_size += 0x1000;
    }
    
    if (new_size < HEAP_MIN_SIZE)
        new_size = HEAP_MIN_SIZE;
    u32 old_size = heap->end - heap->start;
    u32 i = old_size-0x1000;
    while (new_size < i)
    {
        free_frame(get_page(heap->start+i, 0, kernel_directory));
        i -= 0x1000;
    }
    heap->end = heap->start + new_size;
    return new_size;
}

void *alloc(u32 size, b32 page_align, Heap *heap)
{
    u32 req_size = size + sizeof(Alloc_Header) + sizeof(Alloc_Footer);
    i32 i = find_smallest_hole(req_size, page_align, heap);
    
    if (i == -1) // No Hole Found
    {
        u32 old_length = heap->end - heap->start;
        u32 old_end = heap->end;
        
        expand_heap(old_length+req_size, heap);
        u32 new_length = heap->end - heap->start;
        
        i = 0;
        u32 max = 0x0, max_idx = (u32)-1;
        while (i < (i32)heap->index.count)
        {
            u32 tmp = (u32)ordered_array_get(&heap->index, i);
            if (tmp > max)
            {
                max = tmp;
                max_idx = i;
            }
            i++;
        }
        
        if (max_idx == (u32)-1) // Need to add a header
        {
            Alloc_Header *header = (Alloc_Header *)old_end;
            header->magic = HEAP_MAGIC;
            header->size = new_length - old_length;
            header->used = false;
            Alloc_Footer *footer = (Alloc_Footer *)((u32)header + header->size) - 1;
            footer->magic = HEAP_MAGIC;
            footer->header = header;
            ordered_array_insert(&heap->index, header);
        }
        {
            // Adjust header
            Alloc_Header *header = ordered_array_get(&heap->index, max_idx);
            header->size += new_length - old_length;
            
            Alloc_Footer *footer = (Alloc_Footer *)((u32)header + header->size)-1;
            footer->magic = HEAP_MAGIC;
            footer->header = header;
        }
        
        return alloc(size, page_align, heap);
    }
    
    Alloc_Header *orig_header = ordered_array_get(&heap->index, i);
    u32 orig_pos = (u32)orig_header;
    u32 orig_size = orig_header->size;
    
    if (orig_size - req_size <= sizeof(Alloc_Header)+sizeof(Alloc_Footer)) // No space left?
    {
        size += orig_size - req_size; // Expand to fill the hole
        req_size = orig_size;
    }
    
    if (page_align && (orig_pos+sizeof(Alloc_Header)) & 0xFFF)
    {
        u32 new_location = (orig_pos & 0xFFFFF000) + 0x1000 - sizeof(Alloc_Header);
        orig_header->size = new_location - orig_pos;
        orig_header->magic = HEAP_MAGIC;
        orig_header->used = false;
        
        Alloc_Footer *hole_footer = (Alloc_Footer *)(new_location)-1;
        hole_footer->magic = HEAP_MAGIC;
        hole_footer->header = orig_header;
        orig_pos = new_location;
        orig_size = orig_size - orig_header->size;
    }
    else
    {
        ordered_array_remove(&heap->index, i);
    }
    
    Alloc_Header *new_header = (Alloc_Header *)orig_pos;
    new_header->magic = HEAP_MAGIC;
    new_header->used = true;
    new_header->size = req_size;
    
    Alloc_Footer *new_footer = (Alloc_Footer *)(orig_pos + sizeof(Alloc_Header) + size);
    new_footer->magic = HEAP_MAGIC;
    new_footer->header = new_header;
    
    if (orig_size - req_size > 0)
    {
        Alloc_Header *hole_header = (Alloc_Header *)((u32)new_header + new_header->size);
        hole_header->magic = HEAP_MAGIC;
        hole_header->used = false;
        hole_header->size = new_header->size - req_size;
        
        Alloc_Footer *hole_footer = (Alloc_Footer *)((u32)hole_header + hole_header->size)-1;
        if ((u32)hole_footer < heap->end-sizeof(Alloc_Footer))
        {
            hole_footer->magic  = HEAP_MAGIC;
            hole_footer->header = hole_header;
        }
        
        ordered_array_insert(&heap->index, hole_header);
    }
    
    return new_header+1;
}

void free(void *p, Heap *heap)
{
    if (p == 0)
        return;
    
    Alloc_Header *header = (Alloc_Header *)p - 1;
    Alloc_Footer *footer = (Alloc_Footer *)((u32)header + header->size) - 1;
    
    // if (header->magic != HEAP_MAGIC || footer->magic != HEAP_MAGIC)
    
    header->used = false;
    b32 add_to_index = true;
    
    Alloc_Footer *left_footer = (Alloc_Footer *)header - 1;
    if (left_footer->magic == HEAP_MAGIC && !left_footer->header->used)
    {
        Alloc_Header *left_header = left_footer->header;
        left_header->size += header->size;
        footer->header = left_header;
        header = left_header;
        add_to_index = false;
    }
    
    Alloc_Header *right_header = (Alloc_Header *)(footer + 1);
    if (right_header->magic == HEAP_MAGIC && !right_header->used)
    {
        Alloc_Footer *right_footer = (Alloc_Footer *)((u32)right_header + right_header->size)-1;
        header->size += right_header->size;
        right_footer->header = header;
        footer = right_footer;
        
        u32 i = 0;
        while (i < heap->index.count && ordered_array_get(&heap->index, i) != right_header)
            i++;
        if (i < heap->index.count)
            ordered_array_remove(&heap->index, i);
    }
    
    if ((u32)(footer+1) == heap->end) // Hole is at the end of the heap
    {
        // Contract
        u32 old_length = heap->end - heap->start;
        u32 new_length = contract_heap((u32)header - heap->start, heap);
        
        if (header->size - (old_length - new_length) > 0)
        {
            header->size -= old_length - new_length;
            footer = (Alloc_Footer *)((u32)header + header->size)-1;
            footer->magic = HEAP_MAGIC;
            footer->header = header;
        }
        else
        {
            u32 i = 0;
            while (i < heap->index.count && ordered_array_get(&heap->index, i) != header)
                i++;
            if (i < heap->index.count)
                ordered_array_remove(&heap->index, i);
        }
    }
    
    if (add_to_index)
        ordered_array_insert(&heap->index, header);
}