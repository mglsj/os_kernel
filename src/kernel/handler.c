#include "handler.h"
#include "irq.h"
#include "lib.h"
#include "print.h"
#include "process.h"
#include "stdint.h"
#include "syscall.h"
#include "uart.h"

void enable_timer(void);
uint32_t read_timer_status(void);
void set_timer_interval(uint32_t value);
uint32_t read_timer_freq(void);

static uint32_t timer_interval = 0;
static uint64_t ticks = 0;

void init_interrupt_controller(void)
{
    out_word(DISABLE_BASIC_IRQS, 0xffffffff);
    out_word(DISABLE_IRQS_1, 0xffffffff);
    out_word(DISABLE_IRQS_2, 0xffffffff);

    out_word(ENABLE_IRQS_2, (1 << 25));
}

void init_timer(void)
{
    timer_interval = read_timer_freq() / 100;
    enable_timer();
    out_word(CNTP_EL0, (1 << 1));
}

static void timer_interrupt_handler(void)
{
    uint32_t status = read_timer_status();
    if (status & (1 << 2))
    {
        ticks++;
        wake_up(-1);
        set_timer_interval(timer_interval);
    }
}

static uint32_t get_irq_number(void)
{
    return in_word(IRQ_BASIC_PENDING);
}

uint64_t get_ticks(void)
{
    return ticks;
}

void handler(struct TrapFrame *tf)
{
    uint32_t irq;
    int schedule = 0;
    struct ProcessControl *process_control;

    switch (tf->trapno)
    {
    case 1:
        if ((tf->spsr & 0xf) == 0)
        {
            process_control = get_pc();
            printk("sync error occurs in process: %d\r\n", process_control->current_process->pid);
            exit();
        }
        else
        {
            printk("sync error at %x: %x\r\n", tf->elr, tf->esr);
            while (1)
            {
            }
        }
        break;

    case 2:
        irq = in_word(CNTP_STATUS_EL0);
        if (irq & (1 << 1))
        {
            timer_interrupt_handler();
            schedule = 1;
        }
        else
        {
            irq = get_irq_number();
            if (irq & (1 << 19))
            {
                uart_handler();
            }
            else
            {
                printk("unknown irq\r\n");
                while (1)
                {
                }
            }
        }
        break;
    case 3:
        system_call(tf);
        break;
    default:
        printk("unknown exception\r\n");
        while (1)
        {
        }
    }

    if (schedule == 1)
    {
        yield();
    }
}