# UNIOS drivers

Drivers are included only after a safe hardware match.

Rules:
- no direct Linux kernel imports
- no DMA until a safe allocator exists
- no untrusted memory execution
- no unsupported ISA execution
- use a driver registry to verify hardware identity
