.section .text
.global writeu
.global sleepu
.global exitu
.global waitu
.global open_file
.global close_file
.global get_file_size
.global read_file
.global fork
.global exec
.global keyboard_read
.global read_root_directory

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

open_file:
    sub sp, sp, #8
    mov x8, #4

    str x0, [sp]
    mov x0, #1
    mov x1, sp

    svc #1234

    add sp, sp, #8
    ret

close_file:
    sub sp, sp, #8
    mov x8, #5

    str x0, [sp]
    mov x0, #1
    mov x1, sp

    svc #1234

    add sp, sp, #8
    ret

get_file_size:
    sub sp, sp, #8
    mov x8, #6

    str x0, [sp]
    mov x0, #1
    mov x1, sp

    svc #1234

    add sp, sp, #8
    ret

read_file:
    sub sp, sp, #24
    mov x8, #7

    stp x0, x1, [sp]
    str x2, [sp, #16]

    mov x0, #3
    mov x1, sp

    svc #1234

    add sp, sp, #24
    ret

fork:
    mov x8, #8
    mov x0, #0

    svc #1234
    ret

exec:
    sub sp, sp, #8
    mov x8, #9

    str x0, [sp]
    mov x0, #1
    mov x1, sp

    svc #1234

    add sp, sp, #8
    ret

keyboard_read:
    mov x8, #10
    mov x0, #0

    svc #1234
    ret

read_root_directory:
    sub sp, sp, #8
    mov x8, #11

    str x0, [sp]
    mov x0, #1
    mov x1, sp

    svc #1234

    add sp, sp, #8
    ret


.global presentation_show
.global presentation_close
.global presentation_next
.global presentation_prev

presentation_show:
    mov x8, #12
    mov x0, #0

    svc #1234
    ret

presentation_close:
    mov x8, #13
    mov x0, #0

    svc #1234
    ret

presentation_next:
    mov x8, #14
    mov x0, #0

    svc #1234
    ret

presentation_prev:
    mov x8, #15
    mov x0, #0

    svc #1234
    ret
