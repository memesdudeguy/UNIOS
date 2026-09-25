# UNIOS drivers

Driver tree reworked for modern x86_64 machines, taking inspiration from the
Linux kernel driver model (bus/driver match tables, PCI capability walking,
per-class driver families) while staying small and allocation-free.

Layout:
- `driver_core.[ch]` – registry, match tables, bind/probe lifecycle
- `pci.[ch]`         – PCI config-mechanism-1 enumeration + capability list
- `ioports.h`        – shared in/out port helpers
- `builtin/`         – ISA/VGA/PS2/UART + boot-time registration of all drivers
- `gpu/`             – i915-, amdgpu-, nouveau-style display drivers
- `net/`             – r8169 and e1000 Ethernet
- `wifi/`            – ath9k, iwlwifi, mt76, brcmfmac, rtw88 wireless LAN
- `usb/`             – UHCI/OHCI/EHCI/xHCI host controllers
- `storage/`         – AHCI, ata_piix, NVMe
- `audio/`           – HD Audio (hda-intel) and AC97

Third-party-vendor hardware is covered through the same vendor-specific PCI
ID tables (Realtek, MediaTek, Broadcom/Cypress, ASMedia, Renesas, Samsung,
Kioxia, etc.), matching how the Linux kernel supports them.

Rules (unchanged):
- no direct Linux kernel imports; tables are re-implemented here
- no DMA until a safe allocator exists
- no untrusted memory execution (firmware loading must be checksummed)
- no unsupported ISA execution
- use the driver registry to verify hardware identity before probing

Removed as not needed: `linux-port/alien_driver_api.h` (dead header, never
compiled into the kernel).
