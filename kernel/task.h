#ifndef KERNEL_TASK_H
#define KERNEL_TASK_H

#include <kernel/paging.h>

#define MAX_PROCESSES       32768
#define MAX_THREADS         (MAX_PROCESSES*16)
#define THREADS_PER_PROCESS 1024

typedef struct Thread_Ctx
{
    struct
    {
        u32 ebp;
        u32 edi;
        u32 esi;
        u32 edx;
        u32 ecx;
        u32 ebx;
        u32 eax;
        u32 eip;
        u32 cs;
        u32 rflags;
        u32 esp;
        u32 ss;
    } regs;
} Thread_Ctx;

typedef struct Task
{
    u32 tid;
    u32 process;
    Thread_Ctx ctx;
} Task;

typedef struct Process
{
    u32 pid;
    Page_Directory *page_directory;
    Task **threads;
} Process;


void init_tasking();
void task_switch();
int fork();
void move_stack(void *new_stack_start, u32 size);
int get_pid();

#endif // KERNEL_TASK_H
