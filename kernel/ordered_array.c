#include <kernel/ordered_array.h>
#include <kernel/paging.h>
#include <kernel/kheap.h>

#include <libc/mem.h>

i32 standard_comparison(void *a, void *b)
{
    return *(i32 *)a - *(i32 *)b;
}

Ordered_Array make_ordered_array(u32 cap, Compare_Proc *cmp)
{
    Ordered_Array arr;
    
    arr.data = (void **)kmalloc(cap*sizeof(void *));
    memory_set(arr.data, 0, cap*sizeof(void *));
    arr.count = 0;
    arr.cap = cap;
    arr.compare = cmp ? cmp : standard_comparison;
    
    return arr;
}

Ordered_Array place_ordered_array(void *addr, u32 cap, Compare_Proc *cmp)
{
    Ordered_Array arr;
    
    arr.data = (void **)addr;
    memory_set(arr.data, 0, cap*sizeof(void *));
    arr.count = 0;
    arr.cap = cap;
    arr.compare = cmp ? cmp : standard_comparison;
    
    return arr;
}

void destroy_ordered_array(Ordered_Array *arr)
{
    kfree((u32)arr->data);
}

void ordered_array_insert(Ordered_Array *arr, void *v)
{
    u32 i = 0;
    while (i < arr->count && arr->compare(arr->data[i], v) < 0)
        i++;
    if (i == arr->count)
    {
        arr->data[arr->count++] = v;
    }
    else
    {
        void *tmp = arr->data[i];
        arr->data[i] = v;
        while (i < arr->count)
        {
            i++;
            void *tmp2 = arr->data[i];
            arr->data[i] = tmp;
            tmp = tmp2;
        }
        arr->count++;
    }
}

void *ordered_array_get(Ordered_Array *arr, u32 i)
{
    if (i > arr->count)
        return 0;
    return arr->data[i];
}

void ordered_array_remove(Ordered_Array *arr, u32 i)
{
    while (i < arr->count)
    {
        arr->data[i] = arr->data[i+1];
        i++;
    }
    arr->count--;
}
