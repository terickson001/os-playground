#ifndef DRIVERS_SERIAL_H
#define DRIVERS_SERIAL_H

#include <cpu/port.h>

void serial_write(char data);
void serial_write_str(char *str);

#endif // DRIVERS_SERIAL_H
