#ifndef ALIEN_DRIVER_API_H
#define ALIEN_DRIVER_API_H

#include <stdint.h>

void alien_log(const char *message);

void *alien_alloc(uint32_t bytes);
void alien_free(void *address);

void *alien_mmio_map(uintptr_t physical, uint32_t length);
void alien_mmio_unmap(void *address, uint32_t length);

int alien_irq_register(
    uint8_t irq,
    void (*handler)(void *context),
    void *context
);

void alien_irq_unregister(uint8_t irq);

#endif
