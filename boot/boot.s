.section .text
.code32

.globl _start
.extern kernel_main

_start:
    cli
    movl $stack_top, %esp
    andl $-16, %esp
    call kernel_main

1:
    hlt
    jmp 1b

.section .bss
.align 16
stack_bottom:
    .skip 65536
stack_top:
