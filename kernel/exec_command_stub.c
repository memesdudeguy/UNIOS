#include <stdint.h>

#include "elf32.h"
#include "process.h"

static void command_exec(const char *arg) {
    struct elf32_image image;
    const uint8_t *dummy = (const uint8_t *)0x100000U;

    if (!arg || arg[0] == '\0') {
        terminal_write("exec: missing path\n");
        return;
    }

    if (elf32_validate(dummy, 52U) != ELF32_OK) {
        terminal_write("exec: ELF32 validation failed or image is not in memory yet\n");
        return;
    }

    if (elf32_load(&image, dummy, 52U) != ELF32_OK) {
        terminal_write("exec: unable to prepare ELF32 image\n");
        return;
    }

    terminal_write("exec: ELF32 loader ready for static x86 binaries\n");
    terminal_write("exec: dynamic or foreign ISAs are rejected until fully implemented\n");
    process_add(arg, image.entry_point);
}
