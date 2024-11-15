#ifndef _LIB_H
#define _LIB_H

#include "stdint.h"

int printf(const char *format, ...);
int writeu(char *buffer, int buffer_size);
int sleepu(uint64_t ticks);
int exitu(void);
int waitu(uint64_t pid);

#endif