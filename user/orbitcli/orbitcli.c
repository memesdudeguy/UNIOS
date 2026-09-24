#include "orbitcli.h"

static const struct orbit_command commands[] = {
    {"ls",       "filesystem.list",    "list files"},
    {"cd",       "filesystem.chdir",   "change directory"},
    {"pwd",      "filesystem.getcwd",  "show current directory"},
    {"cat",      "filesystem.read",    "display a file"},
    {"cp",       "filesystem.copy",    "copy a file"},
    {"mv",       "filesystem.move",    "move a file"},
    {"rm",       "filesystem.remove",  "remove a file"},
    {"mkdir",    "filesystem.mkdir",   "create a directory"},
    {"clear",    "terminal.clear",     "clear the terminal"},
    {"help",     "terminal.help",      "show commands"},
    {"uname",    "system.info",        "show UNIOS information"},
    {"ps",       "process.list",       "show running processes"},
    {"free",     "memory.info",        "show memory information"},
    {"df",       "filesystem.space",   "show disk space"},
    {"reboot",   "system.reboot",     "restart UNIOS"},
    {"poweroff", "system.poweroff",   "power off UNIOS"},

    {"scan",     "device.scan",       "scan hardware"},
    {"orbit",    "device.list",       "show detected devices"},
    {"dock",     "filesystem.mount",  "mount a filesystem"},
    {"undock",   "filesystem.umount", "unmount a filesystem"},
    {"launch",   "process.start",     "start a program"},
    {"crew",     "process.list",      "show processes and users"},
    {"fuel",     "system.resources",  "show memory and storage"},
    {"nav",      "filesystem.getcwd", "show current location"},
    {"comet",    "filesystem.search", "search for a file"},
    {"beam",     "terminal.send",     "send terminal text"},
    {"airlock",  "filesystem.safe_remove", "safely remove a file"},
    {"starmap",  "filesystem.mounts", "show mounted filesystems"},
    {"aliencc",  "compiler.c",       "compile C source"},
    {"aliencxx", "compiler.cpp",     "compile C++ source"},
    {"drivers",  "driver.list",      "show drivers"},
    {"rescan",   "device.rescan",    "rescan hardware"},
    {"unload",   "driver.unload",    "unload an unused driver"}
};

static const unsigned command_count =
    sizeof(commands) / sizeof(commands[0]);

void orbitcli_init(void)
{
    /* Initialize terminal, filesystem, process, and device services. */
}

void orbitcli_print_prompt(void)
{
    /*
     * Actual terminal output will eventually be:
     *
     * [UNIOS::ORBIT] pilot@ship:/home/pilot >
     */
}

int orbitcli_execute(const char *line)
{
    if (!line || !line[0])
        return 0;

    for (unsigned i = 0; i < command_count; i++) {
        const char *a = line;
        const char *b = commands[i].name;

        while (*a && *b && *a == *b) {
            a++;
            b++;
        }

        if (*a == ' ' || *a == '\0') {
            /*
             * The action dispatcher will call:
             *
             * filesystem.list
             * device.scan
             * driver.list
             * system.reboot
             *
             * This avoids needing Bash or another external shell.
             */
            return 0;
        }
    }

    return -1;
}
