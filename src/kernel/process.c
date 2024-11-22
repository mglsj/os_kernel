#include "process.h"
#include "debug.h"
#include "memory.h"
#include "stddef.h"

static struct Process process_table[NUM_PROC];
static int pid_num = 1;
static struct ProcessControl pc;
void pstart(struct TrapFrame *tf);

struct ProcessControl *get_pc(void)
{
    return &pc;
}

static struct Process *find_unused_process(void)
{
    struct Process *process = NULL;

    for (int i = 0; i < NUM_PROC; i++)
    {
        if (process_table[i].state == PROC_UNUSED)
        {
            process = &process_table[i];
            break;
        }
    }

    return process;
}

static void init_idle_process(void)
{
    struct Process *process;
    struct ProcessControl *process_control;

    process = find_unused_process();
    ASSERT(process == &process_table[0]);

    process->state = PROC_RUNNING;
    process->pid = 0;
    process->page_map = P2V(read_pgd());

    process_control = get_pc();
    process_control->current_process = process;
}

static struct Process *alloc_new_process(void)
{
    struct Process *process;

    process = find_unused_process();
    if (process == NULL)
    {
        return NULL;
    }

    process->stack = (uint64_t)kalloc();
    ASSERT(process->stack != 0);
    memset((void *)process->stack, 0, PAGE_SIZE);

    process->state = PROC_INIT;
    process->pid = pid_num++;

    process->context = process->stack + PAGE_SIZE - sizeof(struct TrapFrame) - 12 * 8;
    *(uint64_t *)(process->context + 11 * 8) = (uint64_t)trap_return;
    process->tf = (struct TrapFrame *)(process->stack + PAGE_SIZE - sizeof(struct TrapFrame));
    process->tf->elr = 0x400000;
    process->tf->sp0 = 0x400000 + PAGE_SIZE;
    process->tf->spsr = 0;

    process->page_map = (uint64_t)kalloc();
    ASSERT(process->page_map != 0);
    memset((void *)process->page_map, 0, PAGE_SIZE);

    return process;
}

static void init_user_process(void)
{
    struct Process *process;
    struct ProcessControl *process_control;
    struct HeadList *list;

    process = alloc_new_process();
    ASSERT(process != NULL);

    ASSERT(setup_uvm(process, "INIT.BIN"));

    process_control = get_pc();
    list = &process_control->ready_list;

    process->state = PROC_READY;
    append_list_tail(list, (struct List *)process);
}

void init_process(void)
{
    init_idle_process();
    init_user_process();
}

static void switch_process(struct Process *prev, struct Process *current)
{
    switch_vm(current->page_map);
    swap(&prev->context, current->context);
}

static void schedule(void)
{
    struct Process *prev_proc;
    struct Process *current_proc;
    struct ProcessControl *process_control;
    struct HeadList *list;

    process_control = get_pc();
    list = &process_control->ready_list;
    prev_proc = process_control->current_process;

    if (is_list_empty(list))
    {
        current_proc = &process_table[0];
    }
    else
    {
        current_proc = (struct Process *)remove_list_head(list);
    }

    current_proc->state = PROC_RUNNING;
    process_control->current_process = current_proc;

    switch_process(prev_proc, current_proc);
}

void yield(void)
{
    struct Process *process;
    struct ProcessControl *process_control;
    struct HeadList *list;

    process_control = get_pc();
    list = &process_control->ready_list;

    if (is_list_empty(list))
    {
        return;
    }

    process = process_control->current_process;
    process->state = PROC_READY;

    if (process->pid != 0)
    {
        append_list_tail(list, (struct List *)process);
    }

    schedule();
}

void sleep(int wait)
{
    struct Process *process;
    struct ProcessControl *process_control;

    process_control = get_pc();
    process = process_control->current_process;
    process->state = PROC_SLEEP;
    process->wait = wait;

    append_list_tail(&process_control->wait_list, (struct List *)process);
    schedule();
}

void wake_up(int wait)
{
    struct Process *process;
    struct ProcessControl *process_control;
    struct HeadList *ready_list;
    struct HeadList *wait_list;

    process_control = get_pc();
    ready_list = &process_control->ready_list;
    wait_list = &process_control->wait_list;

    process = (struct Process *)remove_list(wait_list, wait);

    while (process != NULL)
    {
        process->state = PROC_READY;
        append_list_tail(ready_list, (struct List *)process);
        process = (struct Process *)remove_list(wait_list, wait);
    }
}

void exit(void)
{
    struct Process *process;
    struct ProcessControl *process_control;

    process_control = get_pc();
    process = process_control->current_process;
    process->state = PROC_KILLED;
    process->wait = process->pid;

    append_list_tail(&process_control->kill_list, (struct List *)process);

    wake_up(-3);
    schedule();
}

void wait(int pid)
{
    struct Process *process;
    struct ProcessControl *process_control;
    struct HeadList *list;

    process_control = get_pc();
    list = &process_control->kill_list;

    while (1)
    {
        if (!is_list_empty(list))
        {
            process = (struct Process *)remove_list(list, pid);
            if (process != NULL)
            {
                ASSERT(process->state == PROC_KILLED);
                kfree(process->stack);
                free_vm(process->page_map);

                for (int i = 0; i < 100; i++)
                {
                    if (process->file[i] != NULL)
                    {
                        process->file[i]->fcb->count--;
                        process->file[i]->count--;

                        if (process->file[i]->count == 0)
                        {
                            process->file[i]->fcb = NULL;
                        }
                    }
                }

                memset(process, 0, sizeof(struct Process));
                break;
            }
        }

        sleep(-3);
    }
}

int fork(void)
{
    struct ProcessControl *process_control;
    struct Process *process;
    struct Process *current_process;
    struct HeadList *list;

    process_control = get_pc();
    current_process = process_control->current_process;
    list = &process_control->ready_list;

    process = alloc_new_process();
    if (process == NULL)
    {
        ASSERT(0);
        return -1;
    }

    if (copy_uvm(process->page_map, current_process->page_map, PAGE_SIZE) == false)
    {
        ASSERT(0);
        return -1;
    }

    memcpy(process->file, current_process->file, 100 * sizeof(struct FileDesc *));

    for (int i = 0; i < 100; i++)
    {
        if (process->file[i] != NULL)
        {
            process->file[i]->count++;
            process->file[i]->fcb->count++;
        }
    }

    memcpy(process->tf, current_process->tf, sizeof(struct TrapFrame));
    process->tf->x0 = 0;
    process->state = PROC_READY;
    append_list_tail(list, (struct List *)process);

    return process->pid;
}

int exec(struct Process *process, char *name)
{
    int fd;
    uint32_t size;

    fd = open_file(process, name);
    if (fd == -1)
    {
        exit();
    }

    memset((void *)0x400000, 0, PAGE_SIZE);
    size = get_file_size(process, fd);
    size = read_file(process, fd, (void *)0x400000, size);
    if (size == 0xffffffff)
    {
        exit();
    }

    close_file(process, fd);

    memset(process->tf, 0, sizeof(struct TrapFrame));
    process->tf->elr = 0x400000;
    process->tf->sp0 = 0x400000 + PAGE_SIZE;
    process->tf->spsr = 0;

    return 0;
}
