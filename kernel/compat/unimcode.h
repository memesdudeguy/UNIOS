#ifndef UNIOS_UNIMCODE_H
#define UNIOS_UNIMCODE_H

#include <stdint.h>

enum unimcode_arch {
    UNIMCODE_ARCH_UNKNOWN = 0,
    UNIMCODE_ARCH_I386,
    UNIMCODE_ARCH_X86_64,
    UNIMCODE_ARCH_ARM32,
    UNIMCODE_ARCH_AARCH64,
    UNIMCODE_ARCH_RISCV64
};

enum unimcode_result {
    UNIMCODE_OK = 0,
    UNIMCODE_BAD_FORMAT = -1,
    UNIMCODE_UNSUPPORTED_ARCH = -2,
    UNIMCODE_INVALID_INSTRUCTION = -3,
    UNIMCODE_TRANSLATION_FAILED = -4,
    UNIMCODE_MEMORY_ERROR = -5
};

struct unimcode_image {
    enum unimcode_arch guest_arch;
    uint32_t guest_entry;
    uint32_t guest_base;
    uint32_t guest_size;
    uint32_t native_entry;
    uint8_t translated;
};

int unimcode_init(void);
enum unimcode_arch unimcode_detect_arch(const uint8_t *image, uint32_t size);
int unimcode_load(struct unimcode_image *image, const uint8_t *data, uint32_t size);
int unimcode_translate_block(struct unimcode_image *image, uint32_t guest_pc, uint32_t *native_pc);
void unimcode_release(struct unimcode_image *image);

#endif
