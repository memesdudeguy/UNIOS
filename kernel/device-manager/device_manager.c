#include "device_manager.h"
#include "../drivers/drivers.h"

int device_manager_init(void) {
    /* Registers every built-in driver (GPU, WiFi, Ethernet, USB, storage,
     * audio, legacy ISA) and binds them against enumerated PCI hardware. */
    unios_drivers_builtin_init();
    return 0;
}

uint32_t device_manager_pci_count(void) { return pci_device_count(); }

const struct unios_pci_device *device_manager_pci_at(uint32_t index) {
    return pci_device_at(index);
}

uint32_t device_manager_driver_count(void) { return unios_driver_count(); }

const struct unios_driver *device_manager_driver_at(uint32_t index) {
    return unios_driver_at(index);
}
