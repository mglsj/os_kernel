ENTRY(start)
SECTIONS
{
    . = 0xffff000000080000;

    .text : {
        *(.text)
    }

    .rodata : {
        *(.rodata)
    }

    . = ALIGN(16);
    .data : {
        *(.data)
    }

    .bss : {
        . = ALIGN(16);
        fs_start = .;
        bss_start = .;
        *(.bss)
        bss_end = .;
    }

    .reserved_mem : {
        . = ALIGN(16);
        reserved_mem_start = .;
        . = . + 60M;
        reserved_mem_end = .;
    }

    _kernel_end = .;
}