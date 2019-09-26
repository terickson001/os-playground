#include "keyboard.h"
#include "screen.h"

#include "../cpu/port.h"
#include "../cpu/isr.h"
#include "../cpu/types.h"

#include "../libc/string.h"
#include "../libc/mem.h"
#include "../libc/function.h"

#include "../kernel/kernel.h"

#define SC_MAX 57

#define ENTER     0x1C
#define BACKSPACE 0x0E

static char key_buffer[256];

const char *sc_name[] = {
    "ERROR", "Esc", "1", "2", "3", "4", "5", "6", 
    "7", "8", "9", "0", "-", "=", "Backspace", "Tab", "Q", "W", "E", 
    "R", "T", "Y", "U", "I", "O", "P", "[", "]", "Enter", "Lctrl", 
    "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", "`", 
    "LShift", "\\", "Z", "X", "C", "V", "B", "N", "M", ",", ".", 
    "/", "RShift", "Keypad *", "LAlt", "Spacebar"
};

const char sc_ascii[] = {
    '?', '?', '1', '2', '3', '4', '5', '6',     
    '7', '8', '9', '0', '-', '=', '?', '?', 'Q', 'W', 'E', 'R', 'T', 'Y', 
    'U', 'I', 'O', 'P', '[', ']', '?', '?', 'A', 'S', 'D', 'F', 'G', 
    'H', 'J', 'K', 'L', ';', '\'', '`', '?', '\\', 'Z', 'X', 'C', 'V', 
    'B', 'N', 'M', ',', '.', '/', '?', '?', '?', ' '
};

static void keyboard_callback(Registers regs)
{
    UNUSED(regs);
    
    u8 scancode = port_byte_in(0x60);
    if (scancode > SC_MAX) return; // Scancode not implemented
    if (scancode == BACKSPACE)
    {
        kprint_backspace();
        i32 len = strlen(key_buffer);
        key_buffer[len-1] = 0;
    }
    else if (scancode == ENTER)
    {
        kprint("\n");
        user_input(key_buffer);
        memory_set(key_buffer, 0, sizeof(key_buffer));
    }
    else
    {
        char letter = sc_ascii[scancode];
        char str[2] = {letter, 0};
        kprint(str);
        key_buffer[strlen(key_buffer)] = letter;
    }
}

void init_keyboard()
{
    register_interrupt_handler(IRQ1, &keyboard_callback);
}
