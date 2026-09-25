#include "../driver_core.h"
#include "../pci.h"

/*
 * Wired Ethernet, modeled on Linux r8169.c and e1000_main.c id tables.
 * Probe verifies the I/O or MMIO BAR; packet paths stay closed until the
 * network stack + a safe DMA allocator land (see drivers/README.md).
 */

static int eth_probe_identity(const struct unios_driver *self, const struct unios_pci_device *dev) {
    (void)self;
    return (dev && (dev->bar[0] || dev->bar[1])) ? 1 : 0;
}

static const struct unios_match_id rtl8169_table[] = {
    { UNIOS_BUS_PCI, 0x10EC, 0x8169, 0x02, 0x00, UNIOS_CLS_NETWORK }, /* RTL8169 */
    { UNIOS_BUS_PCI, 0x10EC, 0x8136, 0x02, 0x00, UNIOS_CLS_NETWORK }, /* RTL810xE */
    { UNIOS_BUS_PCI, 0x10EC, 0x8168, 0x02, 0x00, UNIOS_CLS_NETWORK }, /* RTL8168 */
    { UNIOS_BUS_PCI, 0x10EC, 0x8125, 0x02, 0x00, UNIOS_CLS_NETWORK }, /* RTL8125 2.5G (modern boards) */
    { UNIOS_BUS_PCI, 0x10EC, 0x8126, 0x02, 0x00, UNIOS_CLS_NETWORK }, /* RTL8126 5G */
    { UNIOS_BUS_PCI, 0x10EC, UNIOS_PCI_ID_ANY, 0x02, 0x00, UNIOS_CLS_NETWORK },
};
static struct unios_driver rtl8169_driver = {
    "r8169", UNIOS_CLS_NETWORK, rtl8169_table,
    sizeof(rtl8169_table)/sizeof(rtl8169_table[0]), eth_probe_identity, 0, UNIOS_DRV_STALLED
};

static const struct unios_match_id e1000_table[] = {
    { UNIOS_BUS_PCI, 0x8086, 0x100E, 0x02, 0x00, UNIOS_CLS_NETWORK }, /* 82540EM (QEMU default) */
    { UNIOS_BUS_PCI, 0x8086, 0x100F, 0x02, 0x00, UNIOS_CLS_NETWORK }, /* 82545GM */
    { UNIOS_BUS_PCI, 0x8086, 0x1075, 0x02, 0x00, UNIOS_CLS_NETWORK }, /* 82541PI */
    { UNIOS_BUS_PCI, 0x8086, 0x10D3, 0x02, 0x00, UNIOS_CLS_NETWORK }, /* 82574L */
    { UNIOS_BUS_PCI, 0x8086, 0x1521, 0x02, 0x00, UNIOS_CLS_NETWORK }, /* I211 */
    { UNIOS_BUS_PCI, 0x8086, 0x15B8, 0x02, 0x00, UNIOS_CLS_NETWORK }, /* I219-V (common on consumer x86_64) */
    { UNIOS_BUS_PCI, 0x8086, 0x15F9, 0x02, 0x00, UNIOS_CLS_NETWORK }, /* I219-LM */
    { UNIOS_BUS_PCI, 0x8086, UNIOS_PCI_ID_ANY, 0x02, 0x00, UNIOS_CLS_NETWORK },
};
static struct unios_driver e1000_driver = {
    "e1000", UNIOS_CLS_NETWORK, e1000_table,
    sizeof(e1000_table)/sizeof(e1000_table[0]), eth_probe_identity, 0, UNIOS_DRV_STALLED
};

int rtl8169_register(void) { return unios_driver_register(&rtl8169_driver); }
int e1000_register(void)   { return unios_driver_register(&e1000_driver); }
