#ifndef UNIOS_UNITL_H
#define UNIOS_UNITL_H

#include <stdint.h>

enum unitl_syscall {
    UNITL_SYS_EXIT = 1,
    UNITL_SYS_WRITE = 4,
    UNITL_SYS_READ = 3,
    UNITL_SYS_OPENAT = 295,
    UNITL_SYS_CLOSE = 6,
    UNITL_SYS_FSTAT = 108,
    UNITL_SYS_BRK = 45,
    UNITL_SYS_MMAP2 = 192,
    UNITL_SYS_MUNMAP = 91,
    UNITL_SYS_UNAME = 122,
    UNITL_SYS_GETPID = 20,
    UNITL_SYS_CLOCK_GETTIME = 265
};

enum unitl_errno {
    UNITL_EPERM = 1,
    UNITL_ENOENT = 2,
    UNITL_EIO = 5,
    UNITL_EBADF = 9,
    UNITL_EACCES = 13,
    UNITL_EFAULT = 14,
    UNITL_EINVAL = 22,
    UNITL_ENOSYS = 38,
    UNITL_ENOMEM = 12
};

struct unitl_result {
    int32_t value;
    int32_t error;
};

int unitl_init(void);

int32_t unitl_dispatch(
    uint32_t syscall_number,
    uint32_t arg0,
    uint32_t arg1,
    uint32_t arg2,
    uint32_t arg3,
    uint32_t arg4,
    uint32_t arg5
);

#endif
