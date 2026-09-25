PROJECT := UNIOS
BUILD := build
ISO := iso

CC := $(CURDIR)/toolchain/aliencc
LD := ld.lld
AS := clang

CFLAGS := \
	--target=i386-unknown-none-elf \
	-ffreestanding -fno-builtin -fno-stack-protector -fno-pic \
	-m32 -march=i486 -mno-sse -mno-mmx -mno-80387 \
	-Wall -Wextra -O2

# Keep this list synchronized with files present in the repository.
KERNEL_OBJECTS := \
	$(BUILD)/boot.o \
	$(BUILD)/kernel.o \
	$(BUILD)/unitl.o \
	$(BUILD)/device_manager.o \
	$(BUILD)/elf32.o \
	$(BUILD)/process.o

.PHONY: all iso run clean help check run-debug

all: iso

help:
	@printf '%s\n' \
	  'UNIOS build commands:' \
	  '  make              Build the bootable ISO' \
	  '  make run          Build and start QEMU' \
	  '  make run-debug    Start QEMU with extra debug output' \
	  '  make check        Check required host tools' \
	  '  make clean        Remove generated files'

check:
	@command -v clang >/dev/null || { echo 'Missing: clang'; exit 1; }
	@command -v ld.lld >/dev/null || { echo 'Missing: ld.lld'; exit 1; }
	@command -v grub-mkrescue >/dev/null || { echo 'Missing: grub-mkrescue'; exit 1; }
	@command -v qemu-system-i386 >/dev/null || { echo 'Missing: qemu-system-i386'; exit 1; }
	@test -x toolchain/aliencc || { echo 'Missing executable: toolchain/aliencc'; exit 1; }
	@echo 'All required build tools and source files are available.'

$(BUILD)/boot.o: boot/boot.s
	mkdir -p $(BUILD)
	$(AS) --target=i386-unknown-none-elf -m32 -c $< -o $@

$(BUILD)/kernel.o: kernel/kernel.c kernel/elf32.h kernel/process.h
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -Ikernel -c $< -o $@

$(BUILD)/unitl.o: kernel/compat/unitl.c kernel/compat/unitl.h
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -Ikernel -Ikernel/compat -c $< -o $@

$(BUILD)/device_manager.o: kernel/device-manager/device_manager.c kernel/device-manager/device_manager.h kernel/drivers/linux-port/alien_driver_api.h
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -Ikernel -Ikernel/device-manager -Ikernel/drivers/linux-port -c $< -o $@

$(BUILD)/elf32.o: kernel/elf32.c kernel/elf32.h
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -Ikernel -c $< -o $@

$(BUILD)/process.o: kernel/process.c kernel/process.h
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -Ikernel -c $< -o $@

$(BUILD)/unios.kernel: $(KERNEL_OBJECTS) kernel/linker.ld
	$(LD) -m elf_i386 -T kernel/linker.ld -z noexecstack -o $@ $(KERNEL_OBJECTS)

iso: $(BUILD)/unios.kernel
	mkdir -p $(ISO)/boot
	cp $(BUILD)/unios.kernel $(ISO)/boot/unios.kernel
	grub-mkrescue -o $(BUILD)/unios.iso $(ISO)

run: iso
	@echo 'Starting UNIOS i386. Press Ctrl+C to return to the host shell.'
	qemu-system-i386 -cdrom $(BUILD)/unios.iso -m 32M -serial stdio

run-debug: iso
	qemu-system-i386 -cdrom $(BUILD)/unios.iso -m 32M -serial stdio -d guest_errors,cpu_reset

clean:
	rm -rf $(BUILD) $(ISO)
