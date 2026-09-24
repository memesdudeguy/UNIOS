#include <stdint.h>

static volatile uint16_t *const VGA = (uint16_t *)0xB8000;
static uint8_t row = 0;
static uint8_t col = 0;
static const uint8_t color = 0x07;


static void port_out8(uint16_t port, uint8_t value)
{
    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"(value), "Nd"(port)
    );
}

static void terminal_cursor_update(void)
{
    uint16_t position = (uint16_t)row * 80u + col;

    port_out8(0x3D4, 0x0F);
    port_out8(0x3D5, (uint8_t)(position & 0xFF));

    port_out8(0x3D4, 0x0E);
    port_out8(0x3D5, (uint8_t)(position >> 8));

    /* Enable a blinking underline cursor */
    port_out8(0x3D4, 0x0A);
    port_out8(0x3D5, 0x0E);

    port_out8(0x3D4, 0x0B);
    port_out8(0x3D5, 0x0F);
}

static void terminal_clear(void)
{
    for (uint32_t i = 0; i < 80 * 25; i++)
        VGA[i] = ((uint16_t)color << 8) | ' ';

    row = 0;
    col = 0;
    terminal_cursor_update();
}

static void terminal_putc(char c)
{
    if (c == 10) {
        col = 0;
        row++;

        if (row >= 25)
            row = 0;

        terminal_cursor_update();
        return;
    }

    if (c == 8) {
        if (col > 0) {
            col--;
            VGA[row * 80 + col] =
                ((uint16_t)color << 8) | ' ';
        }

        terminal_cursor_update();
        return;
    }

    if (col >= 80) {
        col = 0;
        row++;
    }

    if (row >= 25)
        row = 0;

    VGA[row * 80 + col] =
        ((uint16_t)color << 8) | (uint8_t)c;

    col++;
    terminal_cursor_update();
}

static void terminal_write(const char *text)
{
    while (*text)
        terminal_putc(*text++);
}

static uint8_t port_in8(uint16_t port)
{
    uint8_t value;

    __asm__ volatile (
        "inb %1, %0"
        : "=a"(value)
        : "Nd"(port)
    );

    return value;
}

static char keymap[128] = {
    [0x02] = '1',
    [0x03] = '2',
    [0x04] = '3',
    [0x05] = '4',
    [0x06] = '5',
    [0x07] = '6',
    [0x08] = '7',
    [0x09] = '8',
    [0x0a] = '9',
    [0x0b] = '0',
    [0x0c] = '-',
    [0x0d] = '=',
    [0x10] = 'q',
    [0x11] = 'w',
    [0x12] = 'e',
    [0x13] = 'r',
    [0x14] = 't',
    [0x15] = 'y',
    [0x16] = 'u',
    [0x17] = 'i',
    [0x18] = 'o',
    [0x19] = 'p',
    [0x1e] = 'a',
    [0x1f] = 's',
    [0x20] = 'd',
    [0x21] = 'f',
    [0x22] = 'g',
    [0x23] = 'h',
    [0x24] = 'j',
    [0x25] = 'k',
    [0x26] = 'l',
    [0x2c] = 'z',
    [0x2d] = 'x',
    [0x2e] = 'c',
    [0x2f] = 'v',
    [0x30] = 'b',
    [0x31] = 'n',
    [0x32] = 'm',
    [0x27] = ';',
    [0x28] = '\'',
    [0x29] = '`',
    [0x33] = ',',
    [0x34] = '.',
    [0x35] = '/',
    [0x39] = ' '
};

static char shifted_key(uint8_t code)
{
    switch (code) {
        case 0x02: return '!';
        case 0x03: return '@';
        case 0x04: return '#';
        case 0x05: return '$';
        case 0x06: return '%';
        case 0x07: return '^';
        case 0x08: return '&';
        case 0x09: return '*';
        case 0x0a: return '(';
        case 0x0b: return ')';
        case 0x0c: return '_';
        case 0x0d: return '+';
        case 0x27: return ':';
        case 0x28: return '"';
        case 0x33: return '<';
        case 0x34: return '>';
        case 0x35: return '?';
        default: return 0;
    }
}

static int keyboard_read_key(void)
{
    static int shift = 0;

    for (;;) {
        while ((port_in8(0x64) & 1) == 0)
            __asm__ volatile ("pause");

        uint8_t code = port_in8(0x60);

        if (code == 0x2a || code == 0x36) {
            shift = 1;
            continue;
        }

        if (code == 0xaa || code == 0xb6) {
            shift = 0;
            continue;
        }

        if (code & 0x80)
            continue;

        if (code == 0x1c)
            return 10;

        if (code == 0x0e)
            return 8;

        if (code == 0x0f)
            return 9;

        if (code == 0x01)
            return 27;

        if (code >= 128)
            continue;

        char c = keymap[code];

        if (shift) {
            char shifted = shifted_key(code);

            if (shifted)
                c = shifted;
            else if (c >= 'a' && c <= 'z')
                c = (char)(c - 'a' + 'A');
        }

        if (c)
            return (unsigned char)c;
    }
}

static void terminal_command(const char *command)
{
    if (!command[0])
        return;

    if (command[0] == 'h' &&
        command[1] == 'e' &&
        command[2] == 'l' &&
        command[3] == 'p' &&
        command[4] == 0) {
        terminal_write("commands: help about clear reboot");
        terminal_putc(10);
        return;
    }

    if (command[0] == 'a' &&
        command[1] == 'b' &&
        command[2] == 'o' &&
        command[3] == 'u' &&
        command[4] == 't' &&
        command[5] == 0) {
        terminal_write("UNIOS: Universal Native Integrated Operating System");
        terminal_putc(10);
        terminal_write("compiler: Alien Compiler");
        terminal_putc(10);
        return;
    }

    if (command[0] == 'c' &&
        command[1] == 'l' &&
        command[2] == 'e' &&
        command[3] == 'a' &&
        command[4] == 'r' &&
        command[5] == 0) {
        terminal_clear();
        return;
    }

    terminal_write("unknown command");
    terminal_putc(10);
}

static void terminal_readline(void)
{
    char buffer[128];
    uint32_t length = 0;

    terminal_write("unios:/$ ");

    for (;;) {
        int key = keyboard_read_key();

        if (key == 10) {
            buffer[length] = 0;
            terminal_putc(10);
            terminal_command(buffer);
            length = 0;
            terminal_write("unios:/$ ");
            continue;
        }

        if (key == 8) {
            if (length > 0) {
                length--;
                buffer[length] = 0;
                terminal_putc(8);
            }
            continue;
        }

        if (key >= 32 && key <= 126 &&
            length < sizeof(buffer) - 1) {
            buffer[length++] = (char)key;
            buffer[length] = 0;
            terminal_putc((char)key);
        }
    }
}

void kmain(uint32_t magic, uint32_t multiboot_info)
{
    (void)magic;
    (void)multiboot_info;

    terminal_clear();

    terminal_write("UNIOS booted");
    terminal_putc(10);
    terminal_write("Alien Compiler kernel");
    terminal_putc(10);
    terminal_write("Minimal built-in terminal active");
    terminal_putc(10);

    terminal_command("about");
    terminal_readline();

    for (;;) {
        __asm__ volatile ("hlt");
    }
}
