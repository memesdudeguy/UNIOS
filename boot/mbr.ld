SECTIONS
{
    . = 0x7c00;
    .text : { *(.text) }
    .rodata : { *(.rodata) }
    /DISCARD/ : { *(.comment) *(.note*) *(.eh_frame) }

    /* The BIOS signature must sit at offset 510 of the 512-byte image. */
    .mbrsig 0x7dfe : ALIGN(1)
    {
        KEEP(*(.mbrsig))
    }
}
ASSERT(SIZEOF(.mbrsig) == 2, ".mbrsig must be exactly 2 bytes")
