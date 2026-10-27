# Uno-R3-Ethernet-shield-
using an R3 you can create an ethernet shield, it supports W5100 / W5500 :)
# Arduino Uno R3 Ethernet configuration shield

This design uses a W5500 Ethernet Shield R3 (a W5100 shield is also compatible with the included `Ethernet` library). It is a wired-network design: Ethernet hardware does **not** connect the Uno to Wi-Fi.

## Hardware

- Arduino Uno R3
- W5500 Ethernet Shield R3 (recommended; its W5100 predecessor also works)
- RJ45 Ethernet cable to a router or switch
- USB cable/power supply

Stack the shield directly onto the Uno's R3 headers. The shield uses the Uno SPI lines D11 (MOSI), D12 (MISO), D13 (SCK), and usually D10 as chip-select; avoid using these pins for other peripherals. The microSD socket, if used, normally uses D4 as its chip-select.

## Install and use

1. In Arduino IDE, install/select the standard **Ethernet** library and select **Arduino Uno**.
2. Open `uno_ethernet_config.ino`. Change the `mac[]` value to a unique locally administered address for each device (keep the leading `02`).
3. Upload it, connect Ethernet, and open Serial Monitor at 9600 baud. It prints the assigned address, for example `http://192.168.1.50`.
4. From a computer or phone connected to the same LAN, visit that address. Select DHCP, or untick it and enter IP address, subnet mask, gateway, and DNS manually. Save and restart the Uno.

For the first connection, DHCP must be available or the shield uses its initial static fallback of `192.168.1.177`. In that case, temporarily put a computer on the same `192.168.1.x` subnet and visit that address.

## If Wi-Fi was the intent

An Uno R3 cannot use an Ethernet shield as Wi-Fi. Use an **Arduino WiFi Shield 101** (legacy), an ESP8266/ESP-01 module with suitable 3.3 V power and level shifting, or—more simply—an ESP32 instead of the Uno. Those options can present a Wi-Fi SSID/password setup page, but require different hardware and firmware.

The included page is intentionally unauthenticated. Use it only on a trusted local network; do not expose the Uno directly to the public Internet.
