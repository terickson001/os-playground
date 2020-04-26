#ifndef _KERNEL_STRING_H_
#define _KERNEL_STRING_H_

#include <cpu/types.h>

i32 strlen(char *str);
i32 strcmp(char *a, char *b);
void strcpy(char *dest, char *src);
void strncpy(char *dest, char *src, usize n);
void reverse(char *str);
void int_to_ascii(int n, char *str);
void hex_to_ascii(u32 n, char *str);

#endif
