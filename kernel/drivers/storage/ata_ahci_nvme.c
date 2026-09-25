#include "../driver_core.h"
#include "../pci.h"

/*
 * Storage controllers, modeled on Linux ahci.c / ata_piix.c / nvme driver
 * tables. NVMe is the standard on modern x86_64 machines; AHCI covers SATA
 * on nearly everything since 2005; ata_piix keeps legacy IDE emulation
 * working. Queue setup is gated behind the safe DMA allocator rule from
 * drivers/README.md, so probe validates BARs and PM capability only.
 */

static int stor_probe_identity(const struct unios_driver *self, const struct unios_pci_device *dev) {
    (void)self;
    return (dev && (dev->bar[0] || dev->bar[4] || dev->bar[5])) ? 1 : 0;
}

static const struct unios_match_id ahci_table[] = {
    { UNIOS_BUS_CLASS, UNIOS_PCI_ID_ANY, UNIOS_PCI_ID_ANY, 0x01, 0x06, UNIOS_CLS_STORAGE }, /* all AHCI (prog_if 0x01 checked at bind) */
    { UNIOS_BUS_PCI, 0x8086, 0x1E02, 0x01, 0x06, UNIOS_CLS_STORAGE }, /* Lynx Point AHCI */
    { UNIOS_BUS_PCI, 0x8086, 0xA282, 0x01, 0x06, UNIOS_CLS_STORAGE }, /* Intel 100-series */
    { UNIOS_BUS_PCI, 0x8086, 0x43A2, 0x01, 0x06, UNIOS_CLS_STORAGE }, /* Intel 400/500-series */
    { UNIOS_BUS_PCI, 0x1022, 0x7904, 0x01, 0x06, UNIOS_CLS_STORAGE }, /* AMD FCH AHCI */
    { UNIOS_BUS_PCI, 0x1B21, 0x0612, 0x01, 0x06, UNIOS_CLS_STORAGE }, /* ASMedia ASM1061 */
    { UNIOS_BUS_PCI, 0x10EC, 0x5111, 0x01, 0x06, UNIOS_CLS_STORAGE }, /* Realtek RTS5111 */
};
static struct unios_driver ahci_driver = {
    "ahci", UNIOS_CLS_STORAGE, ahci_table,
    sizeof(ahci_table)/sizeof(ahci_table[0]), stor_probe_identity, 0, UNIOS_DRV_STALLED
};

static const struct unios_match_id ata_piix_table[] = {
    { UNIOS_BUS_PCI, 0x8086, 0x7010, 0x01, 0x01, UNIOS_CLS_STORAGE }, /* PIIX3 IDE */
    { UNIOS_BUS_PCI, 0x8086, 0x2850, 0x01, 0x8F, UNIOS_CLS_STORAGE }, /* ICH8 in compatibility mode */
    { UNIOS_BUS_PCI, 0x8086, 0x8C71, 0x01, 0x8A, UNIOS_CLS_STORAGE }, /* Lynx Point legacy IDE */
    { UNIOS_BUS_PCI, 0x1002, 0x439C, 0x01, 0x8A, UNIOS_CLS_STORAGE }, /* ATI SB700 legacy */
};
static struct unios_driver ata_piix_driver = {
    "ata_piix", UNIOS_CLS_STORAGE, ata_piix_table,
    sizeof(ata_piix_table)/sizeof(ata_piix_table[0]), stor_probe_identity, 0, UNIOS_DRV_STALLED
};

static const struct unios_match_id nvme_table[] = {
    { UNIOS_BUS_CLASS, UNIOS_PCI_ID_ANY, UNIOS_PCI_ID_ANY, 0x01, 0x08, UNIOS_CLS_STORAGE }, /* NVM Express base class */
    { UNIOS_BUS_PCI, 0x144D, UNIOS_PCI_ID_ANY, 0x01, 0x08, UNIOS_CLS_STORAGE },             /* Samsung PM9xx/970/980/990 */
    { UNIOS_BUS_PCI, 0x8086, UNIOS_PCI_ID_ANY, 0x01, 0x08, UNIOS_CLS_STORAGE },             /* Intel/Solidigm P-series */
    { UNIOS_BUS_PCI, 0x1B96, UNIOS_PCI_ID_ANY, 0x01, 0x08, UNIOS_CLS_STORAGE },             /* Fanxiang/other Cn vendor fallback */
    { UNIOS_BUS_PCI, 0x1E0F, UNIOS_PCI_ID_ANY, 0x01, 0x08, UNIOS_CLS_STORAGE },             /* KIOXIA */
    { UNIOS_BUS_PCI, 0x1CC1, UNIOS_PCI_ID_ANY, 0x01, 0x08, UNIOS_CLS_STORAGE },             /* Marvell/CESSNVA */
    { UNIOS_BUS_PCI, 0x1402, UNIOS_PCI_ID_ANY, 0x01, 0x08, UNIOS_CLS_STORAGE },             /* Microchip/Micron */
};
static struct unios_driver nvme_driver = {
    "nvme", UNIOS_CLS_STORAGE, nvme_table,
    sizeof(nvme_table)/sizeof(nvme_table[0]), stor_probe_identity, 0, UNIOS_DRV_STALLED
};

int ahci_register(void)     { return unios_driver_register(&ahci_driver); }
int ata_piix_register(void) { return unios_driver_register(&ata_piix_driver); }
int nvme_register(void)     { return unios_driver_register(&nvme_driver); }
