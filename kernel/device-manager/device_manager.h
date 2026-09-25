#ifndef UNIOS_DEVICE_MANAGER_H
#define UNIOS_DEVICE_MANAGER_H

#include <stdint.h>
#include "../drivers/driver_core.h"

/*
 * The device manager now wraps the driver registry + PCI enumeration table
 * instead of keeping its own duplicate match list. Older hand-rolled
 * register/match API removed as not needed.
 */

int device_manager_init(void);
uint32_t device_manager_pci_count(void);
const struct unios_pci_device *device_manager_pci_at(uint32_t index);
uint32_t device_manager_driver_count(void);
const struct unios_driver *device_manager_driver_at(uint32_t index);

#endif
