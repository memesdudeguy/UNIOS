SECTIONS
{
    . = 0x7c00;
    .text : { *(.text) }
    .rodata : { *(.rodata) }
    . = 0x7dfe;
    SHORT(0xaa55)
}
