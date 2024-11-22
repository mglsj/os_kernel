#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "handler.h"

typedef int (*SYSTEMCALL)(int64_t *argptr);
void init_system_call(void);
void system_call(struct TrapFrame *tf);

#endif