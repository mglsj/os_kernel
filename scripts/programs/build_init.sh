echo -e "${Yellow}[DEBUG] Building Init${Color_Off}"

rm -rf build/programs/init
mkdir build/programs/init

echo "[DEBUG] Assembling Init"
# Assemble
aarch64-linux-gnu-gcc \
    -c src/programs/init/start.s \
    -o build/programs/init/start.o

echo "[DEBUG] Compiling Init"
# Compile
aarch64-linux-gnu-gcc -std=c99 -ffreestanding -mgeneral-regs-only \
    -c src/programs/init/main.c \
    -o build/programs/init/main.o

echo "[DEBUG] Linking Init"
# Link all the files
aarch64-linux-gnu-ld -nostdlib \
    -T src/programs/init/link.lds \
    -o build/programs/init/init \
    build/programs/init/start.o \
    build/programs/init/main.o \
    build/lib/lib.a

echo "[DEBUG] Converting Init"
# Convert the linked file to binary
aarch64-linux-gnu-objcopy -O binary \
    build/programs/init/init \
    build/programs/init/init.bin
