# Hacked mt76 firmware for newifi-d2 2.4ghz issues.

***THIS PATCH IS SPECIFIC TO THE NEWIFI-D2 HARDWARE.  DO NOT USE ON ANY OTHER DEVICE.***

The Changed EEPROM Values are found:

```
mt7603/eeprom.c
```

## How to use

This is a fork of the upstream openwrt mt76 driver.
We insert our changes into commits directly in the openwrt-19.07 branch.

We then need to fork openwrt source and change `package/kernel/mt76/Makefile`:

```
PKG_SOURCE_URL:=<url of this repo>
PKG_SOURCE_DATE:=<date of your change>
PKG_SOURCE_VERSION:=<hash of your checked in change in the driver repo>
```

Then produce a patch from the forked openwrt source branch like so:

```
git diff v19.07.5 > ../phero-openwrt/Custom/patches/newifi-2_4ghz-wifi-mods.patch
```

Which creates a patch to modify this makefile when the phero-openwrt code is
building.  The patch then needs to be used by all targets using the newifi-d2
hardware.

***THIS PATCH IS SPECIFIC TO THE NEWIFI-D2 HARDWARE.  DO NOT USE ON ANY OTHER DEVICE.***

## Details of the patch

This version patches the values read from eeprom for the MT7603EN as follows:

| ADDRESS | NAME | Notes |
|---------|------| ----- |
| 0x0a0 | MT_EE_TX_POWER_CCK | 802.11b Gain - 5.5mbps |
| 0x0a1 | MT_EE_TX_POWER_CCK+1 | 802.11b Gain - 11mbps |
| 0x0a2 | MT_EE_TX_POWER_OFDM_2G_6M | 802.11g Gain - 6/9mbps |
| 0x0a3 | MT_EE_TX_POWER_OFDM_2G_6M+1 | 802.11g Gain - 12/18mbps |
| 0x0a4 | MT_EE_TX_POWER_OFDM_2G_24M | 802.11g Gain - 24mbps |
| 0x0a5 | MT_EE_TX_POWER_OFDM_2G_24M+1 | 802.11g Gain - 36mbps |
| 0x0a6 | MT_EE_TX_POWER_OFDM_2G_54M | 802.11g Gain - 48mbps |
| 0x0a7 | MT_EE_TX_POWER_OFDM_2G_54M+1 | 802.11g Gain - 54mbps |
| 0x0a8	| MT_EE_TX_POWER_HT_BPSK_QPSK | 802.11n Gain BPSK 1/2 |
| 0x0a9 |	MT_EE_TX_POWER_HT_BPSK_QPSK+1 | 802.11n Gain QPSK 1/2, 3/4 |
| 0x0aa	| MT_EE_TX_POWER_HT_16_64_QAM | 802.11n Gain 16-QAM 1/2, 3/4 |
| 0x0ab | MT_EE_TX_POWER_HT_16_64_QAM+1 | 802.11n Gain 64-QAM 2/3 |
| 0x0ac | MT_EE_TX_POWER_HT_16_64_QAM+1 | 802.11n Gain 64-QAM 3/4 |
| 0x0ad | MT_EE_TX_POWER_HT_16_64_QAM+1 | 802.11n Gain 64-QAM 5/6 |
| 0x0ae | Unused | |
| 0x0af | Unused | |
| 0x0c0 | MT_EE_ELAN_RX_MODE_GAIN | Gain of Receiver in Normal Mode |
| 0x0c1 | MT_EE_ELAN_RX_MODE_NF | Noise Factor |
| 0x0c2 | MT_EE_ELAN_RX_MODE_P1DB | P1dB |
| 0x0c3 | MT_EE_ELAN_BYPASS_MODE_GAIN | Gain of Receiver in Bypass Mode |
| 0x0c4 | MT_EE_ELAN_BYPASS_MODE_NF | Noise Factor |
| 0x0c5 | MT_EE_ELAN_BYPASS_MODE_P1DB | P1DB |

### Noise Factor

Noise Factor is the measurement of degradation of the signal-to-noise ratio
(SNR), caused by components in a signal chain. It is a number by which the
performance of an amplifier or a radio receiver can be specified, with lower
values indicating better performance.  This is a function of the Receive
Amplifier, it is not tunable.  I believe what
this setting does is advise the WiFi firmware what the Noise Factor of the
amplifier is, so it can do appropriate software compensation/filtering.

### P1dB

The 1 dB compression point (P1dB) is the output power level at which the gain
decreases 1 dB from its constant value.  Again, this is a function of the
amplifier and is not tunable.  I believe this advises the WiFi firmware what the
P1dB point of the amplifier is so it can do appropriate filtering/adjustment in
software to compensate.


## 0xA0 - 0xAF

| Set    | A0 | A1 | A2 | A3 | A4 | A5 | A6 | A7 | A8 | A9 | AA | AB | AC | AD |
| ---    | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- |
| MODE   | 11b | 11b | 11g | 11g | 11g | 11g | 11g | 11g | 11n | 11n | 11n | 11n | 11n | 11n |
| RATE   | 5.5 | 11 | 6 | 12 | 24 | 36 | 48 | 54 | BPSK | QPSK | 16Q | 64Q23 | 64Q34 | 64Q56 |
|  |
| Actual | c6 | c6 | c6 | c6 | c6 | c0 | c4 | c8 | c8 | c8 | c8 | c8 | c8 | c8 |
|  |
| EEPROM | c2 | c2 | c2 | c4 | c4 | c0 | c0 | c4 | c4 | c4 | c4 | c0 | c0 | 84 |
| Forum  | c6 | c6 | c4 | c4 | c4 | c0 | c0 | c4 | c4 | c4 | c4 | c2 | c0 | c0 |
| China  | c8 | c8 | c6 | c4 | c4 | c2 | c2 | c8 | c6 | c4 | c4 | c6 | c4 | c2 |
| DL     | c9 | c8 | c8 | c8 | c6 | c2 | c0 | c9 | c8 | c8 | c6 | c4 | c2 | c0 |


* EEPROM = Setting in the NeWiFi EEPROM Hardware
* Forum = Setting recommended on OpenWRT Forum
* China = Recommendation from Chinese Forum
* Actual = Settings we settled on after testing.


## 0xC0 - 0xC5 - Chinese Recommended Values

| Set    | C0 | C1 | C2 | C3 | C4 | C5 |
| ---    | -- | -- | -- | -- | -- | -- |
| MODE   | RX | RX | RX | BYPASS | BYPASS | BYPASS |
| RATE   | GAIN | NF | P1dB | GAIN | NF | P1dB |
|  |
| Actual | 0f | 02 | 0a | 0f | 02 | 0a |
|  |
| EEPROM | 0a | 04 | 0a | 08 | 08 | 0f |
| Forum  | 0e | 02 | 00 | 0d | 0d | 0a |
| China  | 0e | 02 | 0d | 0d | 0d | 0a |
| DL     | 0e | 01 | 0d | 14 | 14 | 01 |


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

Gain, 0x00 - 0x0F in 0.5dBm steps.

* NF = Noise Factor, my guess is this tunes the Noise Factor compensation to
  match the Amplifier onboard
* P1DB = The 1 dB compression point (P1dB) is the output power level at which
  the gain decreases 1 dB from its constant value.  Again, my guess this tunes
  internal compensation to the P1dB value of the amplifier.

### References

https://aisoa.cn/post-2439.html
https://forum.openwrt.org/t/newifi-d2-terrible-wifi-performance-on-openwrt/66473/23
https://www.minihere.com/newifi3-d2-eeprom-download.html