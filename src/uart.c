#include "uart.h"
#include "lib.h"

void write_char(unsigned char c)
{
    // Wait till input buffer is free
    while (in_word(UART0_FR) & (1 << 5))
        ;
    // Write character to UART0 data register
    out_word(UART0_DR, c);
}

unsigned char read_char(void)
{
    // Wait till there is data in receive buffer
    while (in_word(UART0_FR) & (1 << 4))
        ;
    return in_word(UART0_DR);
}

void write_string(const char *string)
{
    for (int i = 0; string[i] != '\0'; i++)
        write_char(string[i]);
}

void init_uart(void)
{
    out_word(UART0_CR, 0);
    out_word(UART0_IBRD, 26);
    out_word(UART0_FBRD, 0);
    out_word(UART0_LCRH, 1);
    out_word(UART0_LCRH, (1 << 4) | (1 << 5) | (1 << 6));
    out_word(UART0_IMSC, 0);
    out_word(UART0_CR, (1 << 0) | (1 << 8) | (1 << 9));
}