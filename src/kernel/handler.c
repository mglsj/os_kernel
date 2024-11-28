#include "handler.h"
#include "irq.h"
#include "lib.h"
#include "print.h"
#include "process.h"
#include "stdint.h"
#include "syscall.h"
#include "uart.h"

#ifdef __TARGET_QEMU__

void enable_timer(void);
uint32_t read_timer_status(void);
void set_timer_interval(uint32_t value);
uint32_t read_timer_freq(void);
static uint32_t timer_interval = 0;

#endif

static uint64_t ticks = 0;

void init_interrupt_controller(void)
{
    out_word(DISABLE_BASIC_IRQS, 0xffffffff);
    out_word(DISABLE_IRQS_1, 0xffffffff);
    out_word(DISABLE_IRQS_2, 0xffffffff);

#ifdef __TARGET_RPI3__
    out_word(ENABLE_BASIC_IRQS, 1);
#endif

    out_word(ENABLE_IRQS_2, (1 << 25));
}

void init_timer(void)
{
#ifdef __TARGET_QEMU__
    timer_interval = read_timer_freq() / 100;
    enable_timer();
    out_word(CNTP_EL0, (1 << 1));
#endif

#ifdef __TARGET_RPI3__
    out_word(TIMER_PREDIV, 0x7d);
    out_word(TIMER_LOAD, 19841);
    out_word(TIMER_CTL, 0b10100010);
#endif
}

static void timer_interrupt_handler(void)
{
#ifdef __TARGET_QEMU__
    uint32_t status = read_timer_status();
    if (status & (1 << 2))
    {
        ticks++;
        wake_up(-1);
        set_timer_interval(timer_interval);
    }
#endif

#ifdef __TAGERT_RPI3__
    uint32_t mask = in_word(TIMER_MSKIRQ);
    if (mask & 1)
    {
        ticks++;
        wake_up(-1);
        out_word(TIMER_ACK, 1);
    }
#endif
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
            printk("sync error occurs in process %d\r\n", (int64_t)process_control->current_process->pid);
            exit();
        }
        else
        {
            printk("\r\n");
            printk("sync error at %x: %x\r\n", tf->elr, tf->esr);
            printk("Registers: \r\n");
            printk("x0:\t%x\tx1:\t%x\r\n", tf->x0, tf->x1);
            printk("x2:\t%x\tx3:\t%x\r\n", tf->x2, tf->x3);
            printk("x4:\t%x\tx5:\t%x\r\n", tf->x4, tf->x5);
            printk("x6:\t%x\tx7:\t%x\r\n", tf->x6, tf->x7);
            printk("x8:\t%x\tx9:\t%x\r\n", tf->x8, tf->x9);
            printk("x10:\t%x\tx11:\t%x\r\n", tf->x10, tf->x11);
            printk("x12:\t%x\tx13:\t%x\r\n", tf->x12, tf->x13);
            printk("x14:\t%x\tx15:\t%x\r\n", tf->x14, tf->x15);
            printk("x16:\t%x\tx17:\t%x\r\n", tf->x16, tf->x17);
            printk("x18:\t%x\tx19:\t%x\r\n", tf->x18, tf->x19);
            printk("x20:\t%x\tx21:\t%x\r\n", tf->x20, tf->x21);
            printk("x22:\t%x\tx23:\t%x\r\n", tf->x22, tf->x23);
            printk("x24:\t%x\tx25:\t%x\r\n", tf->x24, tf->x25);
            printk("x26:\t%x\tx27:\t%x\r\n", tf->x26, tf->x27);
            printk("x28:\t%x\tx29:\t%x\r\n", tf->x28, tf->x29);
            printk("x30:\t%x\r\n", tf->x30);
            printk("\r\n");
            while (1)
            {
            }
        }
        break;

    case 2:
#ifdef __TARGET_QEMU__
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
#endif

#ifdef __TARGET_RPI3__
        irq = get_irq_number();
        if (irq & 1)
        {
            timer_interrupt_handler();
            schedule = 1;
        }
        else if (irq & (1 << 19))
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
#endif
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