#include "string.h"

isize strlen(char *str)
{
    i32 len = 0;
    while (*str++) len++;
    return len;
}

isize strcmp(char *a, char *b)
{
    int i;
    for (i = 0; a[i] == b[i]; i++)
        if (!a[i]) return 0;
    return a[i] - b[i];
}

void strcpy(char *dest, char *src)
{
    while (*src)
        *(dest++) = *(src++);
    *dest = 0;
}

void strncpy(char *dest, char *src, u32 n)
{
    while (src[i] && i < n)
        dest[i] = src[i++];
    dest[i] = 0;
}

void reverse(char *str)
{
    int i, j;
    char c;
    for (i = 0, j = strlen(str)-1; i < j; i++, j--)
    {
        c = str[i];
        str[i] = str[j];
        str[j] = c;
    }
}

void int_to_ascii(int n, char *str)
{
    int i, sign;
    if ((sign = n) < 0) n  = -n;
    i = 0;
    do
    {
        str[i++] = n % 10 + '0';
    } while (n /= 10);
    
    if (sign < 0) str[i++] = '-';
    str[i] = 0;
    
    reverse(str);
}

void hex_to_ascii(u32 n, char *str)
{
    int i = 0;
    u8 digit;
    do
    {
        digit = n % 16;
        if (digit <= 9)
            str[i++] = '0' + digit;
        else
            str[i++] = '7' + digit;
    } while (n /= 16);
    
    str[i] = 0;
    reverse(str);
}
