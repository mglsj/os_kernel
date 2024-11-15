#include "syscall.h"
#include "debug.h"
#include "handler.h"
#include "print.h"
#include "process.h"
#include "stddef.h"

static SYSTEMCALL system_calls[10];

static int sys_write(int64_t *argptr)
{
    write_console((char *)argptr[0], (int)argptr[1]);
    return (int)argptr[1];
}

static int sys_sleep(int64_t *argptr)
{
    uint64_t ticks;
    uint64_t old_tikcs;
    uint64_t sleep_ticks = argptr[0];

    ticks = get_ticks();
    old_tikcs = ticks;

    while (ticks - old_tikcs < sleep_ticks)
    {
        sleep(-1);
        ticks = get_ticks();
    }

    return 0;
}

static int sys_exit(int64_t *argptr)
{
    exit();
    return 0;
}

static int sys_wait(int64_t *argptr)
{
    wait(argptr[0]);
    return 0;
}

void system_call(struct TrapFrame *tf)
{
    int64_t i = tf->x8;
    int64_t pram_count = tf->x0;
    int64_t *argptr = (int64_t *)tf->x1;

    if (pram_count < 0 || i < 0 || i > 3)
    {
        tf->x0 = -1;
        return;
    }

    tf->x0 = system_calls[i](argptr);
}

void init_system_call(void)
{
    system_calls[0] = sys_write;
    system_calls[1] = sys_sleep;
    system_calls[2] = sys_exit;
    system_calls[3] = sys_wait;
}
