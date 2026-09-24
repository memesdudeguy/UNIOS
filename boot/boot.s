global _start

_start:
    cli
    mov $stack_top, %esp
    call kernel_main
    hlt
    jmp .

.section .bss
.align 16
stack_bottom:
    .skip 65536
stack_top:
