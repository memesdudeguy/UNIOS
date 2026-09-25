#include "../driver_core.h"
#include "../pci.h"
#include "../ioports.h"

/*
 * Legacy ISA / built-in drivers: VGA text console, PS/2 keyboard (i8042),
 * 16550A serial UART. These match without PCI enumeration; the registry
 * marks them ACTIVE at boot once their hardware sanity probe passes.
 */

#define VGA_CRT_3D0 0x3D4
#define KBD_DATA_PORT   0x60
#define KBD_STATUS_PORT 0x64

static int vga_probe(const struct unios_driver *self, const struct unios_pci_device *dev) {
    (void)self; (void)dev;
    /* CRT controller responds on 0x3D4/0x3D5 for primary text mode. */
    outb(VGA_CRT_3D0, 0x0F);
    return 1;
}

static const struct unios_match_id vga_table[] = {
    { UNIOS_BUS_VIRTUAL, UNIOS_PCI_ID_ANY, UNIOS_PCI_ID_ANY, 0x03, 0x00, UNIOS_CLS_DISPLAY },
};
static struct unios_driver vga_driver = {
    "vga", UNIOS_CLS_DISPLAY, vga_table, 1, vga_probe, 0, UNIOS_DRV_STALLED
};

static int ps2_probe(const struct unios_driver *self, const struct unios_pci_device *dev) {
    (void)self; (void)dev;
    uint8_t status = inb(KBD_STATUS_PORT);
    return !(status & 0x80); /* controller not mid-self-test */
}

static const struct unios_match_id ps2_table[] = {
    { UNIOS_BUS_ISA, UNIOS_PCI_ID_ANY, UNIOS_PCI_ID_ANY, 0x00, 0x00, UNIOS_CLS_INPUT },
};
static struct unios_driver ps2_driver = {
    "i8042-ps2", UNIOS_CLS_INPUT, ps2_table, 1, ps2_probe, 0, UNIOS_DRV_STALLED
};

static int uart_probe(const struct unios_driver *self, const struct unios_pci_device *dev) {
    (void)self; (void)dev;
    /* 16550 loopback test on COM1 like Linux serial core does at startup. */
    outb(0x3FE, 0x1E);
    outb(0x3F7, 0xAE);
    return (inb(0x3FE) == 0x1E && inb(0x3F7) == 0xAE) ? 1 : 0;
}

static const struct unios_match_id uart_table[] = {
    { UNIOS_BUS_ISA, UNIOS_PCI_ID_ANY, UNIOS_PCI_ID_ANY, 0x00, 0x00, UNIOS_CLS_CHIPSET },
};
static struct unios_driver uart_driver = {
    "uart-16550", UNIOS_CLS_CHIPSET, uart_table, 1, uart_probe, 0, UNIOS_DRV_STALLED
};

int vga_console_register(void)  { return unios_driver_register(&vga_driver); }
int ps2_keyboard_register(void) { return unios_driver_register(&ps2_driver); }
int serial_uart_register(void)  { return unios_driver_register(&uart_driver); }
