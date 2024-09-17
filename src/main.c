#include "debug.h"
#include "print.h"
#include "uart.h"

void KMain(void)
{
    init_uart();
    printk("Hello from raspberry pi.\r\n");

    ASSERT(0);

    while (1)
    {
        ;
    }
}