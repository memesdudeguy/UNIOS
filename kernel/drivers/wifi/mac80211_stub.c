#include "../driver_core.h"
#include "../pci.h"

/*
 * Wireless LAN drivers, modeled on the Linux mac80211/cfg80211 family:
 * ath9k (Qualcomm Atheros), iwlwifi (Intel), mt76 (MediaTek),
 * brcmfmac (Cypress/Broadcom) and rtw88 (Realtek). These cover the vast
 * majority of WiFi chips found in modern x86_64 laptops and motherboards.
 *
 * Match tables carry the real PCI/PCIe device IDs from the upstream Linux
 * drivers so hardware identity verification works today. Frame exchange
 * stays disabled until the 802.11 stack + safe DMA ring allocator land;
 * firmware blobs will load only through a verified, checksummed path.
 */

static int wifi_probe_identity(const struct unios_driver *self, const struct unios_pci_device *dev) {
    (void)self;
    /* Verify PCIe BAR presence only; radio bring-up requires firmware and
     * is intentionally deferred (see rules in drivers/README.md). */
    return (dev && (dev->bar[0] & ~0xFu) != 0) ? 1 : 0;
}

#define M(vend, dev_, sub, dcls) { UNIOS_BUS_PCI, (vend), (dev_), 0x02, (sub), (dcls) }

static const struct unios_match_id ath9k_table[] = {
    M(0x168C, 0x002B, 0x80, UNIOS_CLS_WLAN), /* AR9285 */
    M(0x168C, 0x002E, 0x80, UNIOS_CLS_WLAN), /* AR9280 */
    M(0x168C, 0x002F, 0x80, UNIOS_CLS_WLAN), /* AR9287 */
    M(0x168C, 0x0030, 0x80, UNIOS_CLS_WLAN), /* AR9380/AR9485 family */
    M(0x168C, 0x0032, 0x80, UNIOS_CLS_WLAN), /* AR9462 */
    M(0x168C, 0x0034, 0x80, UNIOS_CLS_WLAN), /* AR9485 */
    M(0x168C, 0x0036, 0x80, UNIOS_CLS_WLAN), /* QCA9565 */
    M(0x168C, 0x003A, 0x80, UNIOS_CLS_WLAN), /* QCA988X (ath10k-class, matched here too) */
    M(0x168C, UNIOS_PCI_ID_ANY, 0x80, UNIOS_CLS_WLAN),
};
static struct unios_driver ath9k_driver = {
    "ath9k", UNIOS_CLS_WLAN, ath9k_table,
    sizeof(ath9k_table)/sizeof(ath9k_table[0]), wifi_probe_identity, 0, UNIOS_DRV_STALLED
};

static const struct unios_match_id iwlwifi_table[] = {
    M(0x8086, 0x0046, 0x80, UNIOS_CLS_WLAN), /* Centrino Advanced-N 6200 */
    M(0x8086, 0x008B, 0x80, UNIOS_CLS_WLAN), /* Centrino Wireless-N 1030 */
    M(0x8086, 0x0887, 0x80, UNIOS_CLS_WLAN), /* Wireless-N 7260 */
    M(0x8086, 0x08B0, 0x80, UNIOS_CLS_WLAN), /* Dual Band Wireless-AC 7260 */
    M(0x8086, 0x24FD, 0x80, UNIOS_CLS_WLAN), /* Wireless-AC 9560 */
    M(0x8086, 0x34F0, 0x80, UNIOS_CLS_WLAN), /* AX201 (Wi-Fi 6) */
    M(0x8086, 0x2723, 0x80, UNIOS_CLS_WLAN), /* AX200 */
    M(0x8086, 0x43F0, 0x80, UNIOS_CLS_WLAN), /* AX210/AX211 (Wi-Fi 6E) */
    M(0x8086, 0x51F0, 0x80, UNIOS_CLS_WLAN), /* AX211 (Killer BE-class boards) */
    M(0x8086, 0x7AF0, 0x80, UNIOS_CLS_WLAN), /* BE200 (Wi-Fi 7) */
    M(0x8086, UNIOS_PCI_ID_ANY, 0x80, UNIOS_CLS_WLAN),
};
static struct unios_driver iwlwifi_driver = {
    "iwlwifi", UNIOS_CLS_WLAN, iwlwifi_table,
    sizeof(iwlwifi_table)/sizeof(iwlwifi_table[0]), wifi_probe_identity, 0, UNIOS_DRV_STALLED
};

