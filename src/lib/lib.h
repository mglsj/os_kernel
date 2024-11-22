#ifndef _LIB_H
#define _LIB_H

#include "stdint.h"

int printf(const char *format, ...);
int writeu(char *buffer, int buffer_size);
int sleepu(uint64_t ticks);
int exitu(void);
int waitu(uint64_t pid);
int open_file(char *name);
void close_file(int fd);
int get_file_size(int fd);
int read_file(int fd, void *buffer, uint32_t size);

#endif