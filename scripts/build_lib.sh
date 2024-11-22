echo -e "${Yellow}[DEBUG] Building Lib${Color_Off}"

rm -rf build/lib
mkdir build/lib

echo "[DEBUG] Assembling Lib"
aarch64-linux-gnu-gcc -c src/lib/syscall.s -o build/lib/syscall.o

echo "[DEBUG] Compiling Lib"
aarch64-linux-gnu-gcc -std=c99 -ffreestanding -mgeneral-regs-only -c src/lib/print.c -o build/lib/print.o

echo "[DEBUG] Archiving Lib"
aarch64-linux-gnu-ar rcs build/lib/lib.a \
    build/lib/print.o \
    build/lib/syscall.o
