#include "../driver_core.h"
#include "../pci.h"

/*
 * Audio: HD Audio (snd-hda-intel equivalent — Intel/AMD/NVIDIA/Realtek
 * codecs on essentially all modern x86_64 boards) and legacy AC'97.
 * Match by PCI class 04/03 (HD Audio) and 04/01 (multimedia audio).
 */

static int audio_probe_identity(const struct unios_driver *self, const struct unios_pci_device *dev) {
    (void)self;
    return (dev && (dev->bar[0] & ~0xFu) != 0) ? 1 : 0;
}

static const struct unios_match_id hda_table[] = {
    { UNIOS_BUS_CLASS, UNIOS_PCI_ID_ANY, UNIOS_PCI_ID_ANY, 0x04, 0x03, UNIOS_CLS_AUDIO },
    { UNIOS_BUS_PCI, 0x8086, 0x8C50, 0x04, 0x03, UNIOS_CLS_AUDIO }, /* Lynx Point HDA */
    { UNIOS_BUS_PCI, 0x8086, 0xA170, 0x04, 0x03, UNIOS_CLS_AUDIO }, /* Intel 100/200-series HDA */
    { UNIOS_BUS_PCI, 0x8086, 0x7AD0, 0x04, 0x03, UNIOS_CLS_AUDIO }, /* Tiger Lake-H HDA */
    { UNIOS_BUS_PCI, 0x8086, 0x51CA, 0x04, 0x03, UNIOS_CLS_AUDIO }, /* Alder Lake HDA */
    { UNIOS_BUS_PCI, 0x1022, 0x15E3, 0x04, 0x03, UNIOS_CLS_AUDIO }, /* AMD Starship/Trinity+ HDA */
    { UNIOS_BUS_PCI, 0x10DE, UNIOS_PCI_ID_ANY, 0x04, 0x03, UNIOS_CLS_AUDIO }, /* NVIDIA HDMI audio */
    { UNIOS_BUS_PCI, 0x1002, UNIOS_PCI_ID_ANY, 0x04, 0x03, UNIOS_CLS_AUDIO }, /* AMD HDMI audio */
};
static struct unios_driver hda_driver = {
    "hda-intel", UNIOS_CLS_AUDIO, hda_table,
    sizeof(hda_table)/sizeof(hda_table[0]), audio_probe_identity, 0, UNIOS_DRV_STALLED
};

static const struct unios_match_id ac97_table[] = {
    { UNIOS_BUS_PCI, 0x8086, 0x24C5, 0x04, 0x01, UNIOS_CLS_AUDIO }, /* ICH4 AC97 */
    { UNIOS_BUS_PCI, 0x8086, 0x2668, 0x04, 0x01, UNIOS_CLS_AUDIO }, /* ICH6 AC97 */
    { UNIOS_BUS_PCI, 0x8086, 0x27D9, 0x04, 0x01, UNIOS_CLS_AUDIO }, /* ICH7 AC97 */
    { UNIOS_BUS_PCI, 0x10B9, 0x7110, 0x04, 0x01, UNIOS_CLS_AUDIO }, /* ALi M5455 */
    { UNIOS_BUS_PCI, 0x10DE, 0x01B1, 0x04, 0x01, UNIOS_CLS_AUDIO }, /* nForce AC97 */
};
static struct unios_driver ac97_driver = {
    "ac97", UNIOS_CLS_AUDIO, ac97_table,
    sizeof(ac97_table)/sizeof(ac97_table[0]), audio_probe_identity, 0, UNIOS_DRV_STALLED
};

int hda_intel_register(void) { return unios_driver_register(&hda_driver); }
int ac97_register(void)      { return unios_driver_register(&ac97_driver); }
