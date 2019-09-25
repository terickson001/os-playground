#ifndef _KERNEL_STRING_H_
#define _KERNEL_STRING_H_

#include "../cpu/types.h"

i32 strlen(char *str);
void reverse(char *str);
void int_to_ascii(int n, char *str);

#endif
