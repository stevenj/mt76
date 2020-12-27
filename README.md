# Hacked mt76 firmware for newifi 2.4ghz issues.

This version patches the values read from eeprom for the MT7603EN as follows:

| ADDRESS | NAME | ORIGINAL | OpenWRT Rec | China Rec |
|---------|------|-----------|-----------|
| 0x0a0 | MT_EE_TX_POWER_CCK | 0xC2 | 0xC6 |
| 0x0a1 | MT_EE_TX_POWER_CCK+1 | 0xC2 | 0xC6 |
| 0x0a2 | MT_EE_TX_POWER_OFDM_2G_6M | 0xC2 | 0xC6 |
| 0x0a4 | MT_EE_TX_POWER_OFDM_2G_24M | 0xC2 | 0xC6 |
| 0x0aa | MT_EE_TX_POWER_HT_16_64_QAM | 0xC4C0 | 0xC4C2 |
| 0x0ac | MT_EE_TX_POWER_HT_64_QAM | 0xC084 | 0xC0C0 |
| 0x0c0 | MT_EE_ELAN_RX_MODE_GAIN | 0x0a | 0x0e |
| 0x0c1 | MT_EE_ELAN_RX_MODE_NF | 0x04 | 0x02 |
| 0x0c2 | MT_EE_ELAN_RX_MODE_P1DB | 0x0a | 0x00 |
| 0x0c3 | MT_EE_ELAN_BYPASS_MODE_GAIN | 0x08 | 0x0D |
| 0x0c4 | MT_EE_ELAN_BYPASS_MODE_NF | 0x08 | 0x0D |
| 0x0c5 | MT_EE_ELAN_BYPASS_MODE_P1DB | 0x0f | 0x0A |
|  |  |  |  |
| 0x34 | MT_EE_NIC_CONF_0 | 0x22 | 0x23 |
|

## 0xA0 - 0xAF

| Set | A0 | A1 | A2 | A3 | A4 | A5 | A6 | A7 | A8 | A9 | AA | AB | AC | AD | AE | AF |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Set | A0 | A1 | A2 | A3 | A4 | A5 | A6 | A7 | A8 | A9 | AA | AB | AC | AD | AE | AF |

## 0xC0 - 0xC5 - Chinese Recommended Values

03,02,0a,0d,0d,0a

## 0xA0 - 0xAD = Gain.

- 0x88 = 4.0dBm attenuate
- 0x87 = 3.5dBm attenuate
- 0x86 = 3.0dBm attenuate
- 0x85 = 2.5dBm attenuate
- 0x84 = 2.0dBm attenuate
- 0x83 = 1.5dBm attenuate
- 0x82 = 1.0dBm attenuate
- 0x81 = 0.5dBm attenuate
- 0x00 = No Gain
- 0xc1 = 0.5dBm gain
- 0xc2 = 1.0dBm gain
- 0xc3 = 1.5dBm gain
- 0xc4 = 2.0dBm gain
- 0xc5 = 2.5dBm gain
- 0xc6 = 3.0dBm gain
- 0xc7 = 3.5dBm gain
- 0xc8 = 4.0dBm gain

## 0xC0 - 0xC5

Valid Values 0x00 - 0x0F

* NF = Noise Factor, my guess is this tunes the Noise Factor compensation to
  match the Amplifier onboard
* P1DB = The 1 dB compression point (P1dB) is the output power level at which
  the gain decreases 1 dB from its constant value.  Again, my guess this tunes
  internal compensation to the P1dB value of the amplifier.

### References
https://aisoa.cn/post-2439.html
https://forum.openwrt.org/t/newifi-d2-terrible-wifi-performance-on-openwrt/66473/23