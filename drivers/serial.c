#include <drivers/serial.h>
void serial_write(char data)
{
    while((port_byte_in(0x3f8 + 5) & 0x20) == 0);
    port_byte_out(0x3f8, data);
}

void serial_write_str(char *str)
{
    while (*str)
        serial_write(*(str++));
}