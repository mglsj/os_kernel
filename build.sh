# Assemble boot.s lib.s
aarch64-linux-gnu-gcc -c src/boot.s -o build/boot.o
aarch64-linux-gnu-gcc -c src/lib.s -o build/liba.o
# Compile main.c uart.c print.c
aarch64-linux-gnu-gcc -std=c99 -ffreestanding -mgeneral-regs-only -c src/main.c -o build/main.o
aarch64-linux-gnu-gcc -std=c99 -ffreestanding -mgeneral-regs-only -c src/uart.c -o build/uart.o
aarch64-linux-gnu-gcc -std=c99 -ffreestanding -mgeneral-regs-only -c src/print.c -o build/print.o
aarch64-linux-gnu-gcc -std=c99 -ffreestanding -mgeneral-regs-only -c src/debug.c -o build/debug.o
# Link all the files
aarch64-linux-gnu-ld -nostdlib -T src/link.lds -o build/kernel build/boot.o build/main.o build/liba.o build/uart.o build/print.o build/debug.o
# Convert the linked file to binary
aarch64-linux-gnu-objcopy -O binary build/kernel build/kernel8.img
