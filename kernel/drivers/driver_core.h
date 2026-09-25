#ifndef UNIOS_DRIVER_CORE_H
#define UNIOS_DRIVER_CORE_H

#include <stdint.h>

/*
 * UNIOS driver core.
 *
 * Modeled on the Linux driver model (bus / class / driver registration and
 * PCI id tables) but stripped down for a freestanding kernel: no dynamic
 * allocation, no linked lists beyond fixed arrays, no module loader until
 * the ELF path is proven safe. Every driver in the tree registers itself
 * through unios_driver_register() with a match table; the device manager
 * probes enumerated hardware against those tables.
 */

#define UNIOS_PCI_ID_ANY 0xFFFFu

enum unios_bus_type {
    UNIOS_BUS_ISA = 0,   /* legacy ports: VGA text, i8042/PS2, serial, PIT */
    UNIOS_BUS_PCI,       /* enumerated PCI devices matched by vendor/device */
    UNIOS_BUS_CLASS,     /* matched by PCI base class/subclass only */
    UNIOS_BUS_VIRTUAL    /* built-ins with no discoverable hardware id */
};

enum unios_driver_class {
    UNIOS_CLS_DISPLAY = 0,
    UNIOS_CLS_INPUT,
    UNIOS_CLS_STORAGE,
    UNIOS_CLS_NETWORK,
    UNIOS_CLS_WLAN,
    UNIOS_CLS_USB,
    UNIOS_CLS_AUDIO,
    UNIOS_CLS_CHIPSET
};

enum unios_driver_status {
    UNIOS_DRV_STALLED = 0,   /* registered but hardware not present/unsafe */
    UNIOS_DRV_MATCHED,       /* hardware identity verified, not yet probed */
    UNIOS_DRV_ACTIVE         /* probe succeeded, device is usable */
};

struct unios_match_id {
    enum unios_bus_type bus;
    uint16_t vendor_id;      /* UNIOS_PCI_ID_ANY = wildcard */
    uint16_t device_id;      /* UNIOS_PCI_ID_ANY = wildcard */
    uint8_t class_code;      /* PCI base class, ignored for non-PCI buses */
    uint8_t subclass_code;   /* PCI subclass, ignored for non-PCI buses */
    enum unios_driver_class drv_class;
};

struct unios_pci_device; /* forward declaration, see pci.h */

struct unios_driver {
    const char *name;
    enum unios_driver_class drv_class;
    const struct unios_match_id *table;   /* NULL-terminated by vendor==0x0000+device==0x0000 entry or explicit count */
    uint32_t table_size;
    int (*probe)(const struct unios_driver *self, const struct unios_pci_device *dev);
    void (*shutdown)(const struct unios_driver *self);
    enum unios_driver_status status;
};

#define UNIOS_DRIVER_LIMIT 32

int unios_drivers_init(void);
int unios_driver_register(struct unios_driver *drv);
const struct unios_driver *unios_driver_at(uint32_t index);
uint32_t unios_driver_count(void);
/* Called by the device manager after PCI enumeration for each function. */
int unios_driver_bind_pci(const struct unios_pci_device *dev);
/* Run probes for ISA/virtual built-ins (no PCI device argument). */
void unios_drivers_activate_legacy(void);
void unios_drivers_shutdown_all(void);

#endif /* UNIOS_DRIVER_CORE_H */
