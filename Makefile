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

KERNEL_OBJECTS := \
	$(BUILD)/boot.o \
	$(BUILD)/kernel.o \
	$(BUILD)/ramfs.o \
	$(BUILD)/unitl.o \
	$(BUILD)/device_manager.o

.PHONY: all iso run clean

all: iso

$(BUILD)/boot.o: boot/boot.s
	mkdir -p $(BUILD)
	$(AS) --target=i386-unknown-none-elf -m32 -c $< -o $@

$(BUILD)/kernel.o: kernel/kernel.c kernel/ramfs.h kernel/auth_config.h
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -Ikernel -c $< -o $@

$(BUILD)/ramfs.o: kernel/ramfs.c kernel/ramfs.h
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -Ikernel -c $< -o $@

$(BUILD)/unitl.o: kernel/compat/unitl.c kernel/compat/unitl.h
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -Ikernel -Ikernel/compat -c $< -o $@

$(BUILD)/device_manager.o: \
	kernel/device-manager/device_manager.c \
	kernel/device-manager/device_manager.h \
	kernel/drivers/linux-port/alien_driver_api.h
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) \
		-Ikernel \
		-Ikernel/device-manager \
		-Ikernel/drivers/linux-port \
		-c $< -o $@

$(BUILD)/unios.kernel: $(KERNEL_OBJECTS) kernel/linker.ld
	$(LD) -m elf_i386 -T kernel/linker.ld \
		-z noexecstack \
		-o $@ \
		$(KERNEL_OBJECTS)

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
	rm -rf $(BUILD) $(ISO)
