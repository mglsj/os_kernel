#include "syscall.h"
#include "debug.h"
#include "file.h"
#include "handler.h"
#include "keyboard.h"
#include "print.h"
#include "process.h"
#include "stddef.h"

static SYSTEMCALL system_calls[20];

static int sys_write(int64_t *argptr)
{
    write_console((char *)argptr[0], (int)argptr[1]);
    return (int)argptr[1];
}

static int sys_sleep(int64_t *argptr)
{
    uint64_t ticks;
    uint64_t old_ticks;
    uint64_t sleep_ticks = argptr[0];

    ticks = get_ticks();
    old_ticks = ticks;

    while (ticks - old_ticks < sleep_ticks)
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

static int sys_open_file(int64_t *argptr)
{
    struct ProcessControl *pc = get_pc();
    return open_file(pc->current_process, (char *)argptr[0]);
}

static int sys_close_file(int64_t *argptr)
{
    struct ProcessControl *pc = get_pc();
    close_file(pc->current_process, argptr[0]);

    return 0;
}

static int sys_get_file_size(int64_t *argptr)
{
    struct ProcessControl *pc = get_pc();
    return get_file_size(pc->current_process, argptr[0]);
}

static int sys_read_file(int64_t *argptr)
{
    struct ProcessControl *pc = get_pc();
    return read_file(pc->current_process, argptr[0], (void *)argptr[1], argptr[2]);
}

static int sys_fork(int64_t *argptr)
{
    return fork();
}

static int sys_exec(int64_t *argptr)
{
    struct ProcessControl *pc = get_pc();
    struct Process *process = pc->current_process;

    return exec(process, (char *)argptr[0]);
}

static int sys_keyboard_read(int64_t *argptr)
{
    return read_key_buffer();
}

static int sys_read_root_directory(int64_t *argptr)
{
    return read_root_directory((char *)argptr[0]);
}

void system_call(struct TrapFrame *tf)
{
    int64_t i = tf->x8;
    int64_t param_count = tf->x0;
    int64_t *argptr = (int64_t *)tf->x1;

    if (param_count < 0 || i < 0 || i > 11)
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
    system_calls[4] = sys_open_file;
    system_calls[5] = sys_close_file;
    system_calls[6] = sys_get_file_size;
    system_calls[7] = sys_read_file;
    system_calls[8] = sys_fork;
    system_calls[9] = sys_exec;
    system_calls[10] = sys_keyboard_read;
    system_calls[11] = sys_read_root_directory;
}
