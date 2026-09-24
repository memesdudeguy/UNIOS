# UNIOS

UNIOS is an experimental x86 operating system built for bare-metal i386 and QEMU. It is intentionally conservative: it targets x86-32, keeps a minimal command shell, and rejects unsupported architectures until they are implemented safely.

## Goals

- Build a freestanding x86 kernel
- Support a serial console and minimal shell in QEMU
- Keep the toolchain profile-aware and safe
- Register drivers only after matching device IDs or classes
- Provide a path toward a Linux-like syscall layer without fake compatibility

## Build

```bash
make clean && make
make run
```

## Notes

- Native architecture: x86 (i386 / i486)
- Default target: i386-unknown-none-elf
- Default CPU profile: i486
- Unsupported architectures are rejected explicitly
