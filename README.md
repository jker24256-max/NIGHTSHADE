# NIGHTSHADE v0.1

A starter firmware for the M5Stack Cardputer Adv. This early build offers a simple menu, Wi-Fi access-point discovery, BLE advertisement discovery, and an about/limits screen.

## Cloud build (no laptop required)

1. Open the **Actions** tab in this repository.
2. Select **Build NIGHTSHADE firmware**.
3. Press **Run workflow** (or wait for the automatic build after a commit).
4. Open the completed run and download the `NIGHTSHADE-v0.1-firmware` artifact.
5. Extract the downloaded ZIP to access the firmware files.

## Important

This project currently uses the generic `esp32-s3-devkitc-1` PlatformIO board definition with an 8 MB partition layout as a starting point. A successful compile does not prove the binary is correct for every Cardputer Adv hardware revision. Test carefully and keep a known-good firmware/recovery method available.

Implemented in this starter:
- Wi-Fi AP discovery (SSID, signal strength, open/secured label)
- BLE advertisement discovery (name, short address suffix, RSSI)
- Basic on-device menu

Not implemented yet: IR/TV-B-Gone, SD logging, network diagnostics, GATT inspection, or production-grade recovery. This firmware does not perform deauthentication, jamming, credential capture, or unauthorized device control.

The ESP32-S3 built-in Wi-Fi is 2.4 GHz. Discovery alone does not establish that a network is vulnerable.
