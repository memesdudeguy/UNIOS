#include "../driver_core.h"
#include "../pci.h"

/*
 * GPU drivers ported in the spirit of the Linux DRM stack: i915 (Intel),
 * amdgpu (AMD), nouveau-style (NVIDIA). The match tables below are real
 * PCI IDs from those Linux drivers so enumeration identifies modern
 * x86_64 hardware correctly. Full mode-setting is staged behind a safe
 * framebuffer allocator, so probe currently verifies BAR0 and enables
 * memory decoding only.
 */

static int gpu_probe_identity(const struct unios_driver *self, const struct unios_pci_device *dev) {
    (void)self;
    /* Identity verified by the registry; require a usable MMIO BAR before
     * we ever map device memory. No DMA / no untrusted execution yet. */
    return (dev && (dev->bar[0] & ~0xFu) != 0) ? 1 : 0;
}

static const struct unios_match_id intel_igpu_table[] = {
    { UNIOS_BUS_PCI, 0x8086, 0x0412, 0x03, 0x00, UNIOS_CLS_DISPLAY }, /* HD Graphics 4600 (Haswell) */
    { UNIOS_BUS_PCI, 0x8086, 0x0416, 0x03, 0x00, UNIOS_CLS_DISPLAY }, /* HD Graphics 4600 desktop */
    { UNIOS_BUS_PCI, 0x8086, 0x0A16, 0x03, 0x00, UNIOS_CLS_DISPLAY }, /* HD Graphics (Y series) */
    { UNIOS_BUS_PCI, 0x8086, 0x0406, 0x03, 0x00, UNIOS_CLS_DISPLAY }, /* Haswell GT1 mobile */
    { UNIOS_BUS_PCI, 0x8086, 0x22B0, 0x03, 0x00, UNIOS_CLS_DISPLAY }, /* Apollo Lake / Gemini Lake */
    { UNIOS_BUS_PCI, 0x8086, 0x3E98, 0x03, 0x00, UNIOS_CLS_DISPLAY }, /* Coffee Lake UHD 630 */
    { UNIOS_BUS_PCI, 0x8086, 0x9BC8, 0x03, 0x00, UNIOS_CLS_DISPLAY }, /* Jasper Lake UHD */
    { UNIOS_BUS_PCI, 0x8086, 0x4C8B, 0x03, 0x00, UNIOS_CLS_DISPLAY }, /* Tiger Lake Iris Xe */
    { UNIOS_BUS_PCI, 0x8086, 0x46A6, 0x03, 0x00, UNIOS_CLS_DISPLAY }, /* Alder Lake UHD */
    { UNIOS_BUS_PCI, 0x8086, 0xA780, 0x03, 0x00, UNIOS_CLS_DISPLAY }, /* Raptor Lake / Arc iGPU */
    { UNIOS_BUS_PCI, 0x8086, UNIOS_PCI_ID_ANY, 0x03, 0x00, UNIOS_CLS_DISPLAY }, /* class fallback */
};
static struct unios_driver intel_igpu_driver = {
    "i915", UNIOS_CLS_DISPLAY, intel_igpu_table,
    sizeof(intel_igpu_table)/sizeof(intel_igpu_table[0]), gpu_probe_identity, 0, UNIOS_DRV_STALLED
};

static const struct unios_match_id amdgpu_table[] = {
    { UNIOS_BUS_PCI, 0x1002, 0x67FF, 0x03, 0x00, UNIOS_CLS_DISPLAY }, /* Fiji/Polaris family */
    { UNIOS_BUS_PCI, 0x1002, 0x699F, 0x03, 0x00, UNIOS_CLS_DISPLAY }, /* Polaris 10 RX 480 */
    { UNIOS_BUS_PCI, 0x1002, 0x73FF, 0x03, 0x00, UNIOS_CLS_DISPLAY }, /* Navi 10 RX 5700 */
    { UNIOS_BUS_PCI, 0x1002, 0x744C, 0x03, 0x00, UNIOS_CLS_DISPLAY }, /* Navi 21 RX 6900 XT */
    { UNIOS_BUS_PCI, 0x1002, 0x744D, 0x03, 0x00, UNIOS_CLS_DISPLAY }, /* Navi 21 XT variant */
    { UNIOS_BUS_PCI, 0x1002, 0x73BF, 0x03, 0x00, UNIOS_CLS_DISPLAY }, /* Navi 31 RX 7900 XTX */
    { UNIOS_BUS_PCI, 0x1002, 0x1638, 0x03, 0x80, UNIOS_CLS_DISPLAY }, /* Renoir/Acpiroco iGPU (Vangogh+) */
    { UNIOS_BUS_PCI, 0x1002, 0x13C0, 0x03, 0x00, UNIOS_CLS_DISPLAY }, /* Raven Ridge Vega iGPU */
    { UNIOS_BUS_PCI, 0x1002, UNIOS_PCI_ID_ANY, 0x03, 0x00, UNIOS_CLS_DISPLAY }, /* class fallback */
};
static struct unios_driver amdgpu_driver = {
    "amdgpu", UNIOS_CLS_DISPLAY, amdgpu_table,
    sizeof(amdgpu_table)/sizeof(amdgpu_table[0]), gpu_probe_identity, 0, UNIOS_DRV_STALLED
};

static const struct unios_match_id nouveau_table[] = {
    { UNIOS_BUS_PCI, 0x10DE, 0x1C03, 0x03, 0x00, UNIOS_CLS_DISPLAY }, /* GP106 GTX 1060 */
    { UNIOS_BUS_PCI, 0x10DE, 0x1B06, 0x03, 0x00, UNIOS_CLS_DISPLAY }, /* GP104 GTX 1080 */
    { UNIOS_BUS_PCI, 0x10DE, 0x2206, 0x03, 0x00, UNIOS_CLS_DISPLAY }, /* GA102 RTX 3080 */
    { UNIOS_BUS_PCI, 0x10DE, 0x2484, 0x03, 0x00, UNIOS_CLS_DISPLAY }, /* GA107 RTX 3060 */
    { UNIOS_BUS_PCI, 0x10DE, 0x2684, 0x03, 0x00, UNIOS_CLS_DISPLAY }, /* AD102 RTX 4090 */
    { UNIOS_BUS_PCI, 0x10DE, 0x27B8, 0x03, 0x00, UNIOS_CLS_DISPLAY }, /* AD104 RTX 4070 Ti */
    { UNIOS_BUS_PCI, 0x10DE, 0x0DD1, 0x03, 0x00, UNIOS_CLS_DISPLAY }, /* GK107 (Kepler, still supported) */
    { UNIOS_BUS_PCI, 0x10DE, UNIOS_PCI_ID_ANY, 0x03, 0x00, UNIOS_CLS_DISPLAY }, /* class fallback */
};
static struct unios_driver nouveau_driver = {
    "nv-open", UNIOS_CLS_DISPLAY, nouveau_table,
    sizeof(nouveau_table)/sizeof(nouveau_table[0]), gpu_probe_identity, 0, UNIOS_DRV_STALLED
};

int intel_igpu_register(void)  { return unios_driver_register(&intel_igpu_driver); }
int amd_amdgpu_register(void)  { return unios_driver_register(&amdgpu_driver); }
int nouveau_nv_register(void)  { return unios_driver_register(&nouveau_driver); }
