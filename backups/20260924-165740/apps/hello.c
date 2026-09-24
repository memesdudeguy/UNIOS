static const char message[] = "hello from a Linux ELF process\n";

void _start(void)
{
    __asm__ volatile (
        "movl $4, %%eax\n"       /* Linux i386 sys_write */
        "movl $1, %%ebx\n"       /* stdout */
        "movl %0, %%ecx\n"
        "movl $31, %%edx\n"      /* message length */
        "int $0x80\n"

        "movl $1, %%eax\n"       /* Linux i386 sys_exit */
        "xorl %%ebx, %%ebx\n"
        "int $0x80\n"
        :
        : "r"(message)
        : "eax", "ebx", "ecx", "edx", "memory"
    );

    for (;;) {
        __asm__ volatile ("hlt");
    }
}
