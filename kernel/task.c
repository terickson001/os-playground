#include <kernel/task.h>
#include <kernel/kernel.h>

#include <libc/mem.h>

extern u32 read_eip();
extern void perform_task_switch(u32 eip, u32 directory_addr, u32 ebp, u32 esp);

volatile Task *current_task;
volatile Process **process_table;
volatile Task **task_table;

u32 next_pid = 1;

void init_tasking()
{
    asm volatile("cli");
    
    // Allocate Task Table
    task_table = (Task **)kmalloc(MAX_THREADS*sizeof(Task *));
    
    // Allocate Process Table
    process_table = (Process **)kmalloc(MAX_PROCESSES*sizeof(Task *));
    
    // Create Process for kernel
    process_table[0] = (Process *)kmalloc(sizeof(Process));
    process_table[0]->threads = kmalloc(THREADS_PER_PROCESS*sizeof(Task *));
    process_table[0]->page_directory = kernel_directory;
    process_table[0]->pid = 0;
    
    asm volatile("sti");
}

u32 _new_id(void *table, u32 max_ids)
{
    for (u32 id = 0; id < max_ids; id++)
    {
        if (table[id] == 0)
            return id;
    }
    return -1;
}

u32 create_process()
{
    asm volatile("cli");
    
    // Find available pid
    u32 pid = _new_id(process_table, MAX_PROCESSES);
    if (pid == -1)
        return pid;
    
    Process *new_process = kmalloc(sizeof(Process));
    new_process->threads = kmalloc(sizeof(Task *) * THREADS_PER_PROCESS);
    new_process->page_directory = create_address_space();
    return pid;
}

int fork()
{
    asm volatile("cli");
    
    Task *parent  = (Task *)current_task;
    Page_Directory *directory = clone_page_directory(current_directory);
    
    Task *child = (Task *)kmalloc(sizeof(Task));
    child->id = next_pid++;
    child->esp = child->ebp = 0;
    child->eip = 0;
    child->page_directory = directory;
    child->next = 0;
    
    Task *tmp = (Task *)ready_queue;
    while (tmp->next)
        tmp = tmp->next;
    tmp->next = child;
    
    u32 eip = read_eip();
    
    if (current_task != parent)
        return 0;
    
    u32 esp, ebp;
    asm volatile("mov %%esp, %0" : "=r"(esp));
    asm volatile("mov %%ebp, %0" : "=r"(ebp));
    
    child->esp = esp;
    child->ebp = ebp;
    child->eip = eip;
    
    return child->id;
}

void task_switch()
{
    // Check if tasking has been init'd
    if (!current_task)
        return;
    // serial_write_str("SWITCHING\n");
    u32 esp, ebp;
    asm volatile("mov %%esp, %0" : "=r"(esp));
    asm volatile("mov %%ebp, %0" : "=r"(ebp));
    
    u32 eip = read_eip();
    
    if (eip == 0xBEEF) // Dummy Value, this is the new process
        return;
    
    current_task->eip = eip;
    current_task->esp = esp;
    current_task->ebp = ebp;
    
    current_task = current_task->next;
    if (!current_task)
        current_task = ready_queue;
    
    esp = current_task->esp;
    ebp = current_task->ebp;
    
    perform_task_switch(eip, current_directory->address, esp, ebp);
}

int get_pid()
{
    return current_task->id;
}