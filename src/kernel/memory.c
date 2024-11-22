#include "memory.h"
#include "debug.h"
#include "file.h"
#include "lib.h"
#include "print.h"
#include "process.h"
#include "stdbool.h"
#include "stddef.h"

static struct Page free_memory;
extern char end;
void load_pgd(uint64_t map);

static void free_region(uint64_t v, uint64_t e)
{
    for (uint64_t start = PA_UP(v); start + PAGE_SIZE <= e; start += PAGE_SIZE)
    {
        if (start + PAGE_SIZE <= MEMORY_END)
        {
            kfree(start);
        }
    }
}

void kfree(uint64_t v)
{
    ASSERT(v % PAGE_SIZE == 0);
    ASSERT(v >= (uint64_t)&end);
    ASSERT(v + PAGE_SIZE <= MEMORY_END);

    struct Page *page_address = (struct Page *)v;
    page_address->next = free_memory.next;
    free_memory.next = page_address;
}

void *kalloc(void)
{
    struct Page *page_address = free_memory.next;

    if (page_address != NULL)
    {
        ASSERT((uint64_t)page_address % PAGE_SIZE == 0);
        ASSERT((uint64_t)page_address >= (uint64_t)&end);
        ASSERT((uint64_t)page_address + PAGE_SIZE <= MEMORY_END);

        free_memory.next = page_address->next;
    }

    return page_address;
}

void checkmm(void)
{
    struct Page *v = free_memory.next;
    uint64_t size = 0;
    uint64_t i = 0;

    while (v != NULL)
    {
        size += PAGE_SIZE;
        printk("%d base is %x \r\n", i++, v);
        v = v->next;
    }

    printk("memory size is %u \r\n", size / 1024 / 1024);
}

static uint64_t *find_pgd_entry(uint64_t map, uint64_t v, int alloc, uint64_t attribute)
{
    uint64_t *ptr = (uint64_t *)map;
    void *addr = NULL;
    unsigned int index = (v >> 39) & 0x1ff;

    if (ptr[index] & ENTRY_V)
    {
        addr = (void *)P2V(PDE_ADDR(ptr[index]));
    }
    else if (alloc == 1)
    {
        addr = kalloc();
        if (addr != NULL)
        {
            memset(addr, 0, PAGE_SIZE);
            ptr[index] = (V2P(addr) | attribute | TABLE_ENTRY);
        }
    }

    return addr;
}

static uint64_t *find_pud_entry(uint64_t map, uint64_t v, int alloc, uint64_t attribute)
{
    uint64_t *ptr = NULL;
    void *addr = NULL;
    unsigned int index = (v >> 30) & 0x1ff;

    ptr = find_pgd_entry(map, v, alloc, attribute);
    if (ptr == NULL)
    {
        return NULL;
    }

    if (ptr[index] & ENTRY_V)
    {
        addr = (void *)P2V(PDE_ADDR(ptr[index]));
    }
    else if (alloc == 1)
    {
        addr = kalloc();
        if (addr != NULL)
        {
            memset(addr, 0, PAGE_SIZE);
            ptr[index] = (V2P(addr) | attribute | TABLE_ENTRY);
        }
    }

    return addr;
}

bool map_page(uint64_t map, uint64_t v, uint64_t pa, uint64_t attribute)
{
    uint64_t vstart = PA_DOWN(v);
    uint64_t *ptr = NULL;

    ASSERT(vstart + PAGE_SIZE < MEMORY_END);
    ASSERT(pa % PAGE_SIZE == 0);
    ASSERT(pa + PAGE_SIZE <= V2P(MEMORY_END));

    ptr = find_pud_entry(map, vstart, 1, attribute);
    if (ptr == NULL)
    {
        return false;
    }

    unsigned int index = (vstart >> 21) & 0x1ff;
    ASSERT((ptr[index] & ENTRY_V) == 0);

    ptr[index] = (pa | attribute | BLOCK_ENTRY);

    return true;
}

void free_page(uint64_t map, uint64_t vstart)
{
    unsigned int index;
    uint64_t *ptr = NULL;

    ASSERT(vstart % PAGE_SIZE == 0);

    ptr = find_pud_entry(map, vstart, 0, 0);

    if (ptr != NULL)
    {
        index = (vstart >> 21) & 0x1ff;

        if (ptr[index] & ENTRY_V)
        {
            kfree(P2V(PTE_ADDR(ptr[index])));
            ptr[index] = 0;
        }
    }
}

static void free_pmd(uint64_t map)
{
    uint64_t *pgd = (uint64_t *)map;
    uint64_t *pud = NULL;

    for (int i = 0; i < 512; i++)
    {
        if (pgd[i] & ENTRY_V)
        {
            pud = (uint64_t *)P2V(PDE_ADDR(pgd[i]));

            for (int j = 0; j < 512; j++)
            {
                if (pud[j] & ENTRY_V)
                {
                    kfree(P2V(PDE_ADDR(pud[j])));
                    pud[j] = 0;
                }
            }
        }
    }
}

static void free_pud(uint64_t map)
{
    uint64_t *ptr = (uint64_t *)map;

    for (int i = 0; i < 512; i++)
    {
        if (ptr[i] & ENTRY_V)
        {
            kfree(P2V(PDE_ADDR(ptr[i])));
            ptr[i] = 0;
        }
    }
}

static void free_pgd(uint64_t map)
{
    kfree(map);
}

void free_vm(uint64_t map)
{
    free_page(map, 0x400000);
    free_pmd(map);
    free_pud(map);
    free_pgd(map);
}

bool setup_uvm(struct Process *process, char *file_name)
{
    bool status = false;
    uint64_t map = process->page_map;
    void *page = kalloc();

    if (page != NULL)
    {
        memset(page, 0, PAGE_SIZE);
        status = map_page(map, 0x400000, V2P(page), ENTRY_V | USER | NORMAL_MEMORY | ENTRY_ACCESSED);

        if (status == true)
        {
            int fd = open_file(process, file_name);
            if (fd == -1)
            {
                free_vm(map);
                return false;
            }
            uint32_t size = get_file_size(process, fd);
            if (read_file(process, fd, page, size) != size)
            {
                free_vm(map);
                return false;
            }

            close_file(process, fd);
        }
        else
        {
            kfree((uint64_t)page);
            free_vm(map);
        }
    }

    return status;
}

void switch_vm(uint64_t map)
{
    load_pgd(V2P(map));
}

void init_memory(void)
{
    free_region((uint64_t)&end, MEMORY_END);
    // checkmm();
}