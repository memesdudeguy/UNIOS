#ifndef UNIOS_DRIVERS_H
#define UNIOS_DRIVERS_H

#include "driver_core.h"
#include "pci.h"

/* Registration entry points, one per built-in driver. */
int vga_console_register(void);
int ps2_keyboard_register(void);
int serial_uart_register(void);
int intel_igpu_register(void);
int amd_amdgpu_register(void);
int nouveau_nv_register(void);    /* NVIDIA GeForce ('nouveau'-style open port) */
int rtl8169_register(void);       /* Ethernet: Realtek RTL8169/8110 family */
int e1000_register(void);         /* Ethernet: Intel 8254x/8257x family */
int ath9k_register(void);         /* WiFi: Qualcomm Atheros AR9000 series */
int iwlwifi_register(void);       /* WiFi: Intel AX/AC wireless */
int mt76_register(void);          /* WiFi: MediaTek MT76xx PCIe wireless */
int brcmfmac_register(void);      /* WiFi: Broadcom FullMAC PCIe/SDIO */
int rtw88_register(void);         /* WiFi: Realtek RTL8821CE/RTL8822CE etc. */
int uhci_register(void);          /* USB: UHCI host controller */
int ohci_register(void);          /* USB: OHCI host controller */
int ehci_register(void);          /* USB: EHCI host controller */
int xhci_register(void);          /* USB: xHCI (USB 3.x) host controller */
int ahci_register(void);          /* Storage: SATA AHCI */
int ata_piix_register(void);      /* Storage: legacy ATA/PIIX */
int nvme_register(void);          /* Storage: NVMe SSDs */
int ac97_register(void);          /* Audio: Intel ICH AC'97 */
int hda_intel_register(void);     /* Audio: HD Audio (Intel/AMD/NI) */

/* Bring up every built-in and run the PCI match pass. */
void unios_drivers_builtin_init(void);

#endif /* UNIOS_DRIVERS_H */
