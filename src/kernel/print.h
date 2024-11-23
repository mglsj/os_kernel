#ifndef _PRINT_H
#define _PRINT_H

#include "stdint.h"

int printk(const char *format, ...);
int sprintk(char *out_buffer, const char *format, ...);
void write_console(const char *buffer, int size);

#endif