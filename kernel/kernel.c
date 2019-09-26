#include "../drivers/screen.h"
#include "../drivers/keyboard.h"

#include "../cpu/port.h"
#include "../cpu/isr.h"
#include "../cpu/timer.h"

#include "../libc/string.h"
#include "../libc/mem.h"

#include "kernel.h"

void kernel_main()
{
    isr_install();
    iqr_install();

    clear_screen();

    kprint("Type something, it will go through the kernel\n"
           "Type END to halt the CPU or PAGE to request a kmalloc()\n"
           "> ");
}

void user_input(char *input)
{
    if (strcmp(input, "END") == 0)
    {
        kprint("Stopping the CPU. Bye!\n");
        asm volatile("hlt");
    }
    else if (strcmp(input, "PAGE") == 0)
    {
        u32 phys_addr;
        u32 page = kmalloc(1000, 1, &phys_addr);
        char page_str[16] = "";
        hex_to_ascii(page, page_str);
        char phys_str[16] = "";
        hex_to_ascii(phys_addr, phys_str);
        kprint("Page: ");
        kprint(page_str);
        kprint(", physical address: ");
        kprint(phys_str);
        kprint("\n");
    }
    else if (strcmp(input, "CLEAR") == 0)
    {
        clear_screen();
    }

    kprint("\n> ");
}
