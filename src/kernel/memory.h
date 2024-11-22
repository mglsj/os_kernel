#ifndef _MEMORY_H
#define _MEMORY_H

#include "process.h"
#include "stdbool.h"
#include "stdint.h"

struct Page
{
    struct Page *next;
};

#define KERNEL_BASE 0xffff000000000000
#define P2V(p) ((uint64_t)(p) + KERNEL_BASE)
#define V2P(v) ((uint64_t)(v) - KERNEL_BASE)

#define MEMORY_END P2V(0x30000000)
#define PAGE_SIZE (2 * 1024 * 1024)

#define PA_UP(v) ((((uint64_t)v + PAGE_SIZE - 1) >> 21) << 21)
#define PA_DOWN(v) (((uint64_t)v >> 21) << 21)

#define PDE_ADDR(p) ((uint64_t)p & 0xfffffffff000)
#define PTE_ADDR(p) ((uint64_t)p & 0xffffffe00000)

#define ENTRY_V (1 << 0)
#define TABLE_ENTRY (1 << 1)
#define BLOCK_ENTRY (0 << 1)
#define ENTRY_ACCESSED (1 << 10)
#define NORMAL_MEMORY (1 << 2)
#define DEVICE_MEMORY (0 << 2)
#define USER (1 << 6)

void *kalloc(void);
void kfree(uint64_t v);
void init_memory(void);
bool map_page(uint64_t map, uint64_t v, uint64_t pa, uint64_t attribute);
void switch_vm(uint64_t map);
bool setup_uvm(struct Process *process, char *file_name);
void free_page(uint64_t map, uint64_t vstart);
void free_vm(uint64_t map);
uint64_t read_pgd(void);
bool copy_uvm(uint64_t dst_map, uint64_t src_map, int size);

#endif
