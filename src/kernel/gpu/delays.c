#include "../irq.h"

#define SYSTMR_LO ((volatile unsigned int *)(BASE_ADDR + 0x00003004))
#define SYSTMR_HI ((volatile unsigned int *)(BASE_ADDR + 0x00003008))

/**
 * Wait N CPU cycles (ARM CPU only)
 */
void wait_cycles(unsigned int n)
{
    if (n)
        while (n--)
        {
            __asm__ volatile("nop");
        }
}

/**
 * Wait N microsec (ARM CPU only)
 */
void wait_micro_sec(unsigned int n)
{
    register unsigned long f, t, r;
    // get the current counter frequency
    __asm__ volatile("mrs %0, cntfrq_el0" : "=r"(f));
    // read the current counter
    __asm__ volatile("mrs %0, cntpct_el0" : "=r"(t));
    // calculate required count increase
    unsigned long i = ((f / 1000) * n) / 1000;
    // loop while counter increase is less than i
    do
    {
        __asm__ volatile("mrs %0, cntpct_el0" : "=r"(r));
    } while (r - t < i);
}

/**
 * Get System Timer's counter
 */
unsigned long get_system_timer()
{
    unsigned int h = -1, l;
    // we must read MMIO area as two separate 32 bit reads
    h = *SYSTMR_HI;
    l = *SYSTMR_LO;
    // we have to repeat it if high word changed during read
    if (h != *SYSTMR_HI)
    {
        h = *SYSTMR_HI;
        l = *SYSTMR_LO;
    }
    // compose long int value
    return ((unsigned long)h << 32) | l;
}

/**
 * Wait N microsec (with BCM System Timer)
 */
void wait_micro_sec_st(unsigned int n)
{
    unsigned long t = get_system_timer();
    // we must check if it's non-zero, because qemu does not emulate
    // system timer, and returning constant zero would mean infinite loop
    if (t)
        while (get_system_timer() - t < n)
            ;
}