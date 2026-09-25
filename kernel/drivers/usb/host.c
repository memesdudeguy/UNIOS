#include "../driver_core.h"
#include "../pci.h"

/*
 * USB host controllers, mirroring Linux uhci-hcd / ohci-hcd / ehci-hcd /
 * xhci-hcd class matching. xHCI is what every modern x86_64 machine uses
 * for USB 3.x ports; legacy controllers kept for older hardware.
 * Root-hub enumeration waits on the same safe-DMA prerequisite.
 */

static int usb_probe_class(const struct unios_driver *self, const struct unios_pci_device *dev) {
    (void)self;
    return (dev && dev->class_code == 0x0C) ? 1 : 0;
}

static const struct unios_match_id uhci_table[] = {
    { UNIOS_BUS_CLASS, UNIOS_PCI_ID_ANY, UNIOS_PCI_ID_ANY, 0x0C, 0x03, UNIOS_CLS_USB },
};
static struct unios_driver uhci_driver = {
    "uhci", UNIOS_CLS_USB, uhci_table, 1, usb_probe_class, 0, UNIOS_DRV_STALLED
};

static const struct unios_match_id ohci_table[] = {
    { UNIOS_BUS_CLASS, UNIOS_PCI_ID_ANY, UNIOS_PCI_ID_ANY, 0x0C, 0x03, UNIOS_CLS_USB },
};
static struct unios_driver ohci_driver = {
    "ohci", UNIOS_CLS_USB, ohci_table, 1, usb_probe_class, 0, UNIOS_DRV_STALLED
};

static const struct unios_match_id ehci_table[] = {
    { UNIOS_BUS_CLASS, UNIOS_PCI_ID_ANY, UNIOS_PCI_ID_ANY, 0x0C, 0x03, UNIOS_CLS_USB },
};
static struct unios_driver ehci_driver = {
    "ehci-pci", UNIOS_CLS_USB, ehci_table, 1, usb_probe_class, 0, UNIOS_DRV_STALLED
};

static const struct unios_match_id xhci_table[] = {
    { UNIOS_BUS_CLASS, UNIOS_PCI_ID_ANY, UNIOS_PCI_ID_ANY, 0x0C, 0x03, UNIOS_CLS_USB }, /* prog_if 0x30 = xHCI, checked at probe */
    { UNIOS_BUS_PCI, 0x8086, UNIOS_PCI_ID_ANY, 0x0C, 0x03, UNIOS_CLS_USB },             /* Intel Panther Point onward */
    { UNIOS_BUS_PCI, 0x1022, UNIOS_PCI_ID_ANY, 0x0C, 0x03, UNIOS_CLS_USB },             /* AMD Promontory/FCH xHCI */
    { UNIOS_BUS_PCI, 0x1B21, UNIOS_PCI_ID_ANY, 0x0C, 0x03, UNIOS_CLS_USB },             /* ASMedia ASM1042/1142/2142 */
    { UNIOS_BUS_PCI, 0x1B6F, UNIOS_PCI_ID_ANY, 0x0C, 0x03, UNIOS_CLS_USB },             /* Renesas uPD72020x */
};
static struct unios_driver xhci_driver = {
    "xhci-hcd", UNIOS_CLS_USB, xhci_table,
    sizeof(xhci_table)/sizeof(xhci_table[0]), usb_probe_class, 0, UNIOS_DRV_STALLED
};

int uhci_register(void) { return unios_driver_register(&uhci_driver); }
int ohci_register(void) { return unios_driver_register(&ohci_driver); }
int ehci_register(void) { return unios_driver_register(&ehci_driver); }
int xhci_register(void) { return unios_driver_register(&xhci_driver); }
