# Hacked mt76 firmware for newifi 2.4ghz issues.

This version patches the values read from eeprom for the MT7603EN as follows:

| ADDRESS | NAME | OLD Value | New Value |
|---------|------|-----------|-----------|
| 0x0a0 | MT_EE_TX_POWER_CCK | 0xC2 | 0xC6 |
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