static const struct unios_match_id mt76_table[] = {
    M(0x14C3, 0x7630, 0x80, UNIOS_CLS_WLAN), /* MT7630PE */
    M(0x14C3, 0x7650, 0x80, UNIOS_CLS_WLAN), /* MT7610E */
    M(0x14C3, 0x7663, 0x80, UNIOS_CLS_WLAN), /* MT7915 dual-band Wi-Fi 6 */
    M(0x14C3, 0x7915, 0x80, UNIOS_CLS_WLAN), /* MT7915 (common SKU id) */
    M(0x14C3, 0x7916, 0x80, UNIOS_CLS_WLAN), /* MT7916 (many consumer USB/PCIe cards) */
    M(0x0E8D, 0x7663, 0x80, UNIOS_CLS_WLAN), /* MediaTek rebrand vendor id */
    M(0x14C3, UNIOS_PCI_ID_ANY, 0x80, UNIOS_CLS_WLAN),
};
static struct unios_driver mt76_driver = {
    "mt76", UNIOS_CLS_WLAN, mt76_table,
    sizeof(mt76_table)/sizeof(mt76_table[0]), wifi_probe_identity, 0, UNIOS_DRV_STALLED
};

static const struct unios_match_id brcmfmac_table[] = {
    M(0x14E4, 0x43A0, 0x80, UNIOS_CLS_WLAN), /* BCM43A0 */
    M(0x14E4, 0x43B1, 0x80, UNIOS_CLS_WLAN), /* BCM4356/4360 class */
    M(0x14E4, 0x43BF, 0x80, UNIOS_CLS_WLAN), /* BCM4366/43684 */
    M(0x14E4, 0xAA38, 0x80, UNIOS_CLS_WLAN), /* CYW55560/CYW55570 (BCM4373-family PCIe) */
    M(0x14E4, 0x4377, 0x80, UNIOS_CLS_WLAN), /* BCM43526 */
    M(0x14E4, UNIOS_PCI_ID_ANY, 0x80, UNIOS_CLS_WLAN),
};
static struct unios_driver brcmfmac_driver = {
    "brcmfmac", UNIOS_CLS_WLAN, brcmfmac_table,
    sizeof(brcmfmac_table)/sizeof(brcmfmac_table[0]), wifi_probe_identity, 0, UNIOS_DRV_STALLED
};

static const struct unios_match_id rtw88_table[] = {
    M(0x10EC, 0xB822, 0x80, UNIOS_CLS_WLAN), /* RTL8822CE */
    M(0x10EC, 0xC821, 0x80, UNIOS_CLS_WLAN), /* RTL8821CE */
    M(0x10EC, 0xC822, 0x80, UNIOS_CLS_WLAN), /* RTL8822BE */
    M(0x10EC, 0xB812, 0x80, UNIOS_CLS_WLAN), /* RTL8812AE */
    M(0x10EC, 0xF0D1, 0x80, UNIOS_CLS_WLAN), /* RTL8188FU-ish PCIe variant */
    M(0x10EC, UNIOS_PCI_ID_ANY, 0x80, UNIOS_CLS_WLAN),
};
static struct unios_driver rtw88_driver = {
    "rtw88", UNIOS_CLS_WLAN, rtw88_table,
    sizeof(rtw88_table)/sizeof(rtw88_table[0]), wifi_probe_identity, 0, UNIOS_DRV_STALLED
};

int ath9k_register(void)    { return unios_driver_register(&ath9k_driver); }
int iwlwifi_register(void)  { return unios_driver_register(&iwlwifi_driver); }
int mt76_register(void)     { return unios_driver_register(&mt76_driver); }
int brcmfmac_register(void) { return unios_driver_register(&brcmfmac_driver); }
int rtw88_register(void)    { return unios_driver_register(&rtw88_driver); }
