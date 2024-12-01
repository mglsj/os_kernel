echo -e "${Yellow}[DEBUG] Building Neofetch${Color_Off}"

rm -rf build/programs/neofetch
mkdir build/programs/neofetch

echo "[DEBUG] Assembling Neofetch"
# Assemble
aarch64-linux-gnu-gcc \
    -c src/programs/neofetch/start.s \
    -o build/programs/neofetch/start.o

echo "[DEBUG] Compiling Neofetch"
# Compile
aarch64-linux-gnu-gcc -std=c99 -ffreestanding -mgeneral-regs-only \
    -c src/programs/neofetch/main.c \
    -o build/programs/neofetch/main.o

echo "[DEBUG] Linking Neofetch"
# Link all the files
aarch64-linux-gnu-ld -nostdlib \
    -T src/programs/neofetch/link.lds \
    -o build/programs/neofetch/neofetch \
    build/programs/neofetch/start.o \
    build/programs/neofetch/main.o \
    build/lib/lib.a

echo "[DEBUG] Converting Neofetch"
# Convert the linked file to binary
aarch64-linux-gnu-objcopy -O binary \
    build/programs/neofetch/neofetch \
    build/programs/neofetch/neofetch.bin
