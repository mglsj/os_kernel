rm -rf build/test
mkdir build/test

aarch64-linux-gnu-gcc -c src/test/test.s -o build/test/test.o
aarch64-linux-gnu-objcopy -O binary build/test/test.o build/test/test.bin
