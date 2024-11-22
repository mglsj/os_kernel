echo -e "${Yellow}[DEBUG] Building Console${Color_Off}"

rm -rf build/programs/console
mkdir build/programs/console

echo "[DEBUG] Assembling Console"
# Assemble
aarch64-linux-gnu-gcc \
    -c src/programs/console/start.s \
    -o build/programs/console/start.o

echo "[DEBUG] Compiling Console"
# Compile
aarch64-linux-gnu-gcc -std=c99 -ffreestanding -mgeneral-regs-only \
    -c src/programs/console/main.c \
    -o build/programs/console/main.o

echo "[DEBUG] Linking Console"
# Link all the files
aarch64-linux-gnu-ld -nostdlib \
    -T src/programs/console/link.lds \
    -o build/programs/console/console \
    build/programs/console/start.o \
    build/programs/console/main.o \
    build/lib/lib.a

echo "[DEBUG] Converting Console"
# Convert the linked file to binary
aarch64-linux-gnu-objcopy -O binary \
    build/programs/console/console \
    build/programs/console/console.bin
