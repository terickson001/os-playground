#include "keyboard.h"
#include "screen.h"

#include "../cpu/port.h"
#include "../cpu/isr.h"

#include "../libc/string.h"
#include "../libc/mem.h"
#include "../libc/function.h"

#include "../kernel/kernel.h"

static Keyboard_State STATE = {0};
static Keyboard_Hook *key_hooks[256];

enum Scancode_Set_1 {
    SC_ESCAPE = 0x01,
    SC_1 = 0x02,
    SC_2 = 0x03,
    SC_3 = 0x04,
    SC_4 = 0x05,
    SC_5 = 0x06,
    SC_6 = 0x07,
    SC_7 = 0x08,
    SC_8 = 0x09,
    SC_9 = 0x0A,
    SC_0 = 0x0B,
    SC_MINUS = 0x0C,
    SC_EQUALS = 0x0D,
    SC_BACKSPACE = 0x0E,
    SC_TAB = 0x0F,
    SC_Q = 0x10,
    SC_W = 0x11,
    SC_E = 0x12,
    SC_R = 0x13,
    SC_T = 0x14,
    SC_Y = 0x15,
    SC_U = 0x16,
    SC_I = 0x17,
    SC_O = 0x18,
    SC_P = 0x19,
    SC_BRACKETLEFT = 0x1A,
    SC_BRACKETRIGHT = 0x1B,
    SC_ENTER = 0x1C,
    SC_CONTROLLEFT = 0x1D,
    SC_A = 0x1E,
    SC_S = 0x1F,
    SC_D = 0x20,
    SC_F = 0x21,
    SC_G = 0x22,
    SC_H = 0x23,
    SC_J = 0x24,
    SC_K = 0x25,
    SC_L = 0x26,
    SC_SEMICOLON = 0x27,
    SC_APOSTROPHE = 0x28,
    SC_GRAVE = 0x29,
    SC_SHIFTLEFT = 0x2A,
    SC_BACKSLASH = 0x2B,
    SC_Z = 0x2C,
    SC_X = 0x2D,
    SC_C = 0x2E,
    SC_V = 0x2F,
    SC_B = 0x30,
    SC_N = 0x31,
    SC_M = 0x32,
    SC_COMMA = 0x33,
    SC_PERIOD = 0x34,
    SC_SLASH = 0x35,
    SC_SHIFTRIGHT = 0x36,
    SC_NUMPAD_MULTIPLY = 0x37,
    SC_ALTLEFT = 0x38,
    SC_SPACE = 0x39,
    SC_CAPSLOCK = 0x3A,
    SC_F1 = 0x3B,
    SC_F2 = 0x3C,
    SC_F3 = 0x3D,
    SC_F4 = 0x3E,
    SC_F5 = 0x3F,
    SC_F6 = 0x40,
    SC_F7 = 0x41,
    SC_F8 = 0x42,
    SC_F9 = 0x43,
    SC_F10 = 0x44,
    SC_NUMLOCK = 0x45,
    SC_SCROLLLOCK = 0x46,
    SC_NUMPAD_7 = 0x47,
    SC_NUMPAD_8 = 0x48,
    SC_NUMPAD_9 = 0x49,
    SC_NUMPAD_MINUS = 0x4A,
    SC_NUMPAD_4 = 0x4B,
    SC_NUMPAD_5 = 0x4C,
    SC_NUMPAD_6 = 0x4D,
    SC_NUMPAD_PLUS = 0x4E,
    SC_NUMPAD_1 = 0x4F,
    SC_NUMPAD_2 = 0x50,
    SC_NUMPAD_3 = 0x51,
    SC_NUMPAD_0 = 0x52,
    SC_NUMPAD_PERIOD = 0x53,
    SC_BRACKETANGLE = 0x56, /* KEY BETWEEN THE LEFT SHIFT AND Z. */
    SC_F11 = 0x57,
    SC_F12 = 0x58,

    SC_MEDIA_PREVIOUS = 0xE010,
    SC_MEDIA_NEXT = 0xE019,
    SC_NUMPAD_ENTER = 0xE01C,
    SC_CONTROLRIGHT = 0xE01D,
    SC_VOLUME_MUTE = 0xE020,
    SC_LAUNCH_APP2 = 0xE021,
    SC_MEDIA_PLAY = 0xE022,
    SC_MEDIA_STOP = 0xE024,
    SC_VOLUME_DOWN = 0xE02E,
    SC_VOLUME_UP = 0xE030,
    SC_BROWSER_HOME = 0xE032,
    SC_NUMPAD_DIVIDE = 0xE035,
    SC_PRINTSCREEN = 0xE037,
    /*
    SC_PRINTSCREEN:
    - MAKE: 0xE02A 0xE037
    - BREAK: 0xE0B7 0xE0AA
    */
    SC_ALTRIGHT = 0xE038,
    SC_CANCEL = 0xE046, /* CTRL + PAUSE */
    SC_HOME = 0xE047,
    SC_ARROWUP = 0xE048,
    SC_PAGEUP = 0xE049,
    SC_ARROWLEFT = 0xE04B,
    SC_ARROWRIGHT = 0xE04D,
    SC_END = 0xE04F,
    SC_ARROWDOWN = 0xE050,
    SC_PAGEDOWN = 0xE051,
    SC_INSERT = 0xE052,
    SC_DELETE = 0xE053,
    SC_METALEFT = 0xE05B,
    SC_METARIGHT = 0xE05C,
    SC_APPLICATION = 0xE05D,
    SC_POWER = 0xE05E,
    SC_SLEEP = 0xE05F,
    SC_WAKE = 0xE063,
    SC_BROWSER_SEARCH = 0xE065,
    SC_BROWSER_FAVORITES = 0xE066,
    SC_BROWSER_REFRESH = 0xE067,
    SC_BROWSER_STOP = 0xE068,
    SC_BROWSER_FORWARD = 0xE069,
    SC_BROWSER_BACK = 0xE06A,
    SC_LAUNCH_APP1 = 0xE06B,
    SC_LAUNCH_EMAIL = 0xE06C,
    SC_LAUNCH_MEDIA = 0xE06D,

