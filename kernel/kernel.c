#include "../drivers/port.h"
#include "../drivers/screen.h"
#include "../drivers/keyboard.h"

#include "../cpu/isr.h"
#include "../cpu/timer.h"

#include "util.h"

void main()
{
    isr_install();
    clear_screen();

    asm volatile("sti");
    //    init_timer(50);
    init_keyboard();
    

}
