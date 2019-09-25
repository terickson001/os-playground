#ifndef _KERNEL_SCREEN_H_
#define _KERNEL_SCREEN_H_

#define VIDEO_ADDRESS ((char*)0xb8000)

#define MAX_ROWS (25)
#define MAX_COLS (80)

#define WHITE_ON_BLACK (0x0f)
#define RED_ON_WHITE (0xf4)

/* Screen i/o ports */
#define REG_SCR_CTL (0x3d4)
#define REG_SCR_DAT (0x3d5)

/* Public kernel API */
void kprint_at(char *message, int col, int row);
void kprint(char *message);
void clear_screen();

#endif
