echo -e "${Yellow}[DEBUG] Building LS${Color_Off}"

rm -rf build/programs/ls
mkdir build/programs/ls

echo "[DEBUG] Assembling LS"
# Assemble
aarch64-linux-gnu-gcc \
    -c src/programs/ls/start.s \
    -o build/programs/ls/start.o

echo "[DEBUG] Compiling LS"
# Compile
aarch64-linux-gnu-gcc -std=c99 -ffreestanding -mgeneral-regs-only \
    -c src/programs/ls/main.c \
    -o build/programs/ls/main.o

echo "[DEBUG] Linking LS"
# Link all the files
aarch64-linux-gnu-ld -nostdlib \
    -T src/programs/ls/link.lds \
    -o build/programs/ls/ls \
    build/programs/ls/start.o \
    build/programs/ls/main.o \
    build/lib/lib.a

echo "[DEBUG] Converting LS"
# Convert the linked file to binary
aarch64-linux-gnu-objcopy -O binary \
    build/programs/ls/ls \
    build/programs/ls/ls.bin
