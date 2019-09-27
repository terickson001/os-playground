#ifndef _DRIVER_KEYBOARD_H_
#define _DRIVER_KEYBOARD_H_

#include "../cpu/types.h"

typedef struct Keyboard_State
{
    union {
        u8 mod_state;
        struct {
            u8 lshift : 1;
            u8 rshift : 1;
            u8 lctrl  : 1;
            u8 rctrl  : 1;
            u8 lalt   : 1;
            u8 ralt   : 1;
            u8 lmeta  : 1;
            u8 rmeta  : 1;
        };
    };
    u8 num_lock    : 1;
    u8 scroll_lock : 1;
    u8 caps_lock   : 1;
} Keyboard_State;

typedef struct Kbd_Event
{
    Keyboard_State state;
    
    u8 released : 1;
    u8 keycode;
    char ascii;
} Kbd_Event;

typedef void (Keyboard_Hook)(Kbd_Event event);

void init_keyboard();
void register_keyboard_hook(Keyboard_Hook *hook);
void unregister_keyboard_hook(Keyboard_Hook *hook);

#endif
