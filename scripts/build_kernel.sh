echo -e "${BIYellow}[DEBUG] Building Kernel${Color_Off}"

rm -rf build/kernel
mkdir build/kernel
mkdir build/kernel/gpu

echo "[DEBUG] Assembling Kernel"
# Assemble boot.s lib.s handler.s mmu.s
aarch64-linux-gnu-gcc -c src/kernel/boot.S -o build/kernel/boot.o -D__TARGET_QEMU__
aarch64-linux-gnu-gcc -c src/kernel/lib.S -o build/kernel/liba.o -D__TARGET_QEMU__
aarch64-linux-gnu-gcc -c src/kernel/handler.S -o build/kernel/handlera.o -D__TARGET_QEMU__
aarch64-linux-gnu-gcc -c src/kernel/mmu.S -o build/kernel/mmu.o -D__TARGET_QEMU__

echo "[DEBUG] Compiling Kernel"
# Compile main.c uart.c print.c debug.c handler.c memory.c file.c process.c syscall.c
aarch64-linux-gnu-gcc -std=c99 -ffreestanding -mgeneral-regs-only -c src/kernel/main.c -o build/kernel/main.o -D__TARGET_QEMU__
aarch64-linux-gnu-gcc -std=c99 -ffreestanding -mgeneral-regs-only -c src/kernel/uart.c -o build/kernel/uart.o -D__TARGET_QEMU__
aarch64-linux-gnu-gcc -std=c99 -ffreestanding -mgeneral-regs-only -c src/kernel/print.c -o build/kernel/print.o -D__TARGET_QEMU__
aarch64-linux-gnu-gcc -std=c99 -ffreestanding -mgeneral-regs-only -c src/kernel/debug.c -o build/kernel/debug.o -D__TARGET_QEMU__
aarch64-linux-gnu-gcc -std=c99 -ffreestanding -mgeneral-regs-only -c src/kernel/handler.c -o build/kernel/handler.o -D__TARGET_QEMU__
aarch64-linux-gnu-gcc -std=c99 -ffreestanding -mgeneral-regs-only -c src/kernel/memory.c -o build/kernel/memory.o -D__TARGET_QEMU__
aarch64-linux-gnu-gcc -std=c99 -ffreestanding -mgeneral-regs-only -c src/kernel/file.c -o build/kernel/file.o -D__TARGET_QEMU__
aarch64-linux-gnu-gcc -std=c99 -ffreestanding -mgeneral-regs-only -c src/kernel/process.c -o build/kernel/process.o -D__TARGET_QEMU__
aarch64-linux-gnu-gcc -std=c99 -ffreestanding -mgeneral-regs-only -c src/kernel/syscall.c -o build/kernel/syscall.o -D__TARGET_QEMU__
aarch64-linux-gnu-gcc -std=c99 -ffreestanding -mgeneral-regs-only -c src/kernel/lib.c -o build/kernel/lib.o -D__TARGET_QEMU__
aarch64-linux-gnu-gcc -std=c99 -ffreestanding -mgeneral-regs-only -c src/kernel/keyboard.c -o build/kernel/keyboard.o -D__TARGET_QEMU__

echo "[DEBUG] Compiling GPU"
aarch64-linux-gnu-gcc -std=c99 -Wall -ffreestanding -fno-stack-protector -nostdlib -nostartfiles -mgeneral-regs-only \
    -c src/kernel/gpu/delays.c \
    -o build/kernel/gpu/delays.o
aarch64-linux-gnu-gcc -std=c99 -Wall -ffreestanding -fno-stack-protector -nostdlib -nostartfiles -mgeneral-regs-only \
    -c src/kernel/gpu/video.c \
    -o build/kernel/gpu/video.o
aarch64-linux-gnu-gcc -std=c99 -Wall -ffreestanding -fno-stack-protector -nostdlib -nostartfiles -mgeneral-regs-only \
    -c src/kernel/gpu/mailbox.c \
    -o build/kernel/gpu/mailbox.o
aarch64-linux-gnu-gcc -std=c99 -Wall -ffreestanding -fno-stack-protector -nostdlib -nostartfiles -mgeneral-regs-only \
    -c src/kernel/gpu/fonts.c \
    -o build/kernel/gpu/fonts.o
aarch64-linux-gnu-gcc -std=c99 -Wall -ffreestanding -fno-stack-protector -nostdlib -nostartfiles -mgeneral-regs-only \
    -c src/kernel/gpu/dma.c \
    -o build/kernel/gpu/dma.o
aarch64-linux-gnu-gcc -std=c99 -Wall -ffreestanding -fno-stack-protector -nostdlib -nostartfiles -mgeneral-regs-only \
    -c src/kernel/gpu/terminal.c \
    -o build/kernel/gpu/terminal.o

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
    build/kernel/keyboard.o \
    build/kernel/gpu/delays.o \
    build/kernel/gpu/video.o \
    build/kernel/gpu/mailbox.o \
    build/kernel/gpu/fonts.o \
    build/kernel/gpu/dma.o \
    build/kernel/gpu/terminal.o

echo "[DEBUG] Converting Kernel"
# Convert the linked file to binary
aarch64-linux-gnu-objcopy -O binary build/kernel/kernel build/kernel8.img

echo "[DEBUG] Padding Kernel"
# Get the size of the binary image
size=$(stat -c%s "build/kernel8.img")

# Calculate the padding needed to align to 16 bytes
padding=$(((16 - (size % 16)) % 16))

# Add the padding to the binary image
if [ $padding -ne 0 ]; then
    dd if=/dev/zero bs=1 count=$padding >>build/kernel8.img
fi

source scripts/append_file_system.sh
