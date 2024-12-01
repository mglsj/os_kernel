echo -e "${Yellow}[DEBUG] Building Clear${Color_Off}"

rm -rf build/programs/clear
mkdir build/programs/clear

echo "[DEBUG] Assembling Clear"
# Assemble
aarch64-linux-gnu-gcc \
    -c src/programs/clear/start.s \
    -o build/programs/clear/start.o

echo "[DEBUG] Compiling Clear"
# Compile
aarch64-linux-gnu-gcc -std=c99 -ffreestanding -mgeneral-regs-only \
    -c src/programs/clear/main.c \
    -o build/programs/clear/main.o

echo "[DEBUG] Linking Clear"
# Link all the files
aarch64-linux-gnu-ld -nostdlib \
    -T src/programs/clear/link.lds \
    -o build/programs/clear/clear \
    build/programs/clear/start.o \
    build/programs/clear/main.o \
    build/lib/lib.a

echo "[DEBUG] Converting Clear"
# Convert the linked file to binary
aarch64-linux-gnu-objcopy -O binary \
    build/programs/clear/clear \
    build/programs/clear/clear.bin
