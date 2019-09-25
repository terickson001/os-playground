#include "string.h"

i32 strlen(char *str)
{
    i32 len = 0;
    while (*str++) len++;
    return len;
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
    } while ((n /= 10) > 0);

    if (sign < 0) str[i++] = '-';
    str[i] = 0;
    
    reverse(str);
}
