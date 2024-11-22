echo -e "${Yellow}[DEBUG] Building Init${Color_Off}"

rm -rf build/init
mkdir build/init

echo "[DEBUG] Assembling Init"
aarch64-linux-gnu-gcc -c src/init/start.s -o build/init/start.o

echo "[DEBUG] Compiling Init"
# Compile
aarch64-linux-gnu-gcc -std=c99 -ffreestanding -mgeneral-regs-only -c src/init/main.c -o build/init/main.o

echo "[DEBUG] Linking Init"
# Link all the files
aarch64-linux-gnu-ld -nostdlib -T src/init/link.lds \
    -o build/init/init \
    build/init/start.o \
    build/init/main.o \
    build/lib/lib.a

echo "[DEBUG] Converting Init"
# Convert the linked file to binary
aarch64-linux-gnu-objcopy -O binary build/init/init build/init/init.bin
