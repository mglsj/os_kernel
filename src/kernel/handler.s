.macro kernel_entry
    sub sp, sp, #(36 * 8)
    stp x0, x1, [sp]
    stp x2, x3, [sp, #(16 * 1)]
    stp x4, x5, [sp, #(16 * 2)]
    stp	x6, x7, [sp, #(16 * 3)]
	stp	x8, x9, [sp, #(16 * 4)]
	stp	x10, x11, [sp, #(16 * 5)]
	stp	x12, x13, [sp, #(16 * 6)]
	stp	x14, x15, [sp, #(16 * 7)]
	stp	x16, x17, [sp, #(16 * 8)]
	stp	x18, x19, [sp, #(16 * 9)]
	stp	x20, x21, [sp, #(16 * 10)]
	stp	x22, x23, [sp, #(16 * 11)]
	stp	x24, x25, [sp, #(16 * 12)]
	stp	x26, x27, [sp, #(16 * 13)]
	stp	x28, x29, [sp, #(16 * 14)]
    mrs x0, sp_el0
    stp x30, x0,  [sp, #(16 * 15)]
.endm

.macro kernel_exit
    ldp x30, x0, [sp, #(16 * 15)]
    msr sp_el0, x0

    ldp x0, x1, [sp]
    ldp x2, x3, [sp, #(16 * 1)]
    ldp x4, x5, [sp, #(16 * 2)]
    ldp	x6, x7, [sp, #(16 * 3)]
	ldp	x8, x9, [sp, #(16 * 4)]
	ldp	x10, x11, [sp, #(16 * 5)]
	ldp	x12, x13, [sp, #(16 * 6)]
	ldp	x14, x15, [sp, #(16 * 7)]
	ldp	x16, x17, [sp, #(16 * 8)]
	ldp	x18, x19, [sp, #(16 * 9)]
	ldp	x20, x21, [sp, #(16 * 10)]
	ldp	x22, x23, [sp, #(16 * 11)]
	ldp	x24, x25, [sp, #(16 * 12)]
	ldp	x26, x27, [sp, #(16 * 13)]
	ldp	x28, x29, [sp, #(16 * 14)]

    add sp, sp, #(36 * 8)
    eret
.endm

.macro handler_entry
    mrs x1, esr_el1
    stp x0, x1, [sp, #(16 * 16)]

    mrs x0, elr_el1
    mrs x1, spsr_el1
    stp x0, x1, [sp, #(16 * 17)]

    mov x0, sp
    bl handler
    b trap_return
.endm

.section .text
.global vector_table
.global enable_timer
.global read_timer_freq
.global read_timer_status
.global set_timer_interval
.global enable_irq
.global trap_return
.global pstart
.global swap

.balign 0x800
vector_table:
current_el_sp0_sync:
    b error

.balign 0x80
current_el_sp0_irq:
    b error

.balign 0x80
current_el_sp0_fiq:
    b error

.balign 0x80
current_el_sp0_serror:
    b error

.balign 0x80
current_el_spn_sync:
    b sync_handler

.balign 0x80
current_el_spn_irq:
    b irq_handler

.balign 0x80
current_el_spn_fiq:
    b error

.balign 0x80
current_el_spn_serror:
    b error

.balign 0x80
lower_el_aarch64_sync:
    b sync_handler

.balign 0x80
lower_el_aarch64_irq:
    b irq_handler

.balign 0x80
lower_el_aarch64_fiq:
    b error

.balign 0x80
lower_el_aarch64_serror:
    b error

.balign 0x80
lower_el_aarch32_sync:
    b error

.balign 0x80
lower_el_aarch32_irq:
    b error

.balign 0x80
lower_el_aarch32_fiq:
    b error

.balign 0x80
lower_el_aarch32_serror:
    b error

pstart:
    mov sp, x0
    
trap_return:
    ldp x0, x1, [sp, #(16 * 17)]
    msr elr_el1, x0
    msr spsr_el1, x1
    kernel_exit

sync_handler:
    kernel_entry
    mrs x0, esr_el1
    lsr x1, x0, #26
    cmp x1, #0b010101
    mov x2, #1
    mov x3, #3
    csel x0, x2, x3, ne
    handler_entry

irq_handler:
    kernel_entry
    mov x0, #2
    handler_entry

error:
    kernel_entry
    mov x0, #0
    handler_entry

read_timer_freq:
    mrs x0, CNTFRQ_EL0
    ret

set_timer_interval:
    msr CNTP_TVAL_El0, x0
    ret

enable_timer:
    stp x29, x30, [sp, #-16]!

    bl read_timer_freq
    mov x1, #100
    udiv x0, x0, x1
    bl set_timer_interval

    mov x0, #1
    msr CNTP_CTL_EL0, x0

    ldp x29, x30, [sp], #16
    ret

read_timer_status:
    mrs x0, CNTP_CTL_EL0
    ret

enable_irq:
    msr daifclr, #2
    ret

swap:
    sub	sp,  sp,  #(12 * 8)
	stp	x19, x20, [sp, #(16 * 0)]
	stp	x21, x22, [sp, #(16 * 1)]
	stp	x23, x24, [sp, #(16 * 2)]
	stp	x25, x26, [sp, #(16 * 3)]
	stp	x27, x28, [sp, #(16 * 4)]
	stp	x29, x30, [sp, #(16 * 5)]

    mov x2, sp
    str x2, [x0]
    mov sp, x1

    ldp	x19, x20, [sp, #(16 * 0)]
	ldp	x21, x22, [sp, #(16 * 1)]
	ldp	x23, x24, [sp, #(16 * 2)]
	ldp	x25, x26, [sp, #(16 * 3)]
	ldp	x27, x28, [sp, #(16 * 4)]
	ldp	x29, x30, [sp, #(16 * 5)]
    add sp,  sp,  #(12 * 8)

    ret
    