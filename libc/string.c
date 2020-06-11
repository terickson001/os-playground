#include <libc/string.h>
#include <libc/mem.h>
#include <drivers/serial.h>

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

/* Why */
void strncpy(char *dest, char *src, usize n)
{
    usize i;
    for (i = 0; (dest[i] = src[i]) && i < n; i++);
    for (; i < n; dest[i++] = 0);
}

void str_reverse(char *str)
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


static char DIGIT_CHAR[] = "0123456789" "ABCDEFG";
void i32_to_ascii(i32 n, char *str, u32 base)
{
    int sign;
    if ((sign = n) < 0) n  = -n;
    int i = 0;
    do
    {
        str[i++] = DIGIT_CHAR[n % base];
    } while (n /= base);
    
    if (sign < 0) str[i++] = '-';
    str[i] = 0;
    
    str_reverse(str);
}

void u32_to_ascii(u32 n, char *str, u32 base)
{
    int i = 0;
    do
    {
        str[i++] = DIGIT_CHAR[n % base];
    } while (n /= base);
    str[i] = 0;
    
    str_reverse(str);
}


void int_to_ascii(int n, char *str)
{
    i32_to_ascii(n, str, 10);
}

void hex_to_ascii(u32 n, char *str)
{
    u32_to_ascii(n, str, 16);
}

char to_upper(char c)
{
    if ('a' <= c && c <= 'z') return c-32;
    return c;
}

char to_lower(char c)
{
    if ('A' <= c && c <= 'Z') return c+32;
    return c;
}

typedef enum Integer_Size
{
    IntegerSize_Int,
    IntegerSize_Char,
    IntegerSize_Short,
    IntegerSize_Long,
    IntegerSize_LongLong,
    IntegerSize_Size,
} Integer_Size;

typedef struct Fmt_Info
{
    Integer_Size integer_size;
    u8 base;
    b8 is_unsigned;
    b8 lower_case;
} Fmt_Info;


int snprintf(char *buf, int buf_size, char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    int ret = snprintf_va(buf, buf_size, fmt, va);
    va_end(va);
    return ret;
}

int snprintf_va(char *buf, int buf_size, char *fmt, va_list va)
{
    Fmt_Info info = {0};
    
    int fidx = 0;
    int bidx = 0;
    
    int written;
    
    while (fmt[fidx])
    {
        while (fmt[fidx] != '%' && bidx < buf_size-1)
            buf[bidx++] = fmt[fidx++];
        
        if (fmt[fidx] == '%')
        {
            fidx++;
            
            switch (fmt[fidx])
            {
                case 'h':
                fidx++;
                if (fmt[fidx] == 'h')
                {
                    info.integer_size = IntegerSize_Char;
                    fidx++;
                }
                else
                {
                    info.integer_size = IntegerSize_Short;
                }
                break;
                
                case 'l':
                fidx++;
                if (fmt[fidx] == 'l')
                {
                    info.integer_size = IntegerSize_LongLong;
                    fidx++;
                }
                else
                {
                    info.integer_size = IntegerSize_Long;
                }
                break;
                
                case 'z':
                info.is_unsigned = true;
                // fallthrough
                case 't':
                fidx++;
                info.integer_size = IntegerSize_Size;
                break;
            }
            
            switch (fmt[fidx])
            {
                case 'u':
                info.is_unsigned = true;
                // fallthrough
                case 'd':
                case 'i':
                info.base = 10;
                fidx++;
                break;
                
                case 'o':
                info.base = 8;
                fidx++;
                break;
                
                case 'x':
                info.base = 16;
                info.is_unsigned = true;
                info.lower_case = true;
                fidx++;
                break;
                
                case 'X':
                info.base = 16;
                info.is_unsigned = true;
                info.lower_case = false;
                fidx++;
                break;
                
                /*
                                case 'f':
                                case 'F':
                                case 'g':
                                case 'G':
                */
                
                case 'c':
                buf[bidx++] = (char)va_arg(va, int);
                written = 1;
                fidx++;
                break;
                
                case 's':
                {
                    char *str = va_arg(va, char *);
                    int start = bidx;
                    while (*str && bidx < buf_size-1)
                        buf[bidx++] = *str++;
                    written = bidx - start;
                    fidx++;
                } break;
                
                case '%':
                buf[bidx++] = '%';
                written = 1;
                fidx++;
                break;
            }
            
            if (info.base)
            {
                if (info.is_unsigned)
                {
                    u32 value = 0;
                    switch (info.integer_size)
                    {
                        case IntegerSize_Char:     value = (u32)(char) va_arg(va, unsigned int); break;
                        case IntegerSize_Short:    value = (u32)(short)va_arg(va, unsigned int); break;
                        case IntegerSize_Long:     value = (u32)va_arg(va, unsigned long);       break;
                        case IntegerSize_LongLong: value = (u32)va_arg(va, unsigned long long);  break;
                        case IntegerSize_Size:     value = (u32)va_arg(va, usize);               break;
                        default:                   value = (u32)va_arg(va, unsigned int);        break;
                    }
                    
                    char num_buf[256];
                    u32_to_ascii(value, num_buf, info.base);
                    int start = bidx;
                    int nidx = 0;
                    while (num_buf[nidx] && bidx < buf_size-1)
                    {
                        if (info.lower_case) buf[bidx++] = to_lower(num_buf[nidx++]);
                        else                 buf[bidx++] = to_upper(num_buf[nidx++]);
                    }
                    written = start - bidx;
                }
                else
                {
                    i32 value = 0;
                    switch (info.integer_size)
                    {
                        case IntegerSize_Char:     value = (i32)(char) va_arg(va, int); break;
                        case IntegerSize_Short:    value = (i32)(short)va_arg(va, int); break;
                        case IntegerSize_Long:     value = (i32)va_arg(va, long);       break;
                        case IntegerSize_LongLong: value = (i32)va_arg(va, long long);  break;
                        case IntegerSize_Size:     value = (i32)va_arg(va, isize);      break;
                        default:                   value = (i32)va_arg(va, int);        break;
                    }
                    
                    char num_buf[256];
                    i32_to_ascii(value, num_buf, info.base);
                    int start = bidx;
                    int nidx = 0;
                    while (num_buf[nidx] && bidx < buf_size-1)
                        buf[bidx++] = num_buf[nidx++];
                    written = start - bidx;
                }
            }
        }
        (void)(written);
        if (bidx == buf_size-1)
            break;
    }
    
    buf[bidx] = 0;
    return bidx;
}