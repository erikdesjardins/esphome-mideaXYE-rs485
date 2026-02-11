# esphome-mideaXYE-rs485

This is a fork of https://github.com/wtahler/esphome-mideaXYE-rs485, which I have heavily modified for reverse engineering and to support my specific setup (Midea DLCMHBH36DAK outdoor unit with DLFSHC indoor heads).

Unless you are working on reverse engineering, I would suggest using one of the projects below instead. I will likely not implement features here that I cannot test on my own system. (Also, we should concentrate on a single implementation so work isn't spread out.)

- https://github.com/wtahler/esphome-mideaXYE-rs485
- https://github.com/mdrobnak/esphome/tree/delays_updated/esphome/components/midea_xye

This repo builds on the work of many people. In no particular order:

- https://codeberg.org/xye/xye (original XYE protocol reverse engineering work)
- https://github.com/Bunicutz/ESP32_Midea_RS485 (early ESP32 implementation)
- https://github.com/wtahler/esphome-mideaXYE-rs485 (implementation based on the previous)
- https://github.com/wtahler/esphome-mideaXYE-rs485/pull/11 (reworked version of the previous--what this codebase is based on)
- https://github.com/mdrobnak/esphome/tree/delays_updated/esphome/components/midea_xye (alternate implementation, additional RE work)
- https://github.com/HomeOps/ESPHome-Midea-XYE (fork of the previous, additional RE work)

---

ESPhome files to communicate with Midea AC units via RS485 XYE (CCM) terminals.

This project will allow you to wirelessly communicate with your Midea AC unit via the XYE terminals.  These terminals are normally used for communication with a Central Controller Module (CCM) but the data has been reverse engineered (https://codeberg.org/xye/xye).

# Requirements

## Hardware
1. ESP32 board and RS485 transceiver with automatic flow control.
    * The m5stack [Atom Lite](https://shop.m5stack.com/products/atom-lite-esp32-development-kit) with the [Tail485 adapter](https://shop.m5stack.com/products/atom-tail485) is recommended.
2. Some thermostat wire or 22 gauge wire.
3. Midea-like heat pump or air handler with **XYE terminals**.
    * If your system supports 24v communication, make sure its dip switches are configured for RS485.

## Software
1. ESPHome
2. Home Assistant

# Setup
## Software
1. Flash your ESP32 device with ESPHome.
2. Modify the default boilerplate as needed.
3. Below the default boilerplate, paste the following, modifying the config to match your device:
```yaml
packages:
  esphome-mideaXYE-rs485:
    url: https://github.com/erikdesjardins/esphome-mideaXYE-rs485
    ref: main
    files:
      - path: midea_xye.yaml
        vars:
          tx_pin: 26
          rx_pin: 32
```

## Hardware
1. Open up your air handler and find the XYE terminal.
2. On your RS485 transceiver, connect A -> X, B -> Y, and E -> Ground.
3. If your transceiver accepts 12v power, you can connect it to the HA terminal. If it accepts 24v power and you have a split system, you can connect it to the thermostat R terminal. Make sure you verify voltages of your specific air handler before connecting your esp32 to it. You could potentially fry your esp device!
    * If your air handler does not provide power compatible with your transceiver, you will have to power it externally with USB or some other power source.

# Configuration Variables
| Name | Type | Default | Description |
| -------- | -------- | -------- | -------- |
| **rx_pin** | GPIO Pin | N/A | RX pin for your RS485 Transceiver |
| **tx_pin** | GPIO Pin | N/A | TX pin for your RS485 Transceiver |
| **baud_rate** | int | `4800` | Baud rate (Shouldn't need to change this.) |
| **use_fahrenheit** | bool | `true` | Whether to use °F or °C for the set point. Note that all other values will always be reported in °C. (May need to set the set point again after changing this.) |
| **follow_me_timeout** | int | `1800` | Time in seconds after which to disable Follow Me if you have not updated the temperature. (This is to prevent locking the system on or off if Home Assistant goes offline.) Note: because of this, the same automation which sets the Follow Me temperature should also turn on the Follow Me switch, so it will automatically recover from such situations. |
