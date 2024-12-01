echo -e "${Yellow}[DEBUG] Building PPT${Color_Off}"

rm -rf build/programs/ppt
mkdir build/programs/ppt

echo "[DEBUG] Assembling PPT"
# Assemble
aarch64-linux-gnu-gcc \
    -c src/programs/ppt/start.s \
    -o build/programs/ppt/start.o

echo "[DEBUG] Compiling PPT"
# Compile
aarch64-linux-gnu-gcc -std=c99 -ffreestanding -mgeneral-regs-only \
    -c src/programs/ppt/main.c \
    -o build/programs/ppt/main.o

echo "[DEBUG] Linking PPT"
# Link all the files
aarch64-linux-gnu-ld -nostdlib \
    -T src/programs/ppt/link.lds \
    -o build/programs/ppt/ppt \
    build/programs/ppt/start.o \
    build/programs/ppt/main.o \
    build/lib/lib.a

echo "[DEBUG] Converting PPT"
# Convert the linked file to binary
aarch64-linux-gnu-objcopy -O binary \
    build/programs/ppt/ppt \
    build/programs/ppt/ppt.bin
