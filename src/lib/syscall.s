.section .text
.global writeu
.global sleepu
.global exitu
.global waitu

writeu:
    sub sp, sp, #16
    mov x8, #0

    stp x0, x1, [sp]
    mov x0, #2
    mov x1, sp
    svc #1234

    add sp, sp, #16
    ret

sleepu:
    sub sp, sp, #8
    mov x8, #1

    str x0, [sp]
    mov x0, #1
    mov x1, sp
    svc #1234

    add sp, sp, #8
    ret

exitu:
    mov x8, #2

    mov x0, #0
    svc #1234

    ret

waitu:
    sub sp, sp, #8
    mov x8, #3

    str x0, [sp]
    mov x0, #1
    mov x1, sp
    svc #1234

    add sp, sp, #8
    ret
