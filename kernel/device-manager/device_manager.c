#include "device_manager.h"

static struct unios_device devices[UNIOS_MAX_DEVICES];
static struct unios_driver *drivers[UNIOS_MAX_DRIVERS];

static uint32_t device_count;
static uint32_t driver_count;

static int id_matches(
    const struct unios_device_id *device,
    const struct unios_driver_match *match
) {
    if (device->bus != match->bus)
        return 0;

    if (match->vendor != 0xffff &&
        device->vendor != match->vendor)
        return 0;

    if (match->device != 0xffff &&
        device->device != match->device)
        return 0;

    if (match->class_code != 0xff &&
        device->class_code != match->class_code)
        return 0;

    if (match->subclass != 0xff &&
        device->subclass != match->subclass)
        return 0;

    if (match->interface != 0xff &&
        device->interface != match->interface)
        return 0;

    return 1;
}

void device_manager_init(void)
{
    device_count = 0;
    driver_count = 0;
}

int device_register(const struct unios_device_id *id, const char *name)
{
    if (!id || device_count >= UNIOS_MAX_DEVICES)
        return -1;

    devices[device_count].device_number = device_count;
    devices[device_count].id = *id;
    devices[device_count].name = name;
    devices[device_count].bound_driver = 0;
    devices[device_count].driver_references = 0;
    devices[device_count].present = 1;

    return (int)device_count++;
}

int driver_register(struct unios_driver *driver)
{
    if (!driver || driver_count >= UNIOS_MAX_DRIVERS)
        return -1;

    driver->state = DRIVER_AVAILABLE;
    driver->references = 0;
    drivers[driver_count++] = driver;

    return 0;
}

int driver_match_device(
    const struct unios_device *device,
    const struct unios_driver *driver
) {
    if (!device || !driver || !device->present)
        return 0;

    for (uint32_t i = 0; i < driver->match_count; i++) {
        if (id_matches(&device->id, &driver->matches[i]))
            return 1;
    }

    return 0;
}

int driver_bind_best(struct unios_device *device)
{
    struct unios_driver *best = 0;
    uint8_t best_priority = 0;

    if (!device || !device->present)
        return -1;

    for (uint32_t i = 0; i < driver_count; i++) {
        struct unios_driver *candidate = drivers[i];

        if (candidate->state != DRIVER_AVAILABLE)
            continue;

        if (!driver_match_device(device, candidate))
            continue;

        for (uint32_t j = 0; j < candidate->match_count; j++) {
            if (id_matches(&device->id, &candidate->matches[j]) &&
                candidate->matches[j].priority >= best_priority) {
                best = candidate;
                best_priority = candidate->matches[j].priority;
            }
        }
    }

    if (!best)
        return -2;

    device->bound_driver = best->name;
    device->driver_references++;
    best->references++;
    best->state = DRIVER_BOUND;

    return 0;
}

int driver_unbind(struct unios_device *device)
{
    if (!device || !device->bound_driver)
        return -1;

    for (uint32_t i = 0; i < driver_count; i++) {
        struct unios_driver *driver = drivers[i];

        if (driver->name[0] == device->bound_driver[0]) {
            if (driver->references > 0)
                driver->references--;

            if (device->driver_references > 0)
                device->driver_references--;

            if (driver->references == 0)
                driver->state = DRIVER_AVAILABLE;

            device->bound_driver = 0;
            return 0;
        }
    }

    return -2;
}

void driver_scan_unused(void)
{
    for (uint32_t i = 0; i < driver_count; i++) {
        struct unios_driver *driver = drivers[i];

        if (driver->builtin || !driver->removable)
            continue;

        if (driver->references == 0 &&
            driver->state == DRIVER_AVAILABLE) {
            driver->state = DRIVER_GARBAGE;
        }
    }
}

void driver_collect_garbage(void)
{
    /*
     * Actual filesystem deletion belongs in userspace or the package
     * manager. The kernel only marks safe-to-remove drivers.
     */
    for (uint32_t i = 0; i < driver_count; i++) {
        struct unios_driver *driver = drivers[i];

        if (driver->state == DRIVER_GARBAGE &&
            driver->references == 0) {
            driver->state = DRIVER_UNUSED;
        }
    }
}
