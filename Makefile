ASM = nasm
CC = gcc
LD = ld

ASMFLAGS = -f elf32
CFLAGS = -m32 -Wall -Wextra -Werror -c -I$(INCLUDE_DIR) -I/usr/include -fno-stack-protector -fno-pie -g
LDFLAGS = -m elf_i386 -T linker/link.ld -L/usr/lib -z execstack -no-pie

SRC_DIR = src
BUILD_DIR = build
ASM_DIR = asm
INCLUDE_DIR = include

ASM_SOURCES = $(wildcard $(ASM_DIR)/*.asm)
C_SOURCES = $(wildcard $(SRC_DIR)/*/*.c) kernel.c

ASM_OBJECTS = $(patsubst $(ASM_DIR)/%.asm,$(BUILD_DIR)/%.o,$(ASM_SOURCES))
C_OBJECTS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(C_SOURCES))

KERNEL = kernel.bin

all: $(KERNEL)

$(KERNEL): $(ASM_OBJECTS) $(C_OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(ASM_DIR)/%.asm
	@mkdir -p $(@D)
	$(ASM) $(ASMFLAGS) $< -o $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(BUILD_DIR)/*.o $(KERNEL)

run_fat32:
	qemu-system-x86_64 -drive file=disk_fat32.img,format=raw -kernel $(KERNEL)
run_ext4:
	qemu-system-x86_64 -drive file=disk_ext4.img,format=raw -kernel $(KERNEL) -s -S -serial stdio

.PHONY: all clean run