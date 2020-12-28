// SPDX-License-Identifier: ISC

#include <linux/firmware.h>

#include "mt7603.h"
#include "eeprom.h"

static int
mt7603_efuse_read(struct mt7603_dev *dev, u32 base, u16 addr, u8 *data)
{
	u32 val;
	int i;

	val = mt76_rr(dev, base + MT_EFUSE_CTRL);
	val &= ~(MT_EFUSE_CTRL_AIN |
		 MT_EFUSE_CTRL_MODE);
	val |= FIELD_PREP(MT_EFUSE_CTRL_AIN, addr & ~0xf);
	val |= MT_EFUSE_CTRL_KICK;
	mt76_wr(dev, base + MT_EFUSE_CTRL, val);

	if (!mt76_poll(dev, base + MT_EFUSE_CTRL, MT_EFUSE_CTRL_KICK, 0, 1000))
		return -ETIMEDOUT;

	udelay(2);

	val = mt76_rr(dev, base + MT_EFUSE_CTRL);
	if ((val & MT_EFUSE_CTRL_AOUT) == MT_EFUSE_CTRL_AOUT ||
	    WARN_ON_ONCE(!(val & MT_EFUSE_CTRL_VALID))) {
		memset(data, 0xff, 16);
		return 0;
	}

	for (i = 0; i < 4; i++) {
		val = mt76_rr(dev, base + MT_EFUSE_RDATA(i));
		put_unaligned_le32(val, data + 4 * i);
	}

	return 0;
}

static int
mt7603_efuse_init(struct mt7603_dev *dev)
{
	u32 base = mt7603_reg_map(dev, MT_EFUSE_BASE);
	int len = MT7603_EEPROM_SIZE;
	void *buf;
	int ret, i;

	if (mt76_rr(dev, base + MT_EFUSE_BASE_CTRL) & MT_EFUSE_BASE_CTRL_EMPTY)
		return 0;

	dev->mt76.otp.data = devm_kzalloc(dev->mt76.dev, len, GFP_KERNEL);
	dev->mt76.otp.size = len;
	if (!dev->mt76.otp.data)
		return -ENOMEM;

	buf = dev->mt76.otp.data;
	for (i = 0; i + 16 <= len; i += 16) {
		ret = mt7603_efuse_read(dev, base, i, buf + i);
		if (ret)
			return ret;
	}

	return 0;
}

static bool
mt7603_has_cal_free_data(struct mt7603_dev *dev, u8 *efuse)
{
	if (!efuse[MT_EE_TEMP_SENSOR_CAL])
		return false;

	if (get_unaligned_le16(efuse + MT_EE_TX_POWER_0_START_2G) == 0)
		return false;

	if (get_unaligned_le16(efuse + MT_EE_TX_POWER_1_START_2G) == 0)
		return false;

	if (!efuse[MT_EE_CP_FT_VERSION])
		return false;

	if (!efuse[MT_EE_XTAL_FREQ_OFFSET])
		return false;

	if (!efuse[MT_EE_XTAL_WF_RFCAL])
		return false;

	return true;
}

static void
mt7603_apply_cal_free_data(struct mt7603_dev *dev, u8 *efuse)
{
	static const u8 cal_free_bytes[] = {
		MT_EE_TEMP_SENSOR_CAL,
		MT_EE_CP_FT_VERSION,
		MT_EE_XTAL_FREQ_OFFSET,
		MT_EE_XTAL_WF_RFCAL,
		/* Skip for MT7628 */
		MT_EE_TX_POWER_0_START_2G,
		MT_EE_TX_POWER_0_START_2G + 1,
		MT_EE_TX_POWER_1_START_2G,
		MT_EE_TX_POWER_1_START_2G + 1,
	};
	u8 *eeprom = dev->mt76.eeprom.data;
	int n = ARRAY_SIZE(cal_free_bytes);
	int i;

	if (!mt7603_has_cal_free_data(dev, efuse))
		return;

	if (is_mt7628(dev))
		n -= 4;

	for (i = 0; i < n; i++) {
		int offset = cal_free_bytes[i];

		eeprom[offset] = efuse[offset];
	}
}

