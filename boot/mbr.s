/* UNIOS stage-1 boot sector (MBR).
 *
 * Runs in 16-bit real mode at 0x7c00. Loads build/kernel.bin from disk
 * sectors 2..N into physical memory at 0x10000 using BIOS INT 13h, AH=42h
 * (extended read), enables A20 via the fast gate, loads a tiny GDT, then
 * enters 32-bit protected mode and jumps to _start of the loaded image
 * (boot/boot.s -> kernel_main).
 *
 * IMPORTANT: this file is written as explicit raw instruction encodings
 * (.byte directives) on purpose. clang's integrated assembler with -m16
 * mis-assembles several instructions here (it drops the 0x66 operand-size
 * prefix on `movw $imm16, %reg`, emitting e.g. `bc 00 7c b2 80` instead of
 * `66 bc 00 7c`), which corrupts %esp/%esi, makes the INT 13h disk read
 * fault, and trips SeaBIOS' reboot handler — the classic "booting from
 * hard disk..." loop. Raw bytes are immune to assembler quirks. Each
 * encoding is annotated with the mnemonic it implements.
 *
 * Layout note: everything must stay below offset 510 because the linker
 * script places .mbrsig at 0x7dfe (no room for padding beyond 512 bytes).
 */

.section .text
.code16
.globl _start
_start:
    .byte 0xfa                          /* cli                                    */
    .byte 0xfc                          /* cld                                    */
    .byte 0x31, 0xc0                    /* xor    %ax,%ax                         */
    .byte 0x8e, 0xd8                    /* mov    %ax,%ds                         */
    .byte 0x8e, 0xc0                    /* mov    %ax,%es                         */
    .byte 0x8e, 0xd0                    /* mov    %ax,%ss                         */
    .byte 0x66, 0xbc, 0x00, 0x7c, 0x00, 0x00  /* movabs $0x7c00,%esp (16-bit opsize) */

    /* ---- Load kernel: LBA 1, 64 sectors -> linear 0x10000 (ES:BX) ---- */
    .byte 0xb8, 0x00, 0x10              /* mov    $0x1000,%ax   (kernel dst seg)  */
    .byte 0x8e, 0xc0                    /* mov    %ax,%es                        */
    .byte 0xbb, 0x00, 0x00              /* mov    $0,%bx        (kernel dst off)  */
    .byte 0xba, 0x80, 0x00              /* mov    $0x80,%dx     (first HDD)       */
    .byte 0xbe
    .word dap                           /* mov    $dap,%si  (relocated to 0x7c00+) */
    .byte 0xb8, 0x42, 0x00              /* mov    $0x42,%ax   (AH=42 extended read) */
    .byte 0xcd, 0x13                    /* int    $0x13                           */
    .byte 0x72, 0x06                    /* jc     disk_error  (+6 bytes)          */

    /* ---- Enable A20 via fast gate (port 0x92) ---- */
    .byte 0xe4, 0x92                    /* in     $0x92,%al                       */
    .byte 0x0c, 0x02                    /* or     $0x02,%al                       */
    .byte 0xe6, 0x92                    /* out    %al,$0x92                       */

    /* ---- Enter protected mode ---- */
    .byte 0xfa                          /* cli                                    */
    .byte 0x0f, 0x01, 0x16              /* lgdt   gdt_descriptor (mem operand)     */
    .word gdt_descriptor                /*   16-bit absolute address of the desc.  */
    .byte 0x0f, 0x20, 0xc0              /* mov    %cr0,%eax                       */
    .byte 0x66, 0x83, 0xc8, 0x01        /* or     $0x01,%ax   (PE bit)            */
    .byte 0x0f, 0x22, 0xc0              /* mov    %eax,%cr0                       */
    /* far jump: ea <offset:16> <selector:16> — reload CS with 0x08 */
    .byte 0xea
    .word pm_entry - . + 0x7c00         /*   offset = protected-mode entry (reloc) */
    .word 0x0008                        /*   selector = flat code32                */

/* ---- 32-bit protected mode ---- */
.code32
pm_entry:
    .byte 0xb8, 0x10, 0x00, 0x00, 0x00  /* mov    $0x10,%eax  (flat data sel)     */
    .byte 0x8e, 0xd8                    /* mov    %eax,%ds                        */
    .byte 0x8e, 0xc0                    /* mov    %eax,%es                        */
    .byte 0x8e, 0xd0                    /* mov    %eax,%ss                        */
    .byte 0x8e, 0xe0                    /* mov    %eax,%fs                        */
    .byte 0x8e, 0xe8                    /* mov    %eax,%gs                        */
    .byte 0xbc, 0x00, 0x90, 0x00, 0x00  /* mov    $0x9000,%esp (scratch stack)    */
    /* Jump to the loaded kernel at physical 0x10000 (linked there; see
     * kernel/linker.ld). Direct absolute jump keeps CS = 0x08 flat code. */
    .byte 0xb8, 0x00, 0x00, 0x01, 0x00  /* mov    $0x10000,%eax                   */
    .byte 0xff, 0xe0                    /* jmp    *%eax  -> _start in boot/boot.s */

/* On-disk read failure: print 'D' through the BIOS TTY, then halt. */
disk_error:
    .byte 0xb0, 'D'                     /* mov    $'D',%al                        */
    .byte 0xb4, 0x0e                    /* mov    $0x0e,%ah   (teletype output)   */
    .byte 0xcd, 0x10                    /* int    $0x10                           */
halt_loop:
    .byte 0xfa                          /* cli                                    */
    .byte 0xf4                          /* hlt                                    */
    .byte 0xeb, 0xfe                    /* jmp    halt_loop  (-2)                 */

/* ---- data (kept before offset 510 by the layout above) ---- */
/* Disk Address Packet for INT 13h AH=42h */
.align 4
dap:
    .byte 0x10                          /* packet size                            */
    .byte 0                             /* reserved                               */
    .word 64                            /* sector count (KERNEL_SECTORS)          */
    .word 0x0000                        /* transfer buffer offset (BX)            */
    .word 0x1000                        /* transfer buffer segment (ES) -> 0x10000 */
    .quad 1                             /* starting LBA (sector after the MBR)    */

/* GDT: null, code32 (0x08), data32 (0x10) */
.align 8
gdt_start:
    .quad 0x0000000000000000
    .quad 0x00cf9a000000ffff
    .quad 0x00cf92000000ffff
gdt_end:
.align 2
gdt_descriptor:
    .word gdt_end - gdt_start - 1       /* limit (bytes)                          */
    .long gdt_start                     /* 32-bit base address                    */

.section .mbrsig, "a"
.align 1
.byte 0x55, 0xaa                        /* BIOS boot signature (offsets 510/511)  */
