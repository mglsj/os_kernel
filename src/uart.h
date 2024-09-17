#ifndef _UART_H
#define _UART_H

#define IO_BASE_ADDR 0x3f200000

#define UART0_DR IO_BASE_ADDR + 0x1000
#define UART0_FR IO_BASE_ADDR + 0X1018
#define UART0_CR IO_BASE_ADDR + 0X1030
#define UART0_LCRH IO_BASE_ADDR + 0X102C
#define UART0_FBRD IO_BASE_ADDR + 0X1028
#define UART0_IBRD IO_BASE_ADDR + 0X1024
#define UART0_IMSC IO_BASE_ADDR + 0X1038

unsigned char read_char(void);
void write_char(unsigned char c);
void write_string(const char *string);
void init_uart(void);

#endif