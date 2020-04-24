#include "screen.h"

#include "../cpu/port.h"
#include "../cpu/types.h"

#include "../libc/mem.h"
#include "../libc/string.h"

// Private Procedure Declarations
int get_cursor_offset();
void set_cursor_offset(int offset);
int print_char(char c, int col, int row, char attr);
int get_offset(int col, int row);
int get_offset_row(int offset);
int get_offset_col(int offset);

// Public API

/**
 * Print a message on the specified location
 * If col or row are negative, use the current offset
 */
void kprint_at(char *message, int col, int row)
{
    int offset = get_cursor_offset();
    if (col < 0)
        col = get_offset_col(offset);
    if (row < 0)
        row = get_offset_row(offset);
    offset = get_offset(col, row);
    
    int i = 0;
    while (message[i])
    {
        offset = print_char(message[i++], col, row, WHITE_ON_BLACK);
        row = get_offset_row(offset);
        col = get_offset_col(offset);
    }
    char hex[16];
    hex_to_ascii(i, hex);
}

void kprint(char *message)
{
    kprint_at(message, -1, -1);
}

void kprint_hex(u32 hex)
{
    char str[9];
    hex_to_ascii(hex, str);
    kprint("0x"); kprint(str);
}

void kprint_backspace()
{
    int col, row;
    int offset = get_cursor_offset()-2;
    col = get_offset_col(offset);
    row = get_offset_row(offset);
    
    print_char(0x08, col, row, WHITE_ON_BLACK);
}

void clear_screen()
{
    int screen_size = MAX_COLS * MAX_ROWS;
    int i;
    char *screen = VIDEO_ADDRESS;
    
    for (i = 0; i < screen_size; i++)
    {
        screen[i*2] = ' ';
        screen[i*2+1] = WHITE_ON_BLACK;
    }
    set_cursor_offset(get_offset(0, 0));
}

// Private Procedures

typedef struct Screen_Char
{
    byte symbol;
    byte color;
} Screen_Char;
typedef Screen_Char Screen[MAX_ROWS][MAX_COLS];

int print_char(char c, int col, int row, char attr)
{
    u8 *vidmem = (u8*) VIDEO_ADDRESS;
    Screen *screen = (Screen*)vidmem;
    
    if (!attr) attr = WHITE_ON_BLACK;
    
    /* Error control: print a red 'E' if the coords aren't right */
    if (col >= MAX_COLS || row >= MAX_ROWS)
    {
        (*screen)[MAX_ROWS-1][MAX_COLS-1] = (Screen_Char){'E', RED_ON_WHITE};
        
        return get_offset(col, row);
    }
    
    int offset = get_cursor_offset();
    if (col < 0)
        col = get_offset_col(offset);
    if (row < 0)
        row = get_offset_row(offset);
    offset = get_offset(col, row);
    
    if (c == '\n')
    {
        row = get_offset_row(offset);
        offset = get_offset(0, row+1);
    }
    else if (c == 0x08) // BACKSPACE
    {
        (*screen)[row][col] = (Screen_Char){' ', attr};
    }
    else
    {
        (*screen)[row][col] = (Screen_Char){c, attr};
        offset+=2;
    }
    if (offset >= MAX_ROWS * MAX_COLS * 2)
    {
        memory_copy(&(*screen)[0][0], &(*screen)[1][0], sizeof(*screen)-sizeof((*screen)[0]));
        Screen_Char *last_line = &(*screen)[MAX_ROWS-1][0];
        for (int i = 0; i < MAX_COLS; i++) last_line[i] = (Screen_Char){0};
        
        offset -= 2 * MAX_COLS;
    }
    set_cursor_offset(offset);
    
    return offset;
}

int get_cursor_offset()
{
    port_byte_out(REG_SCR_CTL, 14);
    int offset = port_byte_in(REG_SCR_DAT) << 8;
    port_byte_out(REG_SCR_CTL, 15);
    offset += port_byte_in(REG_SCR_DAT);
    
    return offset*2;
}

void set_cursor_offset(int offset)
{
    offset /= 2;
    port_byte_out(REG_SCR_CTL, 14);
    port_byte_out(REG_SCR_DAT, (u8)(offset >> 8));
    port_byte_out(REG_SCR_CTL, 15);
    port_byte_out(REG_SCR_DAT, (u8)(offset & 0xff));
}

int get_offset(int col, int row) { return 2 * (row * MAX_COLS + col); }
int get_offset_row(int offset) { return offset / (2 * MAX_COLS); }
int get_offset_col(int offset) { return (offset - (get_offset_row(offset)*2*MAX_COLS))/2; }
