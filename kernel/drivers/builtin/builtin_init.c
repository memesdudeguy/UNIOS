#include "../drivers.h"

/*
 * Single entry point that registers every built-in driver, then runs the
 * PCI enumeration + match pass. Order matters only for display fallback:
 * vendor-specific GPU drivers register before any generic handling.
 */
void unios_drivers_builtin_init(void) {
    unios_drivers_init();

    /* Legacy / always-present hardware first so console works even if
     * PCI enumeration finds nothing. */
    vga_console_register();
    ps2_keyboard_register();
    serial_uart_register();

    /* Display adapters (Linux DRM equivalents). */
    intel_igpu_register();
    amd_amdgpu_register();
    nouveau_nv_register();

    /* Wired networking. */
    rtl8169_register();
    e1000_register();

    /* Wireless LAN (WiFi) — modern x86_64 laptops/boards. */
    ath9k_register();
    iwlwifi_register();
    mt76_register();
    brcmfmac_register();
    rtw88_register();

    /* USB host stack controllers. */
    xhci_register();
    ehci_register();
    ohci_register();
    uhci_register();

    /* Storage. */
    nvme_register();
    ahci_register();
    ata_piix_register();

    /* Audio. */
    hda_intel_register();
    ac97_register();

    /* Enumerate PCI and bind matching drivers to real hardware. */
    if (pci_enumerate() > 0) {
        for (uint32_t i = 0; i < pci_device_count(); ++i) {
            unios_driver_bind_pci(pci_device_at(i));
        }
    }

    /* Bring up the port-matched legacy ISA drivers now that we know the
     * machine shape (probe functions do the actual sanity checks). */
    unios_drivers_activate_legacy();
}
