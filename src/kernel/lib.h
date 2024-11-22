#ifndef _LIB_H
#define _LIB_H

#include "stdbool.h"
#include "stdint.h"

struct List
{
	struct List *next;
};

struct HeadList
{
	struct List *next;
	struct List *tail;
};

void delay(uint64_t value);
void out_word(uint64_t addr, uint32_t value);
uint32_t in_word(uint64_t addr);

void memset(void *dst, int value, unsigned int size);
void memcpy(void *dst, void *src, unsigned int size);
void memmove(void *dst, void *src, unsigned int size);
int memcmp(void *src1, void *src2, unsigned int size);
unsigned char get_el(void);

void append_list_tail(struct HeadList *list, struct List *item);
struct List *remove_list_head(struct HeadList *list);
bool is_list_empty(struct HeadList *list);
struct List *remove_list(struct HeadList *list, int wait);

#endif