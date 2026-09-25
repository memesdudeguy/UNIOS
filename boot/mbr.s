/* UNIOS stage-1 boot sector (MBR).
 *
 * Runs in 16-bit real mode at 0x7c00. Loads build/kernel.bin from disk
 * sectors 2..N into physical memory at 0x10000 using BIOS INT 13h, AH=42h
 * (extended read), enables A20 via the fast gate, loads a tiny GDT, then
 * far-jumps into 32-bit protected mode and transfers control to _start of
 * the loaded image (boot/boot.s -> kernel_main).
 *
 * Everything lives in .text so the whole file links as one contiguous
 * 510-byte blob; the .mbrsig section supplies bytes 510..511.
 */

.section .text
.code16
.globl _start
_start:
    cli
    cld
    xorw %ax, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    movw $0x7c00, %sp        /* valid 16-bit stack inside our own segment */

    /* ---- Load kernel: LBA 1, 64 sectors -> linear 0x10000 ---- */
    movb $0x80, %dl          /* first hard disk (QEMU IDE) */
    movw $dap, %si
    movw $0x42, %ax
    int $0x13
    jc disk_error

    /* ---- Enable A20 via fast gate (port 0x92) ---- */
    inb $0x92, %al
    orb $0x02, %al
    outb %al, $0x92

    /* ---- Enter protected mode ---- */
    cli
    lgdt [gdt_descriptor]    /* memory operand (brackets required in -m16;
                              * without them clang emits a broken 3-byte
                              * form that loads the wrong GDT base) */
    movl %cr0, %eax
    orl  $0x01, %eax         /* PE */
    movl %eax, %cr0
    ljmp $0x08, $protected_mode

.code32
protected_mode:
    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    movw %ax, %fs
    movw %ax, %gs
    movl $0x9000, %esp       /* scratch stack below the kernel image */

    /* Transfer control to the loaded kernel at physical 0x10000.
     * A memory-indirect FAR JUMP (opcode FF /5) is used deliberately:
     * clang's `ljmp $sel, $imm` form under -m16 emits a broken 32-bit
     * far-pointer immediate (selector lands in the high half of the
     * offset), which jumps into garbage and trips SeaBIOS' triple-fault
     * reboot handler — the classic "boot from hard disk" loop. */
    ljmp *pm_kernel_ptr

.align 4
pm_kernel_ptr:
    .long 0x10000            /* EIP = kernel entry (_start in boot/boot.s) */
    .word 0x0008             /* CS  = flat code32 selector from our GDT */

disk_error:
    movb $'D', %al           /* print 'D' via BIOS TTY on disk failure */
    movb $0x0e, %ah
    int $0x10
halt_loop:
    cli
    hlt
    jmp halt_loop

/* ---- data (must fit within the 512-byte sector) ---- */
.align 4
dap:
    .byte 0x10               /* packet size */
    .byte 0
    .word 64                 /* sector count (KERNEL_SECTORS) */
    .word 0x0000             /* transfer offset */
    .word 0x1000             /* transfer segment -> linear 0x10000 */
    .quad 1                  /* starting LBA (sector after MBR) */

/* GDT: null, code32 (0x08), data32 (0x10) */
.align 8
gdt_start:
    .quad 0x0000000000000000
    .quad 0x00cf9a000000ffff
    .quad 0x00cf92000000ffff
gdt_end:
gdt_descriptor:
    .word gdt_end - gdt_start - 1
    .long gdt_start

.section .mbrsig, "a"
.align 1
.byte 0x55, 0xaa
