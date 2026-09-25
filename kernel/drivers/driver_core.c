#include "driver_core.h"
#include "pci.h"

/*
 * Driver registry. Fixed-size array, insertion order preserved so the
 * 'drivers' shell command output is stable. Modeled on Linux's
 * driver_register()/bus_for_each_dev() pairing but without kobjects.
 */

static struct unios_driver *registry[UNIOS_DRIVER_LIMIT];
static uint32_t registry_count = 0;

int unios_drivers_init(void) {
    registry_count = 0;
    return 0;
}

int unios_driver_register(struct unios_driver *drv) {
    if (!drv || !drv->table || drv->table_size == 0) return -1;
    if (registry_count >= UNIOS_DRIVER_LIMIT) return -2;
    drv->status = UNIOS_DRV_STALLED;
    registry[registry_count++] = drv;
    return 0;
}

const struct unios_driver *unios_driver_at(uint32_t index) {
    if (index >= registry_count) return 0;
    return registry[index];
}

uint32_t unios_driver_count(void) { return registry_count; }

static int match_one(const struct unios_match_id *m, const struct unios_pci_device *dev) {
    if (m->bus != UNIOS_BUS_PCI && m->bus != UNIOS_BUS_CLASS) return 0;
    if (m->class_code != dev->class_code) return 0;
    if (m->subclass_code != 0x00 && m->subclass_code != dev->subclass_code) return 0;
    if (m->vendor_id != UNIOS_PCI_ID_ANY && m->vendor_id != dev->vendor_id) return 0;
    if (m->device_id != UNIOS_PCI_ID_ANY && m->device_id != dev->device_id) return 0;
    return 1;
}

int unios_driver_bind_pci(const struct unios_pci_device *dev) {
    if (!dev) return -1;
    for (uint32_t i = 0; i < registry_count; ++i) {
        struct unios_driver *drv = registry[i];
        if (drv->status != UNIOS_DRV_STALLED) continue; /* already bound elsewhere */
        for (uint32_t j = 0; j < drv->table_size; ++j) {
            if (!match_one(&drv->table[j], dev)) continue;
            drv->status = UNIOS_DRV_MATCHED;
            if (drv->probe && drv->probe(drv, dev) == 1) {
                drv->status = UNIOS_DRV_ACTIVE;
            }
            return (int)i;
        }
    }
    return -1;
}

void unios_drivers_activate_legacy(void) {
    for (uint32_t i = 0; i < registry_count; ++i) {
        struct unios_driver *drv = registry[i];
        const struct unios_match_id *m = &drv->table[0];
        if (m->bus != UNIOS_BUS_ISA && m->bus != UNIOS_BUS_VIRTUAL) continue;
        if (drv->status != UNIOS_DRV_STALLED || !drv->probe) continue;
        if (drv->probe(drv, 0) == 1) drv->status = UNIOS_DRV_ACTIVE;
    }
}

void unios_drivers_shutdown_all(void) {
    for (uint32_t i = registry_count; i > 0; --i) {
        struct unios_driver *drv = registry[i - 1];
        if (drv->status == UNIOS_DRV_ACTIVE && drv->shutdown) {
            drv->shutdown(drv);
        }
        drv->status = UNIOS_DRV_STALLED;
    }
}
