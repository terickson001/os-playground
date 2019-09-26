#include "screen.h"

#include "../cpu/port.h"
#include "../cpu/types.h"

#include "../libc/mem.h"

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
}

void kprint(char *message)
{
    kprint_at(message, -1, -1);
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

int print_char(char c, int col, int row, char attr)
{
    u8 *vidmem = (u8*) VIDEO_ADDRESS;
    if (!attr) attr = WHITE_ON_BLACK;

    /* Error control: print a red 'E' if the coords aren't right */
    if (col >= MAX_COLS || row >= MAX_ROWS)
    {
        vidmem[2*(MAX_COLS)*(MAX_ROWS)-2] = 'E';
        vidmem[2*(MAX_COLS)*(MAX_ROWS)-1] = RED_ON_WHITE;
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
        vidmem[offset] = ' ';
        vidmem[offset+1] = attr;
    }
    else
    {
        vidmem[offset] = c;
        vidmem[offset+1] = attr;
        offset+=2;
    }
    if (offset >= MAX_ROWS * MAX_COLS * 2)
    {
        memory_copy(vidmem, vidmem+(MAX_COLS*2), (MAX_COLS*(MAX_ROWS-1)*2)); // Scroll text
        u8 *last_line = vidmem + get_offset(0, MAX_ROWS-1);
        for (int i = 0; i < MAX_COLS*2; i++) last_line[i] = 0;
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
