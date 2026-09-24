#ifndef UNIOS_DEVICE_MANAGER_H
#define UNIOS_DEVICE_MANAGER_H

#include <stdint.h>

#define UNIOS_MAX_DEVICES 128
#define UNIOS_MAX_DRIVERS 128
#define UNIOS_DRIVER_NAME 32

enum unios_bus {
    UNIOS_BUS_UNKNOWN = 0,
    UNIOS_BUS_PCI,
    UNIOS_BUS_USB,
    UNIOS_BUS_ISA,
    UNIOS_BUS_ACPI
};

enum unios_driver_state {
    DRIVER_UNUSED = 0,
    DRIVER_AVAILABLE,
    DRIVER_BOUND,
    DRIVER_UNLOAD_PENDING,
    DRIVER_GARBAGE
};

struct unios_device_id {
    enum unios_bus bus;
    uint16_t vendor;
    uint16_t device;
    uint16_t subsystem_vendor;
    uint16_t subsystem_device;
    uint8_t class_code;
    uint8_t subclass;
    uint8_t interface;
};

struct unios_device {
    uint32_t device_number;
    struct unios_device_id id;
    const char *name;
    const char *bound_driver;
    uint32_t driver_references;
    uint8_t present;
};

struct unios_driver_match {
    enum unios_bus bus;
    uint16_t vendor;
    uint16_t device;
    uint8_t class_code;
    uint8_t subclass;
    uint8_t interface;
    uint8_t priority;
};

struct unios_driver {
    char name[UNIOS_DRIVER_NAME];
    const struct unios_driver_match *matches;
    uint32_t match_count;
    enum unios_driver_state state;
    uint32_t references;
    uint8_t builtin;
    uint8_t removable;
};

void device_manager_init(void);

int device_register(const struct unios_device_id *id, const char *name);

int driver_register(struct unios_driver *driver);

int driver_match_device(
    const struct unios_device *device,
    const struct unios_driver *driver
);

int driver_bind_best(struct unios_device *device);

int driver_unbind(struct unios_device *device);

void driver_scan_unused(void);

void driver_collect_garbage(void);

#endif
