#ifndef UNIOS_ALIEN_DRIVER_API_H
#define UNIOS_ALIEN_DRIVER_API_H

#include <stdint.h>

enum unios_driver_policy {
    UNIOS_POLICY_DISABLED = 0,
    UNIOS_POLICY_MATCH_ONLY = 1,
    UNIOS_POLICY_ACTIVE = 2
};

struct unios_driver_descriptor {
    const char *name;
    uint16_t vendor_id;
    uint16_t device_id;
    uint8_t class_code;
    uint8_t subclass_code;
    enum unios_driver_policy policy;
};

#endif
