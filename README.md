# UNIOS

UNIOS is an experimental x86 operating system for QEMU and bare-metal i386-compatible hardware.

## Build commands

Run these commands from the repository root:

```bash
make check        # verify required tools
make              # build the bootable ISO
make run          # build and boot UNIOS in QEMU
make run-debug    # boot QEMU with extra diagnostics
make clean        # remove generated files
```

Inside the UNIOS serial console, type:

```text
help
?
about
ls
pwd
clear
```

Friendly aliases include:

| Command | Alias |
|---|---|
| `help` | `?` |
| `clear` | `cls` |
| `ls` | `dir` |
| `rm` | `del` |
| `pwd` | `nav` |
| `exit` | `quit` |

Commands requiring arguments show examples instead of silently failing. Unsupported kernel features report their status clearly rather than pretending to complete.

## Current scope

- Native target: `i386-unknown-none-elf`
- Default CPU profile: `i486`
- Serial shell: supported
- ELF/process support: validation and metadata foundation only
- Arch/pacman/AUR applications: not supported yet
- Unsupported architectures: rejected explicitly
