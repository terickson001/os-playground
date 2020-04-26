#include <drivers/screen.h>
#include <drivers/keyboard.h>
#include <drivers/fs/initrd.h>

#include <cpu/port.h>
#include <cpu/isr.h>
#include <cpu/timer.h>

#include <libc/string.h>
#include <libc/mem.h>
#include <libc/function.h>

#include <kernel/kernel.h>
#include <kernel/paging.h>
#include <kernel/fs.h>

#include <boot/multiboot.h>

extern u32 placement_address;
static char termbuf[4096];

void term_execute()
{
    if (strcmp(termbuf, "exit") == 0)
    {
        kprint("Stopping the CPU. Bye!\n");
        asm volatile("hlt");
    }
    else if (strcmp(termbuf, "clear") == 0)
    {
        clear_screen();
    }
    
    memory_set(termbuf, 0, sizeof(termbuf));
}

void term_input(Kbd_Event e)
{
    if (e.released)
        return;
    if (e.ascii == '\n')
    {
        kprint("\n");
        term_execute();
        kprint("\n> ");
    }
    else if (e.ascii == '\b')
    {
        i32 len = strlen(termbuf);
        if (len)
        {
            termbuf[len-1] = 0;
            kprint_backspace();
        }
    }
    else
    {
        char str[2] = {e.ascii, 0};
        kprint(str);
        termbuf[strlen(termbuf)] = e.ascii;
    }
}

/*
void serial_write(u8 data)
{
    while((port_byte_in(COM1 + 5) & 0x20) == 0);
    port_byte_out(COM1, data);
}
*/

void kernel_main(u32 magic, Multiboot *mb)
{
    
    kprint_hex(magic); kprint("\n");
    kprint_hex((u32)mb); kprint("\n");
    if (magic != 0x2BADB002)
        return;
    
    u32 initrd_location = *((u32*)mb->mods_addr);
    u32 initrd_end = *(u32*)(mb->mods_addr+4);
    // Don't trample our module with placement accesses, please!
    placement_address = initrd_end;
    
    isr_install();
    iqr_install();
    
    clear_screen();
    
    register_keyboard_hook(&term_input);
    
    kprint("Type something, it will go through the kernel\n"
           "Type `exit` to halt the CPU or `page` to request a kmalloc()\n"
           "> ");
    
    fs_root = init_initrd(initrd_location);
    
    int i = 0;
    Directory_Entry *node = 0;
    while ( (node = fs_read_dir(fs_root, i)) != 0)
    {
        kprint("Found file ");
        kprint(node->name);
        File_System_Node *fsnode = fs_find_dir(fs_root, node->name);
        
        if ((fsnode->flags&0x7) == FS_DIRECTORY)
            kprint("\n\t(directory)\n");
        else
        {
            kprint("\n\t contents: \"");
            char buf[256];
            u32 sz = fs_read(fsnode, 0, 255, (byte *)buf);
            buf[sz] = 0;
            kprint(buf);
            kprint("\"\n");
        }
        i++;
    }
}