# Define variables
CC = aarch64-linux-gnu-gcc
LD = aarch64-linux-gnu-ld
OBJCOPY = aarch64-linux-gnu-objcopy
CFLAGS = -std=c99 -ffreestanding -mgeneral-regs-only -D__TARGET_QEMU__
GPU_CFLAGS = -std=c99 -Wall -ffreestanding -fno-stack-protector -nostdlib -nostartfiles -mgeneral-regs-only
LDFLAGS = -nostdlib -T src/kernel/link.lds
BUILD_DIR = build/kernel
GPU_DIR = $(BUILD_DIR)/gpu
SRC_DIR = src/kernel
IMG = build/kernel8.img

# Source files
ASM_SRCS = boot.S lib.S handler.S mmu.S
C_SRCS = main.c uart.c print.c debug.c handler.c memory.c file.c process.c syscall.c lib.c keyboard.c
GPU_SRCS = delays.c video.c mailbox.c fonts.c dma.c terminal.c

# Object files
ASM_OBJS = $(ASM_SRCS:%.S=$(BUILD_DIR)/%.o)
C_OBJS = $(C_SRCS:%.c=$(BUILD_DIR)/%.o)
GPU_OBJS = $(GPU_SRCS:%.c=$(GPU_DIR)/%.o)

# Default target
all: $(IMG)

# Create build directories
$(BUILD_DIR):
    mkdir -p $(BUILD_DIR)
    mkdir -p $(GPU_DIR)

# Assemble .S files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.S | $(BUILD_DIR)
    $(CC) -c $< -o $@ $(CFLAGS)

# Compile .c files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
    $(CC) -c $< -o $@ $(CFLAGS)

# Compile GPU .c files
$(GPU_DIR)/%.o: $(SRC_DIR)/gpu/%.c | $(BUILD_DIR)
    $(CC) -c $< -o $@ $(GPU_CFLAGS)

# Link the kernel
$(BUILD_DIR)/kernel: $(ASM_OBJS) $(C_OBJS) $(GPU_OBJS)
    $(LD) $(LDFLAGS) -o $@ $^

# Convert the linked file to binary
$(IMG): $(BUILD_DIR)/kernel
    $(OBJCOPY) -O binary $< $@
    @# Get the size of the binary image
    size=$(shell stat -c%s "$@")
    @# Calculate the padding needed to align to 16 bytes
    padding=$$(( (16 - (size % 16)) % 16 ))
    @# Add the padding to the binary image
    if [ $$padding -ne 0 ]; then dd if=/dev/zero bs=1 count=$$padding >> $@; fi

# Clean build files
clean:
    rm -rf $(BUILD_DIR) $(IMG)

# Include additional scripts
.PHONY: all clean