static int
mt7603_eeprom_load(struct mt7603_dev *dev)
{
	int ret;
	u8 *eeprom;
	const struct firmware *fw;

	ret = mt76_eeprom_init(&dev->mt76, MT7603_EEPROM_SIZE);
	if (ret < 0)
		return ret;

	// Modify EEPROM Values to try and improve 2.4Ghz Operation
	eeprom = dev->mt76.eeprom.data;

	// 0xA0 - 0xAD

	// 802.11b TX Gain
	eeprom[MT_EE_TX_POWER_CCK] = 0xc9;			  // 802.11b 5.5mbps
	eeprom[MT_EE_TX_POWER_CCK+1] = 0xc8;          // 802.11b 11mbps

	// 802.11g TX Gain
	eeprom[MT_EE_TX_POWER_OFDM_2G_6M] = 0xc8;     // 802.11g  6mbps
	eeprom[MT_EE_TX_POWER_OFDM_2G_6M+1] = 0xc8;   // 802.11g 12mbps
	eeprom[MT_EE_TX_POWER_OFDM_2G_24M] = 0xc6;    // 802.11g 24mbps
	eeprom[MT_EE_TX_POWER_OFDM_2G_24M+1] = 0xc2;  // 802.11g 36mbps
	eeprom[MT_EE_TX_POWER_OFDM_2G_54M] = 0xc0;    // 802.11g 48mbps
	eeprom[MT_EE_TX_POWER_OFDM_2G_54M] = 0xc9;    // 802.11g 54mbps

	// 802.11n TX Gain
	eeprom[MT_EE_TX_POWER_HT_BPSK_QPSK] = 0xc8;   // 802.11n 7.2/14.4/15/30 mbps
	eeprom[MT_EE_TX_POWER_HT_BPSK_QPSK+1] = 0xc8; // 802.11n 21.7/43.3/45/90 mbps
	eeprom[MT_EE_TX_POWER_HT_16_64_QAM] = 0xc6;   // 802.11n 28.9/44.3/60/90(1) 57.8/86.7/120/180(2)
	eeprom[MT_EE_TX_POWER_HT_16_64_QAM+1] = 0xc4; // 802.11n 57.8/120mbps(1)  115.6/240mbps(2)
	eeprom[MT_EE_TX_POWER_HT_64_QAM] = 0xc2;      // 802.11n 65/135mbps(1)    130/270mbps(2)
	eeprom[MT_EE_TX_POWER_HT_64_QAM+1] = 0xc2;    // 802.11n 72.2/150mbps(1)  144.4/300mbps(2)

	// 0xC0 - 0xC5
	eeprom[MT_EE_ELAN_RX_MODE_GAIN] = 0x0f;		  // RX Gain
	eeprom[MT_EE_ELAN_RX_MODE_NF] = 0x01;		  // Noise Factor
	eeprom[MT_EE_ELAN_RX_MODE_P1DB] = 0x0d;		  // The 1 dB compression point (P1dB) is the output power level at which the gain decreases 1 dB from its constant value.
	eeprom[MT_EE_ELAN_BYPASS_MODE_GAIN] = 0x14;
	eeprom[MT_EE_ELAN_BYPASS_MODE_NF] = 0x14;
	eeprom[MT_EE_ELAN_BYPASS_MODE_P1DB] = 0x01;

	// This should allow me to patch eeprom for testing with a reboot, NOT a rebuild.
	if(request_firmware_direct(&fw, "eeprom_patch.bin", dev->mt76.dev) == 0) {
		if (fw->size > 0xff) {
			for (int offset = 0xa0; offset <= 0xaf; offset++) {
				eeprom[offset] = fw->data[offset];
			}
			for (int offset = 0xc0; offset <= 0xc5; offset++) {
				eeprom[offset] = fw->data[offset];
			}
		}
	}
	release_firmware(fw);

	return mt7603_efuse_init(dev);
}

static int mt7603_check_eeprom(struct mt76_dev *dev)
{
	u16 val = get_unaligned_le16(dev->eeprom.data);

	switch (val) {
	case 0x7628:
	case 0x7603:
		return 0;
	default:
		return -EINVAL;
	}
}

int mt7603_eeprom_init(struct mt7603_dev *dev)
{
	int ret;

	ret = mt7603_eeprom_load(dev);
	if (ret < 0)
		return ret;

	if (dev->mt76.otp.data) {
		if (mt7603_check_eeprom(&dev->mt76) == 0)
			mt7603_apply_cal_free_data(dev, dev->mt76.otp.data);
		else
			memcpy(dev->mt76.eeprom.data, dev->mt76.otp.data,
			       MT7603_EEPROM_SIZE);
	}

	dev->mt76.cap.has_2ghz = true;
	memcpy(dev->mt76.macaddr, dev->mt76.eeprom.data + MT_EE_MAC_ADDR,
	       ETH_ALEN);

	mt76_eeprom_override(&dev->mt76);

	return 0;
}
