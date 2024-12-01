#ifndef _LIB_H_
#define _LIB_H_

#include "stdint.h"

struct DirEntry
{
    uint8_t name[8];
    uint8_t ext[3];
    uint8_t attributes;
    uint8_t reserved;
    uint8_t create_ms;
    uint16_t create_time;
    uint16_t create_date;
    uint16_t access_date;
    uint16_t attr_index;
    uint16_t m_time;
    uint16_t m_date;
    uint16_t cluster_index;
    uint32_t file_size;
} __attribute__((packed));

#define ENTRY_EMPTY 0
#define ENTRY_DELETED 0xe5

int printf(const char *format, ...);
int writeu(char *buffer, int buffer_size);
void sleepu(uint64_t ticks);
int exitu(void);
int waitu(uint64_t pid);
int open_file(char *name);
void close_file(int fd);
int get_file_size(int fd);
int read_file(int fd, void *buffer, uint32_t size);
int fork(void);
int exec(char *file_name);
char keyboard_read(void);
int read_root_directory(void *buffer);

void presentation_show(void);
void presentation_close(void);
void presentation_next(void);
void presentation_prev(void);

void memset(void *dst, int value, unsigned int size);
void memcpy(void *dst, void *src, unsigned int size);
void memmove(void *dst, void *src, unsigned int size);
int memcmp(void *src1, void *src2, unsigned int size);

#endif