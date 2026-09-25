#include "pci.h"
#include "ioports.h"

#define PCI_CONFIG_ADDR 0xCF8
#define PCI_CONFIG_DATA 0xCFC

/* PCI capability IDs (from the PC spec, same constants Linux uses) */
#define PCI_CAP_ID_PM  0x01
#define PCI_CAP_ID_MSI 0x05

static struct unios_pci_device devices[UNIOS_PCI_MAX_DEVICES];
static uint32_t device_count = 0;
static int pci_available = -1;

static uint32_t make_addr(uint8_t bus, uint8_t slot, uint8_t func, uint8_t off) {
    return 0x80000000u
         | ((uint32_t)bus << 16)
         | ((uint32_t)(slot & 31) << 11)
         | ((uint32_t)(func & 7) << 8)
         | (off & 0xFCu);
}

uint32_t pci_config_read32(uint8_t bus, uint8_t slot, uint8_t func, uint8_t off) {
    outl(PCI_CONFIG_ADDR, make_addr(bus, slot, func, off));
    return inl(PCI_CONFIG_DATA);
}

void pci_config_write32(uint8_t bus, uint8_t slot, uint8_t func, uint8_t off, uint32_t value) {
    outl(PCI_CONFIG_ADDR, make_addr(bus, slot, func, off));
    outl(PCI_CONFIG_DATA, value);
}

uint16_t pci_config_read16(uint8_t bus, uint8_t slot, uint8_t func, uint8_t off) {
    uint32_t v = pci_config_read32(bus, slot, func, off & ~3u);
    return (uint16_t)(v >> ((off & 2u) * 8u));
}

int pci_present(void) {
    uint32_t probe;
    if (pci_available >= 0) return pci_available;
    /*
     * Mechanism #1 sanity check like Linux pci_check_type(): read a known
     * function that must exist on any QEMU/most bare-metal chipsets
     * (bus 0 dev 0 = host bridge). All-ones or all-zeros means no PCI.
     */
    outl(PCI_CONFIG_ADDR, 0);
    probe = pci_config_read32(0, 0, 0, PCI_VENDOR_ID);
    pci_available = (probe != 0xFFFFFFFFu && probe != 0) ? 1 : 0;
    return pci_available;
}

uint8_t pci_find_capability(const struct unios_pci_device *dev, uint8_t cap_id) {
    uint8_t off;
    uint16_t status;
    uint16_t guard = 0;

    status = pci_config_read16(dev->bus, dev->slot, dev->func, PCI_STATUS);
    if (!(status & 0x10)) return 0; /* no capabilities list */

    off = (uint8_t)(pci_config_read32(dev->bus, dev->slot, dev->func, 0x34) & 0xFFu);
    while (off >= 0x40u && off < 0xFFu && guard++ < 48u) {
        uint32_t entry = pci_config_read32(dev->bus, dev->slot, dev->func, off);
        if ((uint8_t)(entry & 0xFFu) == cap_id) return off;
        off = (uint8_t)((entry >> 8) & 0xF8u); /* next pointer, dword aligned */
    }
    return 0;
}

static void read_bars(struct unios_pci_device *dev) {
    for (int i = 0; i < 6; ++i) {
        uint32_t raw = pci_config_read32(dev->bus, dev->slot, dev->func,
                                         (uint8_t)(PCI_BAR0 + 4u * i));
        if (raw & 0x1u) {
            dev->bar[i] = raw & ~0x3u;          /* I/O space BAR */
        } else {
            dev->bar[i] = raw & ~0xFu;          /* memory BAR, mask type bits */
        }
    }
}

int pci_enumerate(void) {
    device_count = 0;
    if (!pci_present()) return -1;

    for (uint32_t bus = 0; bus < 256u; ++bus) {
        uint16_t vid = pci_config_read16((uint8_t)bus, 0, 0, PCI_VENDOR_ID);
        if (vid == 0xFFFFu) continue; /* this bus does not exist */

        for (uint32_t slot = 0; slot < 32u; ++slot) {
            uint32_t vd = pci_config_read32((uint8_t)bus, (uint8_t)slot, 0, PCI_VENDOR_ID);
            if ((vd & 0xFFFFu) == 0xFFFFu || vd == 0) continue;

            uint8_t hdr = (uint8_t)((pci_config_read32((uint8_t)bus, (uint8_t)slot, 0, 0x0C) >> 16) & 0xFFu);
            uint32_t max_func = (hdr & 0x80u) ? 8u : 1u;

            for (uint32_t func = 0; func < max_func; ++func) {
                uint32_t vdf = pci_config_read32((uint8_t)bus, (uint8_t)slot, (uint8_t)func, PCI_VENDOR_ID);
                if ((vdf & 0xFFFFu) == 0xFFFFu || vdf == 0) continue;
                if (device_count >= UNIOS_PCI_MAX_DEVICES) return (int)device_count;

                struct unios_pci_device *dev = &devices[device_count++];
                uint32_t clsrev = pci_config_read32((uint8_t)bus, (uint8_t)slot, (uint8_t)func, PCI_REVISION);
                dev->bus = (uint8_t)bus;
                dev->slot = (uint8_t)slot;
                dev->func = (uint8_t)func;
                dev->vendor_id = (uint16_t)(vdf & 0xFFFFu);
                dev->device_id = (uint16_t)(vdf >> 16);
                dev->prog_if = (uint8_t)(clsrev & 0xFFu);
                dev->subclass_code = (uint8_t)((clsrev >> 8) & 0xFFu);
                dev->class_code = (uint8_t)((clsrev >> 16) & 0xFFu);
                dev->header_type = (uint8_t)((clsrev >> 24) & 0xFFu);
                dev->irq_line = (uint8_t)(pci_config_read32((uint8_t)bus, (uint8_t)slot, (uint8_t)func, PCI_INTERRUPT_LINE) & 0xFFu);
                read_bars(dev);
                dev->has_pm_cap = pci_find_capability(dev, PCI_CAP_ID_PM) ? 1 : 0;
                dev->msi_cap_off = pci_find_capability(dev, PCI_CAP_ID_MSI);

                /* Enable I/O + memory decoding so BAR drivers can touch the
                 * device, mirroring what Linux does at probe time. */
                uint32_t cmd = pci_config_read32(dev->bus, dev->slot, dev->func, PCI_COMMAND);
                pci_config_write32(dev->bus, dev->slot, dev->func, PCI_COMMAND, cmd | 0x3u);
            }
        }
    }
    return (int)device_count;
}

uint32_t pci_device_count(void) { return device_count; }

const struct unios_pci_device *pci_device_at(uint32_t index) {
    if (index >= device_count) return 0;
    return &devices[index];
}