    SC_PAUSE = 0xE11D45,
};

const char sc_ascii[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
    '-', '=', '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
    '\n', 0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'',
    '`', 0, '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // FUNCTION KEYS
    '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.', // NUMPAD
    0, 0 // F11, F12
};

void send_key_event(u8 keycode, u8 released, char ascii)
{
    Kbd_Event event;
    event.state = STATE;
    event.released = released;
    event.keycode = keycode;
    event.ascii = ascii;

    for (int i = 0; i < 256; i++)
        if (key_hooks[i]) (*key_hooks[i])(event);
}

const char sc_ascii_shifted[128] = {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')',
    '_', '+', '\b', '\t',
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}',
    '\n', 0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"',
    '~', 0, '|',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' ', 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // FUNCTION KEYS
    '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.', // NUMPAD
    0, 0 // F11, F12
};

#define REG_KEY_DAT 0x60
#define REG_KEY_STS 0x64
#define REG_KEY_CMD 0x64

static void keyboard_callback(Registers *regs)
{
    UNUSED(regs);
    
    u8 keycode;
    
    u64 scancode = port_byte_in(REG_KEY_DAT);
    if (scancode == 0xE0)
    {
        scancode <<= 8;
        scancode += port_byte_in(REG_KEY_DAT);
        if (scancode == 0xE02A || scancode == 0xE0B7) // PrtScr special case
        {
            scancode <<= 16;
            scancode += port_byte_in(REG_KEY_DAT) << 8;
            scancode += port_byte_in(REG_KEY_DAT);
        }
    }
    else if (scancode == 0xE1) // Pause special case
    {
        scancode <<= 40;
        scancode += (u64)(port_byte_in(REG_KEY_DAT)) << 32;
        scancode += port_byte_in(REG_KEY_DAT) << 24;
        scancode += port_byte_in(REG_KEY_DAT) << 16;
        scancode += port_byte_in(REG_KEY_DAT) << 8;
        scancode += port_byte_in(REG_KEY_DAT);
    }

    u8 released = 0;
    released = (scancode >> 7) & 1;
    if (released)
    {
        scancode -= 0x80;
        if (scancode > 0xFFFF && scancode <= 0xFFFFFFFF)
            scancode -= 0x800000;
    }

    keycode = 0;
    switch (scancode)
    {
    case SC_ESCAPE:
        keycode = 0;
        goto row1;
    case SC_F1:
    case SC_F2:
    case SC_F3:
    case SC_F4:
    case SC_F5:
    case SC_F6:
    case SC_F8:
    case SC_F9:
    case SC_F10:
        keycode = 1 + scancode-SC_F1;
        goto row1;
    case SC_F11:
        keycode = 11;
        goto row1;
    case SC_F12:
        keycode = 12;
        goto row1;
    case SC_PRINTSCREEN:
        keycode = 15;
        goto row1;
    case SC_SCROLLLOCK:
        keycode = 16;
        goto row1;
    case SC_PAUSE:
        keycode = 17;
        goto row1;
        
    case SC_GRAVE:
        keycode = 0;
        goto row2;
    case SC_1:
    case SC_2:
    case SC_3:
    case SC_4:
    case SC_5:
    case SC_6:
    case SC_7:
    case SC_8:
    case SC_9:
        keycode = 1 + scancode-SC_1;
        goto row2;
    case SC_0:
    case SC_MINUS:
    case SC_EQUALS:
    case SC_BACKSPACE:
        keycode = 10 + scancode-SC_0;
        goto row2;
    case SC_INSERT:
        keycode = 15;
        goto row2;
    case SC_HOME:
        keycode = 16;
        goto row2;
    case SC_PAGEUP:
        keycode = 17;
        goto row2;
    case SC_NUMLOCK:
        keycode = 18;
        goto row2;
    case SC_NUMPAD_DIVIDE:
        keycode = 19;
        goto row2;
    case SC_NUMPAD_MULTIPLY:
        keycode = 20;
        goto row2;
    case SC_NUMPAD_MINUS:
        keycode = 21;
        goto row2;
        
    case SC_TAB:
    case SC_Q:
    case SC_W:
    case SC_E:
    case SC_R:
    case SC_T:
    case SC_Y:
    case SC_U:
    case SC_I:
    case SC_O:
    case SC_P:
    case SC_BRACKETLEFT:
    case SC_BRACKETRIGHT:
    case SC_ENTER:
        keycode = scancode-SC_TAB;
        goto row3;
    case SC_DELETE:
        keycode = 15;
        goto row3;
    case SC_END:
        keycode = 16;
        goto row3;
    case SC_PAGEDOWN:
        keycode = 17;
        goto row3;
    case SC_NUMPAD_7:
    case SC_NUMPAD_8:
    case SC_NUMPAD_9:
        keycode = 18 + scancode-SC_NUMPAD_7;
        goto row3;
    case SC_NUMPAD_PLUS:
        keycode = 21;
        goto row3;
        
    case SC_CAPSLOCK:
        keycode = 0;
        goto row4;
    case SC_A:
    case SC_S:
    case SC_D:
    case SC_F:
    case SC_G:
    case SC_H:
    case SC_J:
    case SC_K:
    case SC_L:
    case SC_SEMICOLON:
    case SC_APOSTROPHE:
        keycode = 1 + scancode-SC_A;
        goto row4;
    case SC_NUMPAD_4:
    case SC_NUMPAD_5:
    case SC_NUMPAD_6:
        keycode = 18 + scancode-SC_NUMPAD_4;
        goto row4;

    case SC_SHIFTLEFT:
    case SC_BACKSLASH:
    case SC_Z:
    case SC_X:
    case SC_C:
    case SC_V:
    case SC_B:
    case SC_N:
    case SC_M:
    case SC_COMMA:
    case SC_PERIOD:
    case SC_SLASH:
    case SC_SHIFTRIGHT:
        keycode = scancode-SC_SHIFTLEFT;
        goto row5;
    case SC_ARROWUP:
        keycode = 16;
        goto row5;
    case SC_NUMPAD_1:
    case SC_NUMPAD_2:
    case SC_NUMPAD_3:
        keycode = 18 + scancode-SC_NUMPAD_1;
        goto row5;
    case SC_NUMPAD_ENTER:
        keycode = 21;
        goto row5;

    case SC_CONTROLLEFT:
        keycode = 0;
        goto row6;
    case SC_METALEFT:
        keycode = 1;
        goto row6;
    case SC_ALTLEFT:
        keycode = 2;
        goto row6;
    case SC_SPACE:
        keycode = 3;
        goto row6;
    case SC_ALTRIGHT:
        keycode = 4;
        goto row6;
    case SC_METARIGHT:
        keycode = 5;
        goto row6;
    case SC_APPLICATION:
        keycode = 6;
        goto row6;
    case SC_CONTROLRIGHT:
        keycode = 7;
        goto row6;
    case SC_ARROWLEFT:
        keycode = 15;
        goto row6;
    case SC_ARROWDOWN:
        keycode = 16;
        goto row6;
    case SC_ARROWRIGHT:
        keycode = 17;
        goto row6;
    case SC_NUMPAD_0:
        keycode = 18;
        goto row6;
    case SC_NUMPAD_PERIOD:
        keycode = 20;
        goto row6;
    default: {
        char sc_hex[16];
        hex_to_ascii(scancode, sc_hex);
        kprint("ERROR: UNRECOGNIZED SCANCODE (0x");
        kprint(sc_hex);
        kprint(")\n");
        return;
    }
    }

 row6: keycode += 1 << 5;
 row5: keycode += 1 << 5;
 row4: keycode += 1 << 5;
 row3: keycode += 1 << 5;
 row2: keycode += 1 << 5;
 row1: keycode += 1 << 5;
    
    if (scancode == SC_SHIFTLEFT)
    {
        STATE.lshift = !released;
    }
    else if (scancode == SC_SHIFTRIGHT)
    {
        STATE.rshift = !released;
    }
    else if (scancode == SC_CONTROLLEFT)
    {
        STATE.lctrl = !released;
    }
    else if (scancode == SC_CONTROLRIGHT)
    {
        STATE.rctrl = !released;
    }
    else if (scancode == SC_ALTLEFT)
    {
        STATE.lalt = !released;
    }
    else if (scancode == SC_ALTRIGHT)
    {
        STATE.ralt = !released;
    }
    else
    {
        char ascii;
        if (STATE.lshift || STATE.rshift)
            ascii = sc_ascii_shifted[scancode];
        else
            ascii = sc_ascii[scancode];
        send_key_event(keycode, released, ascii);
    }
}

void init_keyboard()
{
    register_interrupt_handler(IRQ1, &keyboard_callback);
}

void register_keyboard_hook(Keyboard_Hook *hook)
{
    int i = 0;
    while (key_hooks[i] != 0) i++;
    key_hooks[i] = hook;
}

void unregister_keyboard_hook(Keyboard_Hook *hook)
{
    for (u16 i = 0; i < sizeof(key_hooks); i++)
        if (key_hooks[i] == hook) key_hooks[i] = 0;
}
