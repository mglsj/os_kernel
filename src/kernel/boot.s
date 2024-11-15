.equ FS_BASE, 0xffff000030000000
.equ FS_SIZE, 101*16*63*512

.section .text
.global start

start:
    mrs x0, mpidr_el1
    and x0, x0, #3
    cmp x0, #0
    beq kernel_entry

end:
    b end

kernel_entry:
    mrs x0, currentel
    lsr x0, x0, #2
    cmp x0, #2
    bne end

    msr sctlr_el1, xzr
    mov x0, #(1 << 31)
    msr hcr_el2, x0

    mov x0, #0b1111000101
    msr spsr_el2, x0

    adr x0, el1_entry
    msr elr_el2, x0

    eret

el1_entry:
    mov sp, #0x80000

    bl setup_vm
    bl enable_mmu

    ldr x0, =FS_BASE
    ldr x1, =bss_start
    ldr x2, =FS_SIZE
    bl memcpy

    ldr x0, =bss_start
    ldr x1, =bss_end
    sub x2, x1, x0
    mov x1, #0
    bl memset

    ldr x0, =vector_table
    msr vbar_el1, x0
    
    mov x0, #0xffff000000000000
    add sp, sp, x0

    ldr x0, =KMain
    blr x0

halt:
    wfi
    b halt
    