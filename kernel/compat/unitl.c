#include "unitl.h"

static int unitl_valid_user_range(uint32_t address, uint32_t length) {
    if (address >= 0xC0000000U) {
        return 0;
    }
    if (length > 0xC0000000U - address) {
        return 0;
    }
    return 1;
}

int unitl_init(void) {
    return 0;
}

int32_t unitl_dispatch(
    uint32_t syscall_number,
    uint32_t arg0,
    uint32_t arg1,
    uint32_t arg2,
    uint32_t arg3,
    uint32_t arg4,
    uint32_t arg5
) {
    (void)arg3;
    (void)arg4;
    (void)arg5;

    switch (syscall_number) {
    case UNITL_SYS_WRITE:
        if (arg0 != 1 && arg0 != 2) {
            return -UNITL_EBADF;
        }
        if (!unitl_valid_user_range(arg1, arg2)) {
            return -UNITL_EFAULT;
        }
        return -UNITL_ENOSYS;
    case UNITL_SYS_GETPID:
        return 2;
    case UNITL_SYS_UNAME:
        if (!unitl_valid_user_range(arg0, 65)) {
            return -UNITL_EFAULT;
        }
        return -UNITL_ENOSYS;
    case UNITL_SYS_EXIT:
        return -UNITL_ENOSYS;
    default:
        return -UNITL_ENOSYS;
    }
}
