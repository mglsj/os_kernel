#include "debug.h"
#include "file.h"
#include "handler.h"
#include "lib.h"
#include "memory.h"
#include "print.h"
#include "process.h"
#include "stddef.h"
#include "syscall.h"
#include "uart.h"

void KMain(void)
{
    init_uart();
    printk("Hello, Raspberry pi\r\n");
    printk("We are at EL %u\r\n", (uint64_t)get_el());

    init_memory();
    init_fs();
    init_system_call();
    init_timer();
    init_interrupt_controller();
    init_process();
    enable_irq();
}