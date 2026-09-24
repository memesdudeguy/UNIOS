#ifndef UNIOS_AUTH_H
#define UNIOS_AUTH_H

#include <stdint.h>

#define UNIOS_USERNAME_MAX 32
#define UNIOS_PASSWORD_MAX 128

struct unios_user {
    uint32_t uid;
    uint32_t gid;
    char username[UNIOS_USERNAME_MAX];
    uint8_t administrator;
    uint8_t locked;
};

struct unios_credentials {
    char username[UNIOS_USERNAME_MAX];
    char password[UNIOS_PASSWORD_MAX];
};

int auth_login(
    const char *username,
    const char *password,
    struct unios_user *result
);

int auth_sudo(
    const struct unios_user *user,
    const char *command
);

int auth_switch_user(
    const struct unios_user *current,
    const char *target_username,
    const char *password,
    struct unios_user *result
);

int auth_change_password(
    const struct unios_user *user,
    const char *old_password,
    const char *new_password
);

int auth_is_admin(const struct unios_user *user);

#endif
