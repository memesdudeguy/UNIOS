# UNIOS

UNIOS is an independent small Unix-like operating system.

Goals:

- Original kernel
- C, C++, and assembly support
- Alien Compiler toolchain wrappers
- Built-in terminal without Bash or another shell
- Linux driver compatibility layer
- POSIX-oriented userspace
- i386 support for very old x86 hardware
- Future x86_64, ARM64, and RISC-V targets
- Hardened memory and syscall boundaries
- Small optimized kernel

The initial target is i386 Multiboot under QEMU.

## Device manager

UNIOS scans PCI, USB, ISA, and ACPI buses. It compares hardware IDs with
drivers in `drivers/catalog/drivers.db`.

Drivers are selected by the highest matching priority. Unused removable
drivers are marked as garbage only after they have zero references. Built-in
drivers are never deleted.

Run the catalog preview with:

    tools/unios-driver-scan

## OrbitCLI command themes

Unix-compatible commands retain their normal names for software portability.

UNIOS commands use a spacecraft vocabulary:

- `scan` detects hardware
- `orbit` lists devices
- `dock` mounts storage
- `undock` unmounts storage
- `fuel` displays system resources
- `crew` displays processes
- `nav` displays the current path
- `comet` searches for files
- `airlock` safely removes files
- `starmap` lists mounted filesystems
- `drivers` lists active drivers
