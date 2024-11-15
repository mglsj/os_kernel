.equ MAIR_ATTR, (0x44 << 8)
.equ TCR_T0SZ,  (16) 
.equ TCR_T1SZ,  (16 << 16)
.equ TCR_TG0,   (0 << 14)
.equ TCR_TG1,   (2 << 30)
.equ TCR_VALUE, (TCR_T0SZ | TCR_T1SZ | TCR_TG0 | TCR_TG1)
.equ PAGE_SIZE, (2*1024*1024)

.global enable_mmu
.global setup_vm
.global load_pgd
.global read_pgd


read_pgd:
    mrs x0, ttbr0_el1
    ret

load_pgd:
    msr ttbr0_el1, x0
    tlbi vmalle1is
    dsb ish
    isb

    ret

enable_mmu:
    adr x0, pgd_ttbr1
    msr ttbr1_el1, x0

    adr x0, pgd_ttbr0
    msr ttbr0_el1, x0

    ldr x0, =MAIR_ATTR
    msr mair_el1, x0

    ldr x0, =TCR_VALUE
    msr tcr_el1, x0

    mrs x0, sctlr_el1
    orr x0, x0, #1
    msr sctlr_el1, x0
    
    ret


setup_vm:
setup_kvm:
    adr x0, pgd_ttbr1
    adr x1, pud_ttbr1
    orr x1, x1, #3
    str x1, [x0]

    adr x0, pud_ttbr1
    adr x1, pmd_ttbr1
    orr x1, x1, #3
    str x1, [x0]

    mov x2, #0x34000000
    adr x1, pmd_ttbr1
    mov x0, #(1 << 10 | 1 << 2 | 1 << 0)

loop1:
    str x0, [x1], #8
    add x0, x0, #PAGE_SIZE
    cmp x0, x2
    blo loop1

    mov x2, #0x40000000
    mov x0, #0x3f000000

    adr x3, pmd_ttbr1
    lsr x1, x0, #(21 - 3)
    add x1, x1, x3

    orr x0, x0, #1
    orr x0, x0, #(1 << 10)

loop2:
    str x0, [x1], #8
    add x0, x0, #PAGE_SIZE
    cmp x0, x2
    blo loop2

    adr x0, pud_ttbr1
    add x0, x0, #(1 * 8)
    adr x1, pmd2_ttbr1
    orr x1, x1, #3
    str x1, [x0]

    mov x2, #0x41000000
    mov x0, #0x40000000

    adr x1, pmd2_ttbr1
    orr x0, x0, #1
    orr x0, x0, #(1 << 10)

loop3:
    str x0, [x1], #8
    add x0, x0, #PAGE_SIZE
    cmp x0, x2
    blo loop3


setup_uvm:
    adr x0, pgd_ttbr0
    adr x1, pud_ttbr0
    orr x1, x1, #3
    str x1, [x0]

    adr x0, pud_ttbr0
    adr x1, pmd_ttbr0
    orr x1, x1, #3
    str x1, [x0]

    adr x1, pmd_ttbr0
    mov x0, #(1 << 10 | 1 << 2 | 1 << 0)
    str x0, [x1]

    ret


.balign 4096
pgd_ttbr1:
    .space 4096
pud_ttbr1:
    .space 4096
pmd_ttbr1:
    .space 4096
pmd2_ttbr1:
    .space 4096

pgd_ttbr0:
    .space 4096
pud_ttbr0:
    .space 4096
pmd_ttbr0:
    .space 4096



	