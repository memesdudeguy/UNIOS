.section .text
.code16

.globl _start
_start:
    cli
    xorw %ax, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    movw $0x7c00, %sp

    movw $disk_address_packet, %si
    movw $0x42, %ax
    movw $0x80, %dx
    int $0x13
    jc disk_error

    cli
    lgdt gdt_descriptor
    movl %cr0, %eax
    orl $0x1, %eax
    movl %eax, %cr0
    ljmp $0x08, $protected_mode

disk_error:
    movb $'E', %al
    movb $0x0e, %ah
    int $0x10
1:
    cli
    hlt
    jmp 1b

.code32
protected_mode:
    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    movw %ax, %fs
    movw %ax, %gs
    movl $0x90000, %esp

    # The kernel is loaded at physical address 0x10000.
    movl $0x10000, %eax
    call *%eax
2:
    cli
    hlt
    jmp 2b

.align 4
disk_address_packet:
    .byte 0x10, 0x00
    .word 64
    .word 0x0000
    .word 0x1000
    .quad 1

gdt:
    .quad 0x0000000000000000
    .quad 0x00cf9a000000ffff
    .quad 0x00cf92000000ffff
gdt_descriptor:
    .word gdt_descriptor - gdt - 1
    .long gdt
