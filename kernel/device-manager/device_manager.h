#ifndef UNIOS_DEVICE_MANAGER_H
#define UNIOS_DEVICE_MANAGER_H

#include <stdint.h>

enum unios_driver_state {
    UNIOS_DRIVER_DISABLED = 0,
    UNIOS_DRIVER_MATCHED = 1,
    UNIOS_DRIVER_ACTIVE = 2
};

struct unios_device_match {
    uint16_t vendor_id;
    uint16_t device_id;
    uint8_t class_code;
    uint8_t subclass_code;
    enum unios_driver_state state;
};

int device_manager_init(void);
int device_manager_register_match(const struct unios_device_match *match);
int device_manager_match_device(uint16_t vendor_id, uint16_t device_id, uint8_t class_code, uint8_t subclass_code);

#endif
