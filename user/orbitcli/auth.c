#include "auth.h"

/*
 * Authentication must eventually call the protected UNIOS identity service.
 *
 * Never store plaintext passwords.
 * Store only a salted password hash.
 * Password verification must happen outside the command parser.
 */

int auth_is_admin(const struct unios_user *user)
{
    return user && user->administrator && !user->locked;
}

int auth_login(
    const char *username,
    const char *password,
    struct unios_user *result
) {
    /*
     * Future implementation:
     *
     * 1. Ask the identity service to verify username/password.
     * 2. Compare against a salted password hash.
     * 3. Apply account lockout and login rate limits.
     * 4. Return a session identity.
     *
     * Do not implement password checking in the kernel terminal.
     */
    (void)username;
    (void)password;
    (void)result;
    return -1;
}

int auth_sudo(
    const struct unios_user *user,
    const char *command
) {
    if (!user || !command)
        return -1;

    if (!auth_is_admin(user))
        return -2;

    /*
     * Future implementation:
     *
     * - Verify the user's password.
     * - Create a temporary privileged capability.
     * - Execute only the requested command.
     * - Do not create a permanent root login.
     */
    return 0;
}

int auth_switch_user(
    const struct unios_user *current,
    const char *target_username,
    const char *password,
    struct unios_user *result
) {
    if (!current || !target_username || !password || !result)
        return -1;

    /*
     * Future implementation:
     *
     * - Verify target account credentials.
     * - Create a new OrbitCLI session.
     * - Replace the current identity.
     * - Keep filesystem and process permissions tied to the new UID.
     */
    return -1;
}

int auth_change_password(
    const struct unios_user *user,
    const char *old_password,
    const char *new_password
) {
    if (!user || !old_password || !new_password)
        return -1;

    /*
     * Password changes belong to the identity service.
     * The terminal must never write the password database directly.
     */
    return -1;
}
