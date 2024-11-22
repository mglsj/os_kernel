echo -e "${BIYellow}[DEBUG] Building Kernel${Color_Off}"

rm -rf build/kernel
mkdir build/kernel
mkdir build/kernel/gpu

echo "[DEBUG] Assembling Kernel"
# Assemble boot.s lib.s handler.s mmu.s
aarch64-linux-gnu-gcc -c src/kernel/boot.s -o build/kernel/boot.o
aarch64-linux-gnu-gcc -c src/kernel/lib.s -o build/kernel/liba.o
aarch64-linux-gnu-gcc -c src/kernel/handler.s -o build/kernel/handlera.o
aarch64-linux-gnu-gcc -c src/kernel/mmu.s -o build/kernel/mmu.o

echo "[DEBUG] Compiling Kernel"
# Compile main.c uart.c print.c debug.c handler.c memory.c file.c process.c syscall.c
aarch64-linux-gnu-gcc -std=c99 -ffreestanding -mgeneral-regs-only -c src/kernel/main.c -o build/kernel/main.o
aarch64-linux-gnu-gcc -std=c99 -ffreestanding -mgeneral-regs-only -c src/kernel/uart.c -o build/kernel/uart.o
aarch64-linux-gnu-gcc -std=c99 -ffreestanding -mgeneral-regs-only -c src/kernel/print.c -o build/kernel/print.o
aarch64-linux-gnu-gcc -std=c99 -ffreestanding -mgeneral-regs-only -c src/kernel/debug.c -o build/kernel/debug.o
aarch64-linux-gnu-gcc -std=c99 -ffreestanding -mgeneral-regs-only -c src/kernel/handler.c -o build/kernel/handler.o
aarch64-linux-gnu-gcc -std=c99 -ffreestanding -mgeneral-regs-only -c src/kernel/memory.c -o build/kernel/memory.o
aarch64-linux-gnu-gcc -std=c99 -ffreestanding -mgeneral-regs-only -c src/kernel/file.c -o build/kernel/file.o
aarch64-linux-gnu-gcc -std=c99 -ffreestanding -mgeneral-regs-only -c src/kernel/process.c -o build/kernel/process.o
aarch64-linux-gnu-gcc -std=c99 -ffreestanding -mgeneral-regs-only -c src/kernel/syscall.c -o build/kernel/syscall.o
aarch64-linux-gnu-gcc -std=c99 -ffreestanding -mgeneral-regs-only -c src/kernel/lib.c -o build/kernel/lib.o

echo "[DEBUG] Compiling GPU"
aarch64-linux-gnu-gcc -std=c99 -Wall -O2 -ffreestanding -fno-stack-protector -nostdlib -nostartfiles -mgeneral-regs-only -c src/kernel/gpu/delays.c -o build/kernel/gpu/delays.o
aarch64-linux-gnu-gcc -std=c99 -Wall -O2 -ffreestanding -fno-stack-protector -nostdlib -nostartfiles -mgeneral-regs-only -c src/kernel/gpu/lfb.c -o build/kernel/gpu/lfb.o
aarch64-linux-gnu-gcc -std=c99 -Wall -O2 -ffreestanding -fno-stack-protector -nostdlib -nostartfiles -mgeneral-regs-only -c src/kernel/gpu/mbox.c -o build/kernel/gpu/mbox.o

echo "[DEBUG] Linking Kernel"
# Link all the files
aarch64-linux-gnu-ld -nostdlib -T src/kernel/link.lds \
    -o build/kernel/kernel \
    build/kernel/boot.o \
    build/kernel/main.o \
    build/kernel/liba.o \
    build/kernel/uart.o \
    build/kernel/print.o \
    build/kernel/debug.o \
    build/kernel/handlera.o \
    build/kernel/handler.o \
    build/kernel/mmu.o \
    build/kernel/memory.o \
    build/kernel/file.o \
    build/kernel/process.o \
    build/kernel/syscall.o \
    build/kernel/lib.o \
    build/kernel/gpu/delays.o \
    build/kernel/gpu/lfb.o \
    build/kernel/gpu/mbox.o

echo "[DEBUG] Converting Kernel"
# Convert the linked file to binary
aarch64-linux-gnu-objcopy -O binary build/kernel/kernel build/kernel8.img

echo "[DEBUG] Appending filesystem to kernel"
# Append filesystem image to the kernel
dd bs=16MB if=src/os.img >>build/kernel8.img
