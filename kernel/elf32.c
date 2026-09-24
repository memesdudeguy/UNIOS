#include "elf32.h"

static int elf32_has_valid_header(const uint8_t *image, uint32_t size) {
    if (!image || size < 52U) {
        return 0;
    }
    if (image[0] != ELF32_MAGIC_0 || image[1] != ELF32_MAGIC_1 ||
        image[2] != ELF32_MAGIC_2 || image[3] != ELF32_MAGIC_3) {
        return 0;
    }
    if (image[4] != ELF32_CLASS_32) {
        return 0;
    }
    if (image[5] != ELF32_DATA_LSB) {
        return 0;
    }
    return 1;
}

int elf32_validate(const void *image, uint32_t size) {
    const uint8_t *bytes = (const uint8_t *)image;
    uint16_t machine;

    if (!image || size < 52U) {
        return ELF32_BAD_SIZE;
    }
    if (!elf32_has_valid_header(bytes, size)) {
        return ELF32_BAD_MAGIC;
    }

    machine = (uint16_t)bytes[18] | ((uint16_t)bytes[19] << 8);
    if (machine != ELF32_EM_386) {
        return ELF32_UNSUPPORTED_MACHINE;
    }

    return ELF32_OK;
}

int elf32_load(struct elf32_image *info, const void *image, uint32_t size) {
    const uint8_t *bytes = (const uint8_t *)image;
    uint32_t entry;
    uint32_t phoff;
    uint16_t phentsize;
    uint16_t phnum;

    if (!info) {
        return ELF32_NOT_READY;
    }

    if (elf32_validate(image, size) != ELF32_OK) {
        return ELF32_BAD_MAGIC;
    }

    entry = (uint32_t)bytes[24] |
            ((uint32_t)bytes[25] << 8) |
            ((uint32_t)bytes[26] << 16) |
            ((uint32_t)bytes[27] << 24);

    phoff = (uint32_t)bytes[28] |
            ((uint32_t)bytes[29] << 8) |
            ((uint32_t)bytes[30] << 16) |
            ((uint32_t)bytes[31] << 24);

    phentsize = (uint16_t)bytes[42] | ((uint16_t)bytes[43] << 8);
    phnum = (uint16_t)bytes[44] | ((uint16_t)bytes[45] << 8);

    if (phoff == 0U || phnum == 0U || phentsize == 0U) {
        return ELF32_BAD_PROGRAM_HEADERS;
    }
    if (entry == 0U || entry >= 0xC0000000U) {
        return ELF32_BAD_ENTRY;
    }
    if (phoff + (uint32_t)phnum * (uint32_t)phentsize > size) {
        return ELF32_BAD_PROGRAM_HEADERS;
    }

    info->entry_point = entry;
    info->text_base = 0x100000U;
    info->text_size = 0;
    info->data_base = 0x100000U;
    info->data_size = 0;
    info->valid = 1U;
    return ELF32_OK;
}
