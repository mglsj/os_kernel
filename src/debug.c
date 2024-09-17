#include "debug.h"
#include "print.h"

void error_check(char *file, uint64_t line)
{
    printk("\r\n----------------------------------------\r\n");
    printk("              ERROR CHECK");
    printk("\r\n----------------------------------------\r\n");
    printk("Assertion Failed [%s: %u]\r\n", file, line);

    while (1)
        ;
}
