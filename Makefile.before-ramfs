PROJECT := UNIOS
BUILD := build
ISO := iso
CC := $(CURDIR)/toolchain/aliencc
LD := ld.lld
AS := clang

CFLAGS := \
    --target=i386-unknown-none-elf \
    -ffreestanding \
    -fno-builtin \
    -fno-stack-protector \
    -fno-pic \
    -m32 \
    -march=i486 \
    -mno-sse \
    -mno-mmx \
    -mno-80387 \
    -Wall -Wextra \
    -O2

all: iso

$(BUILD)/boot.o: boot/boot.s
	mkdir -p $(BUILD)
	$(AS) --target=i386-unknown-none-elf -m32 -c $< -o $@

$(BUILD)/kernel.o: kernel/kernel.c
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/unios.kernel: $(BUILD)/boot.o $(BUILD)/kernel.o kernel/linker.ld
	$(LD) -m elf_i386 -T kernel/linker.ld \
		-z noexecstack \
		-o $@ \
		$(BUILD)/boot.o \
		$(BUILD)/kernel.o

iso: $(BUILD)/unios.kernel
	mkdir -p $(ISO)/boot
	cp $(BUILD)/unios.kernel $(ISO)/boot/unios.kernel
	grub-mkrescue -o $(BUILD)/unios.iso $(ISO)

run: iso
	qemu-system-i386 \
		-cdrom $(BUILD)/unios.iso \
		-m 32M \
		-serial stdio

clean:
	rm -rf $(BUILD) iso/boot/unios.kernel
