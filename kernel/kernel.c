#include "../drivers/screen.h"
#include "../drivers/keyboard.h"

#include "../cpu/port.h"
#include "../cpu/isr.h"
#include "../cpu/timer.h"

#include "../libc/string.h"
#include "../libc/mem.h"

#include "kernel.h"

static char termbuf[4096];

void term_execute()
{
    if (strcmp(termbuf, "exit") == 0)
    {
        kprint("Stopping the CPU. Bye!\n");
        asm volatile("hlt");
    }
    /* else if (strcmp(termbuf, "page") == 0) */
    /* { */
    /*     u32 phys_addr; */
    /*     u32 page = kmalloc(1000, 1, &phys_addr); */
    /*     char page_str[16] = ""; */
    /*     hex_to_ascii(page, page_str); */
    /*     char phys_str[16] = ""; */
    /*     hex_to_ascii(phys_addr, phys_str); */
    /*     kprint("Page: "); */
    /*     kprint(page_str); */
    /*     kprint(", physical address: "); */
    /*     kprint(phys_str); */
    /* } */
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



void kernel_main()
{
    isr_install();
    iqr_install();

    clear_screen();

    register_keyboard_hook(&term_input);
    kprint("Type something, it will go through the kernel\n"
           "Type `exit` to halt the CPU or `page` to request a kmalloc()\n"
           "> ");
}

