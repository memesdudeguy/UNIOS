#include "device_manager.h"

#define UNIOS_DRIVER_LIMIT 16

static struct unios_device_match registry[UNIOS_DRIVER_LIMIT];
static int registry_count = 0;

int device_manager_init(void) {
    registry_count = 0;
    return 0;
}

int device_manager_register_match(const struct unios_device_match *match) {
    if (!match || registry_count >= UNIOS_DRIVER_LIMIT) {
        return -1;
    }
    registry[registry_count++] = *match;
    return 0;
}

int device_manager_match_device(uint16_t vendor_id, uint16_t device_id, uint8_t class_code, uint8_t subclass_code) {
    for (int i = 0; i < registry_count; ++i) {
        if (registry[i].vendor_id == vendor_id && registry[i].device_id == device_id) {
            return registry[i].state;
        }
        if (registry[i].class_code == class_code && registry[i].subclass_code == subclass_code) {
            return registry[i].state;
        }
    }
    return UNIOS_DRIVER_DISABLED;
}
