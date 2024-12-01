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

#include "gpu/terminal.h"

#include "presentation/presentation.h"

void KMain(void)
{
    init_uart();
    printk("Hello from \033[1;31mRaspberry\033[0m \033[1;32mpi\033[0m\r\n\r\n");

    init_terminal();
    init_presentation();

    init_memory();
    init_fs();
    init_system_call();
    init_timer();
    init_interrupt_controller();
    init_process();
    enable_irq();
}
