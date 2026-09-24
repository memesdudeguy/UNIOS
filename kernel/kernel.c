#include <stdint.h>

#define SERIAL_PORT 0x3f8

static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static void serial_init(void) {
    outb(SERIAL_PORT + 1, 0x00);
    outb(SERIAL_PORT + 3, 0x80);
    outb(SERIAL_PORT + 0, 0x03);
    outb(SERIAL_PORT + 1, 0x00);
    outb(SERIAL_PORT + 3, 0x03);
    outb(SERIAL_PORT + 2, 0xC7);
    outb(SERIAL_PORT + 4, 0x0B);
}

static void serial_write_char(char c) {
    while ((inb(SERIAL_PORT + 5) & 0x20) == 0) {}
    outb(SERIAL_PORT, (uint8_t)c);
}

static void serial_write(const char *s) {
    while (*s) {
        serial_write_char(*s++);
    }
}

static void terminal_putc(char c) {
    serial_write_char(c);
}

static void terminal_write(const char *s) {
    serial_write(s);
}

static void terminal_clear(void) {
    terminal_write("\033[2J\033[H");
}

static int command_is(const char *command, const char *expected) {
    int i = 0;
    while (command[i] != '\0' && expected[i] != '\0') {
        if (command[i] != expected[i]) return 0;
        i++;
    }
    return command[i] == '\0' && expected[i] == '\0';
}

static int command_starts_with(const char *command, const char *prefix) {
    int i = 0;
    while (prefix[i] != '\0') {
        if (command[i] != prefix[i]) return 0;
        i++;
    }
    return 1;
}

static void print_prompt(void) {
    terminal_write("unios:/$ ");
}

static void command_help(void) {
    terminal_write("Unix: help about clear reboot shutdown echo uname whoami id pwd\n");
    terminal_write("Files: ls cd cat mkdir rm\n");
    terminal_write("Processes: ps kill\n");
    terminal_write("Security: su sudo [disabled]\n");
    terminal_write("Space: scan orbit dock undock fuel crew nav comet airlock starmap drivers\n");
}

static void command_about(void) {
    terminal_write("UNIOS minimal x86 kernel\n");
    terminal_write("Target: i386 freestanding\n");
    terminal_write("Compiler: clang wrapper / aliencc\n");
    terminal_write("Shell: safe command dispatcher\n");
}

static void command_clear(void) {
    terminal_clear();
}

static void command_reboot(void) {
    terminal_write("reboot: not implemented yet\n");
}

static void command_shutdown(void) {
    terminal_write("shutdown: qemu debug-exit is not enabled in this build\n");
}

static void command_echo(const char *arg) {
    terminal_write(arg ? arg : "");
    terminal_write("\n");
}

static void command_uname(void) {
    terminal_write("UNIOS 0.1 i386\n");
}

static void command_whoami(void) {
    terminal_write("root\n");
}

static void command_id(void) {
    terminal_write("uid=0(root) gid=0(root)\n");
}

static void command_pwd(void) {
    terminal_write("/\n");
}

static void command_ls(void) {
    terminal_write("/\n");
    terminal_write("/etc\n");
    terminal_write("/home\n");
    terminal_write("/tmp\n");
}

static void command_cd(const char *arg) {
    if (!arg || arg[0] == '\0') {
        terminal_write("cd: missing path\n");
        return;
    }
    terminal_write("cd: ");
    terminal_write(arg);
    terminal_write("\n");
}

static void command_cat(const char *arg) {
    if (!arg || arg[0] == '\0') {
        terminal_write("cat: missing file\n");
        return;
    }
    terminal_write("cat: ");
    terminal_write(arg);
    terminal_write("\n");
}

static void command_mkdir(const char *arg) {
    if (!arg || arg[0] == '\0') {
        terminal_write("mkdir: missing dir\n");
        return;
    }
    terminal_write("mkdir: ");
    terminal_write(arg);
    terminal_write("\n");
}

static void command_rm(const char *arg) {
    if (!arg || arg[0] == '\0') {
        terminal_write("rm: missing path\n");
        return;
    }
    terminal_write("rm: ");
    terminal_write(arg);
    terminal_write("\n");
}

