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

#define LCTRL     0x1D
#define RCTRL     0xE01D
#define LSHIFT    0x2A
#define RSHIFT    0x36
#define LALT      0x38
#define RALT      0xE038


static char key_buffer[256];
static char key_mods[6];
typedef enum MOD_KEY
{
    MOD_LSHIFT,
    MOD_RSHIFT,
    MOD_LCTRL,
    MOD_RCTRL,
    MOD_LALT,
    MOD_RALT,
} MOD_KEY;

enum Scancode {
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
    SC_PREIOD = 0x34,
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
    SC_ALT_PRINTSCREEN = 0x54, /* ALT + PRINT SCREEN. MAPVIRTUALKEYEX( VK_SNAPSHOT, MAPVK_VK_TO_VSC_EX, 0 ) RETURNS SCANCODE 0x54. */
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
    /*
    SC_PAUSE:
    - MAKE: 0xE11D 45 0xE19D C5
    - MAKE IN RAW INPUT: 0xE11D 0x45
    - BREAK: NONE
    - NO REPEAT WHEN YOU HOLD THE KEY DOWN
    - THERE ARE NO BREAK SO I DON'T KNOW HOW THE KEY DOWN/UP IS EXPECTED TO WORK. RAW INPUT SENDS "KEYDOWN" AND "KEYUP" MESSAGES, AND IT APPEARS THAT THE KEYUP MESSAGE IS SENT DIRECTLY AFTER THE KEYDOWN MESSAGE (YOU CAN'T HOLD THE KEY DOWN) SO DEPENDING ON WHEN GETMESSAGE OR PEEKMESSAGE WILL RETURN MESSAGES, YOU MAY GET BOTH A KEYDOWN AND KEYUP MESSAGE "AT THE SAME TIME". IF YOU USE VK MESSAGES MOST OF THE TIME YOU ONLY GET KEYDOWN MESSAGES, BUT SOME TIMES YOU GET KEYUP MESSAGES TOO.
    - WHEN PRESSED AT THE SAME TIME AS ONE OR BOTH CONTROL KEYS, GENERATES A 0xE046 (SC_CANCEL) AND THE STRING FOR THAT SCANCODE IS "BREAK".
    */
};

typedef enum Keycode
{
    
} Keycode;

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
    '7', '8', '9', '0', '-', '=', '?', '?', 'q', 'w', 'e', 'r', 't', 'y', 
    'u', 'i', 'o', 'p', '[', ']', '?', '?', 'a', 's', 'd', 'f', 'g', 
    'h', 'j', 'k', 'l', ';', '\'', '`', '?', '\\', 'z', 'x', 'c', 'v', 
    'b', 'n', 'm', ',', '.', '/', '?', '?', '?', ' '
};

static void keyboard_callback(Registers *regs)
{
    UNUSED(regs);
    
    u32 scancode = port_byte_in(0x60);
    if (scancode >= 0xE0)
    {
        scancode <<= 8;
        scancode += port_byte_in(0x60);
        char scancode_str[5];
        hex_to_ascii(scancode, scancode_str);
        kprint("FOUND SUBCODE 0x");
        kprint(scancode_str);
        kprint("\n");
    }
    
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
