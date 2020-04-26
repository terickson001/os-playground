#ifndef KERNEL_ORDERED_ARRAY_H
#define KERNEL_ORDERED_ARRAY_H

#include <cpu/types.h>

typedef i32 Compare_Proc(void *a, void *b);
typedef struct Ordered_Array
{
    void **data;
    u32 count;
    u32 cap;
    Compare_Proc *compare;
} Ordered_Array;

i32 standard_comparison(void *a, void *b);

Ordered_Array make_ordered_array(u32 cap, Compare_Proc *cmp);
Ordered_Array place_ordered_array(void *addr, u32 cap, Compare_Proc *cmp);
void destroy_ordered_array(Ordered_Array *arr);
void ordered_array_insert(Ordered_Array *array, void *v);
void *ordered_array_get(Ordered_Array *array, u32 i);
void ordered_array_remove(Ordered_Array *array, u32 i);

#endif // KERNEL_ORDERED_ARRAY_H