static void command_ps(void) {
    terminal_write("PID STATE COMMAND\n");
    terminal_write("1 run kernel\n");
    terminal_write("2 run shell\n");
}

static void command_kill(const char *arg) {
    if (!arg || arg[0] == '\0') {
        terminal_write("kill: usage: kill PID\n");
        return;
    }
    terminal_write("kill: process terminated\n");
}

static void command_scan(void) {
    terminal_write("scan: no PCI hardware exposed by current QEMU profile\n");
    terminal_write("scan: VGA and PS/2 drivers available\n");
}

static void command_orbit(void) {
    terminal_write("orbit: UNIOS device manager\n");
    terminal_write("driver: vga matched\n");
    terminal_write("driver: ps2 matched\n");
    terminal_write("driver: pci not present\n");
}

static void command_dock(void) {
    terminal_write("dock: ramfs mounted at /\n");
}

static void command_undock(void) {
    terminal_write("undock: removable devices are not present\n");
}

static void command_fuel(void) {
    terminal_write("fuel: memory allocator status unavailable\n");
}

static void command_crew(void) {
    terminal_write("crew: root shell active\n");
}

static void command_nav(void) {
    terminal_write("/\n");
}

static void command_comet(void) {
    terminal_write("comet: device discovery complete\n");
}

static void command_airlock(void) {
    terminal_write("airlock: no removable device attached\n");
}

static void command_starmap(void) {
    terminal_write("/\n/etc\n/home\n/tmp\n");
}

static void command_drivers(void) {
    terminal_write("drivers:\n");
    terminal_write("vga: matched and active\n");
    terminal_write("ps2: matched and active\n");
    terminal_write("pci: disabled until enumeration exists\n");
}

static void terminal_command(const char *command) {
    if (!command[0]) return;

    if (command_is(command, "help")) { command_help(); return; }
    if (command_is(command, "about")) { command_about(); return; }
    if (command_is(command, "clear")) { command_clear(); return; }
    if (command_is(command, "reboot")) { command_reboot(); return; }
    if (command_is(command, "shutdown")) { command_shutdown(); return; }
    if (command_starts_with(command, "echo ")) { command_echo(command + 5); return; }
    if (command_is(command, "uname")) { command_uname(); return; }
    if (command_is(command, "whoami")) { command_whoami(); return; }
    if (command_is(command, "id")) { command_id(); return; }
    if (command_is(command, "pwd") || command_is(command, "nav")) { command_pwd(); return; }
    if (command_is(command, "ls")) { command_ls(); return; }
    if (command_starts_with(command, "cd ")) { command_cd(command + 3); return; }
    if (command_starts_with(command, "cat ")) { command_cat(command + 4); return; }
    if (command_starts_with(command, "mkdir ")) { command_mkdir(command + 6); return; }
    if (command_starts_with(command, "rm ")) { command_rm(command + 3); return; }
    if (command_is(command, "ps")) { command_ps(); return; }
    if (command_starts_with(command, "kill ")) { command_kill(command + 5); return; }
    if (command_is(command, "scan")) { command_scan(); return; }
    if (command_is(command, "orbit")) { command_orbit(); return; }
    if (command_is(command, "dock")) { command_dock(); return; }
    if (command_is(command, "undock")) { command_undock(); return; }
    if (command_is(command, "fuel")) { command_fuel(); return; }
    if (command_is(command, "crew")) { command_crew(); return; }
    if (command_is(command, "comet")) { command_comet(); return; }
    if (command_is(command, "airlock")) { command_airlock(); return; }
    if (command_is(command, "starmap")) { command_starmap(); return; }
    if (command_is(command, "drivers")) { command_drivers(); return; }

    terminal_write("unknown command\n");
}

static void kernel_main(void) {
    serial_init();
    terminal_write("UNIOS boot\n");

    for (;;) {
        char input[128];
        int i = 0;

        print_prompt();

        while (1) {
            char c = (char)inb(SERIAL_PORT);
            if (c == '\r' || c == '\n') {
                terminal_write("\n");
                break;
            }
            if (c == '\b' && i > 0) {
                i--;
                terminal_write("\b ");
                continue;
            }
            if (i < 127) {
                input[i++] = c;
                terminal_putc(c);
            }
        }

        input[i] = '\0';
        terminal_command(input);
    }
}
