#ifndef UNIOS_PCI_H
#define UNIOS_PCI_H

#include <stdint.h>

/*
 * Minimal PCI enumeration (config mechanism #1), modeled on the way Linux
 * walks buses in drivers/pci/probe.c, but fixed-size and allocation-free.
 */

#define UNIOS_PCI_MAX_DEVICES 32

/* Standard config-space offsets */
#define PCI_VENDOR_ID   0x00
#define PCI_DEVICE_ID   0x02
#define PCI_COMMAND     0x04
#define PCI_STATUS      0x06
#define PCI_REVISION    0x08
#define PCI_PROG_IF     0x09
#define PCI_SUBCLASS    0x0A
#define PCI_CLASS       0x0B
#define PCI_CACHE_LINE  0x0C
#define PCI_HEADER_TYPE 0x0E
#define PCI_SUBSYSTEM_VENDOR 0x2C
#define PCI_SUBSYSTEM_ID     0x2E
#define PCI_BAR0        0x10
#define PCI_INTERRUPT_LINE 0x3C

struct unios_pci_device {
    uint8_t bus;
    uint8_t slot;      /* 0-31 */
    uint8_t func;      /* 0-7 */
    uint16_t vendor_id;
    uint16_t device_id;
    uint8_t class_code;
    uint8_t subclass_code;
    uint8_t prog_if;
    uint8_t header_type;
    uint8_t irq_line;
    uint32_t bar[6];
    uint8_t has_pm_cap;    /* power-management capability present */
    uint8_t msi_cap_off;   /* MSI capability offset, 0 if absent */
};

int pci_present(void);                 /* sanity-probe config space */
int pci_enumerate(void);               /* scan bus 0..255, fill table */
uint32_t pci_device_count(void);
const struct unios_pci_device *pci_device_at(uint32_t index);
uint32_t pci_config_read32(uint8_t bus, uint8_t slot, uint8_t func, uint8_t off);
void pci_config_write32(uint8_t bus, uint8_t slot, uint8_t func, uint8_t off, uint32_t value);
uint16_t pci_config_read16(uint8_t bus, uint8_t slot, uint8_t func, uint8_t off);
/* Walk the capability list looking for Linux-defined PCI cap IDs. */
uint8_t pci_find_capability(const struct unios_pci_device *dev, uint8_t cap_id);

#endif /* UNIOS_PCI_H */
