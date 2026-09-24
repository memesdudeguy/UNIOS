# OrbitCLI

OrbitCLI is the built-in UNIOS command-line interface.

It is not Bash, Zsh, or a separate shell process. It is a small command
dispatcher inside the UNIOS userspace terminal.

It supports familiar Unix/Linux command names and UNIOS space-themed aliases.

Examples:

    ls
    pwd
    cd /home/pilot
    scan
    orbit
    drivers
    rescan
    dock /dev/disk0 /mnt
    fuel
    launch starship
    airlock old-file.txt
    aliencc hello.c -o hello
    reboot

Prompt:

    [UNIOS::ORBIT] pilot@ship:/home/pilot >

## Privilege commands

OrbitCLI includes:

    sudo command
    su username
    whoami
    id
    passwd

Space-themed aliases:

    captain command
    transfer username
    identity

`sudo` grants temporary privilege for one command. `su` creates a new
OrbitCLI identity session. Passwords must be stored as salted hashes by the
UNIOS identity service, never as plaintext and never inside the terminal
program.

## OrbitLine

OrbitCLI uses OrbitLine for interactive editing.

Supported keys:

- Arrow keys: movement and history
- Home/End: line navigation
- Backspace/Delete: editing
- Tab: command and path completion
- Ctrl+C: cancel
- Ctrl+L: clear display
- Ctrl+U: clear line
- Ctrl+W: delete previous word

The terminal must provide a PS/2 or USB keyboard driver before these keys
can work on real hardware.
