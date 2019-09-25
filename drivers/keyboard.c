#include "keyboard.h"
#include "port.h"
#include "screen.h"

#include "../cpu/isr.h"
#include "../cpu/types.h"

#include "../kernel/util.h"

void print_key(u8 scancode);
static void keyboard_callback(Registers regs)
{
    u8 scancode = port_byte_in(0x60);
    char sc_ascii[4];
    int_to_ascii((int)scancode, sc_ascii);
    kprint("Keyboard scancode: ");
    kprint(sc_ascii);
    kprint(", ");
    print_key(scancode);
    kprint("\n");
}

void init_keyboard()
{
    register_interrupt_handler(IRQ1, &keyboard_callback);
}

static char *scancode_names[] = {
    "ERROR",
    "ESC",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "0",
    "-",
    "+",
    "Backspace",
    "Tab",
    "Q",
    "W",
    "E",
    "R",
    "T",
    "Y",
    "U",
    "I",
    "O",
    "P",
    "[",
    "]",
    "ENTER",
    "LCtrl",
    "A",
    "S",
    "D",
    "F",
    "G",
    "H",
    "J",
    "K",
    "L",
    ";",
    "'",
    "`",
    "LShift",
    "\\",
    "Z",
    "X",
    "C",
    "V",
    "B",
    "N",
    "M",
    ",",
    ".",
    "/",
    "Rshift",
    "Keypad *",
    "LAlt",
    "Spc",
};

void print_key(u8 scancode)
{
    if (scancode <= 0x39)
    {
        kprint(scancode_names[scancode]);
    }
    else if (scancode <= 0x7f)
    {
        kprint("Unknown key down");
    }
    else if (scancode <= 0x39 + 0x80)
    {
        kprint("KEY UP: ");
        print_key(scancode - 0x80);
    }
    else
    {
        kprint("Unknown key up");
    }
}
