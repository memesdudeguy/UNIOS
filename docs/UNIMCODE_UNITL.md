# UniMcode and UniTL

## UniMcode

UniMcode is the UNIOS native machine-code compatibility layer.

It is not a virtual machine and does not boot a guest operating system.
It translates foreign user-process instructions into native UNIOS instructions.

Execution modes:

1. Native i386 ELF: execute directly.
2. Foreign ISA ELF: decode blocks into UniIR, translate to i386 code, cache blocks,
   and execute the native code.
3. Unsupported ISA: reject safely with an executable-format error.

## UniTL

UniTL is the Linux ABI translation layer.

Arch Linux applications normally use the Linux ELF format, glibc/musl,
and Linux system calls. UniTL translates supported Linux system calls into
UNIOS kernel services.

UniTL does not copy Arch Linux internals. It implements compatible behavior.

Initial Linux syscall targets:

- exit
- read
- write
- openat
- close
- brk
- mmap
- munmap
- fstat
- clock_gettime
- getpid
- uname

## Safety requirements

- Foreign code always runs in user mode.
- Translation cache pages are never writable and executable at the same time.
- Invalid instructions terminate only the process.
- Syscall arguments are validated before crossing into the kernel.
- Unsupported instructions return ENOSYS or an executable-format error.
- No password, filesystem, or kernel memory is exposed to translated code.
