echo -e "${Yellow}[DEBUG] Building Test${Color_Off}"

rm -rf build/programs/test
mkdir build/programs/test

echo "[DEBUG] Assembling Test"
# Assemble
aarch64-linux-gnu-gcc \
    -c src/programs/test/start.s \
    -o build/programs/test/start.o

echo "[DEBUG] Compiling Test"
# Compile
aarch64-linux-gnu-gcc -std=c99 -ffreestanding -mgeneral-regs-only \
    -c src/programs/test/main.c \
    -o build/programs/test/main.o

echo "[DEBUG] Linking Test"
# Link all the files
aarch64-linux-gnu-ld -nostdlib \
    -T src/programs/test/link.lds \
    -o build/programs/test/test \
    build/programs/test/start.o \
    build/programs/test/main.o \
    build/lib/lib.a

echo "[DEBUG] Converting Test"
# Convert the linked file to binary
aarch64-linux-gnu-objcopy -O binary \
    build/programs/test/test \
    build/programs/test/test.bin
