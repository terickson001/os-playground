#include <kernel/task.h>

volatile u32 current_tid;
volatile Thread **thread_table;

u32 new_tid(Thread **table, u32 max_ids)
{
    for (u32 i = 0; i < max_ids; i++)
    {
        if (table[i] == 0) 
            return i;
    }
    return ~0;
}

void init_tasking()
{
    Thread *thread = (Thread *)kmalloc(sizeof(Thread));
    thread->tid = new_tid((Thread **)thread_table, MAX_THREADS);
    current_tid = thread->tid;
}

