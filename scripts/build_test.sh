echo -e "${Yellow}[DEBUG] Building Test${Color_Off}"

rm -rf build/test
mkdir build/test

echo "[DEBUG] Assembling Test"
aarch64-linux-gnu-gcc -c src/test/test.s -o build/test/test.o

echo "[DEBUG] Linking Test"
aarch64-linux-gnu-objcopy -O binary build/test/test.o build/test/test.bin
