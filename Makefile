PROJECT := UNIOS
BUILD := build
IMAGE := $(BUILD)/unios.img
KERNEL_SECTORS := 64

CC := $(CURDIR)/toolchain/aliencc
LD := ld.lld
AS := clang
OBJCOPY := llvm-objcopy

# CPU profile: default targets modern x86_64 machines (x86-64-v2: SSE4.2,
# POPCNT and friends are on every Intel Nehalem+ / AMD Excavator+ CPU).
# Override with `make UNIOS_CPU=i486` for legacy 32-bit QEMU/ISA testing.
UNIOS_CPU ?= x86-64-v2
export UNIOS_CPU

ifeq ($(filter i386 i486 pentium,$(UNIOS_CPU)),)
ARCH_CFLAGS := -march=$(UNIOS_CPU) -mtune=generic
else
ARCH_CFLAGS := -march=$(UNIOS_CPU) -mno-sse -mno-mmx -mno-80387
endif

CFLAGS := \
	--target=i386-unknown-none-elf \
	-ffreestanding -fno-builtin -fno-stack-protector -fno-pic \
	-m32 $(ARCH_CFLAGS) \
	-fomit-frame-pointer \
	-Wall -Wextra -O2

DRIVER_SOURCES := $(wildcard kernel/drivers/*.c kernel/drivers/*/*.c)
# Flatten paths: kernel/drivers/pci.c -> build/drv_kernel_drivers_pci.o
DRIVER_OBJECTS := $(patsubst %.c,$(BUILD)/drv_%.o,$(subst /,_,$(patsubst kernel/%,%,$(DRIVER_SOURCES))))

KERNEL_OBJECTS := \
	$(BUILD)/boot.o \
	$(BUILD)/kernel.o \
	$(BUILD)/unitl.o \
	$(BUILD)/device_manager.o \
	$(BUILD)/elf32.o \
	$(BUILD)/process.o \
	$(DRIVER_OBJECTS)

.PHONY: all image run run-debug clean help check drivers

all: image

help:
	@printf '%s\n' \
	  'UNIOS build commands:' \
	  '  make              Build a bootable raw disk image' \
	  '  make run          Boot the image in QEMU' \
	  '  make run-debug    Boot QEMU with debug output' \
	  '  make drivers      List built-in driver sources' \
	  '  make check        Check required host tools' \
	  '  make clean        Remove generated files'

check:
	@command -v clang >/dev/null || { echo 'Missing: clang'; exit 1; }
	@command -v ld.lld >/dev/null || { echo 'Missing: ld.lld'; exit 1; }
	@command -v $(OBJCOPY) >/dev/null || { echo 'Missing: $(OBJCOPY)'; exit 1; }
	@command -v qemu-system-i386 >/dev/null || { echo 'Missing: qemu-system-i386'; exit 1; }
	@test -x toolchain/aliencc || { echo 'Missing executable: toolchain/aliencc'; exit 1; }
	@echo 'All required build tools are available.'

$(BUILD)/mbr.o: boot/mbr.s
	mkdir -p $(BUILD)
	$(AS) --target=i386-pc-none-elf -m16 -c $< -o $@

$(BUILD)/mbr.bin: $(BUILD)/mbr.o boot/mbr.ld
	$(LD) -m elf_i386 -T boot/mbr.ld --oformat binary -o $@ $<
	@test "$$(stat -c%s $@)" -eq 512 || { echo 'MBR must be exactly 512 bytes'; exit 1; }

$(BUILD)/boot.o: boot/boot.s
	mkdir -p $(BUILD)
	$(AS) --target=i386-unknown-none-elf -m32 -c $< -o $@

$(BUILD)/kernel.o: kernel/kernel.c kernel/elf32.h kernel/process.h
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -Ikernel -c $< -o $@

$(BUILD)/unitl.o: kernel/compat/unitl.c kernel/compat/unitl.h
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -Ikernel -Ikernel/compat -c $< -o $@

$(BUILD)/device_manager.o: kernel/device-manager/device_manager.c kernel/device-manager/device_manager.h kernel/drivers/driver_core.h kernel/drivers/drivers.h
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -Ikernel -Ikernel/device-manager -Ikernel/drivers -c $< -o $@

# Driver sources live in kernel/drivers/[<subdir>/] but must land flat in the
# build dir as drv_<path-with-underscores>.o, so map each one explicitly.
# Note: $(patsubst %.c,...) is required here -- `$(1:.c=)` would leave the
# literal text ".o" in the flattened name (no suffix is stripped).
define driver_rule
$(BUILD)/drv_$(subst /,_,$(patsubst kernel/%.c,%,$(1))).o: $(1)
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -Ikernel -Ikernel/drivers -c $$< -o $$@
endef
$(foreach src,$(DRIVER_SOURCES),$(eval $(call driver_rule,$(src))))

drivers:
	@printf '%s\n' $(DRIVER_SOURCES)

$(BUILD)/elf32.o: kernel/elf32.c kernel/elf32.h
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -Ikernel -c $< -o $@

$(BUILD)/process.o: kernel/process.c kernel/process.h
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -Ikernel -c $< -o $@

$(BUILD)/unios.kernel: $(KERNEL_OBJECTS) kernel/linker.ld
	$(LD) -m elf_i386 -T kernel/linker.ld -z noexecstack -o $@ $(KERNEL_OBJECTS)

$(BUILD)/kernel.bin: $(BUILD)/unios.kernel
	$(OBJCOPY) -O binary $< $@
	@test "$$(stat -c%s $@)" -le $$((512 * $(KERNEL_SECTORS))) || { echo 'Kernel is larger than the bootloader load limit'; exit 1; }

$(IMAGE): $(BUILD)/mbr.bin $(BUILD)/kernel.bin
	mkdir -p $(BUILD)
	dd if=/dev/zero of=$@ bs=512 count=$$((1 + $(KERNEL_SECTORS))) status=none
	dd if=$(BUILD)/mbr.bin of=$@ bs=512 seek=0 conv=notrunc status=none
	dd if=$(BUILD)/kernel.bin of=$@ bs=512 seek=1 conv=notrunc status=none

image: $(IMAGE)

run: $(IMAGE)
	@echo 'Starting UNIOS without GRUB. Press Ctrl+C to return to the host shell.'
	qemu-system-i386 -drive format=raw,file=$(IMAGE),if=ide -m 32M -serial stdio

run-debug: $(IMAGE)
	qemu-system-i386 -drive format=raw,file=$(IMAGE),if=ide -m 32M -serial stdio -d guest_errors,cpu_reset

clean:
	rm -rf $(BUILD)
