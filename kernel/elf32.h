#ifndef UNIOS_ELF32_H
#define UNIOS_ELF32_H

#include <stdint.h>

#define ELF32_MAGIC_0 0x7f
#define ELF32_MAGIC_1 'E'
#define ELF32_MAGIC_2 'L'
#define ELF32_MAGIC_3 'F'
#define ELF32_CLASS_32 1
#define ELF32_DATA_LSB 1
#define ELF32_EM_386 3
#define ELF32_EM_X86_64 62

enum elf32_status {
    ELF32_OK = 0,
    ELF32_BAD_MAGIC = -1,
    ELF32_BAD_CLASS = -2,
    ELF32_BAD_ENDIAN = -3,
    ELF32_UNSUPPORTED_MACHINE = -4,
    ELF32_BAD_SIZE = -5,
    ELF32_BAD_PROGRAM_HEADERS = -6,
    ELF32_BAD_ENTRY = -7,
    ELF32_BAD_OVERFLOW = -8,
    ELF32_NOT_READY = -9
};

struct elf32_image {
    uint32_t entry_point;
    uint32_t text_base;
    uint32_t text_size;
    uint32_t data_base;
    uint32_t data_size;
    uint8_t valid;
};

int elf32_validate(const void *image, uint32_t size);
int elf32_load(struct elf32_image *info, const void *image, uint32_t size);

#endif
