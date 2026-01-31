# esphome-mideaXYE-rs485
ESPhome files to communicate with Midea AC units via RS485 XYE (CCM) terminals.

This project will allow you to wirelessly communicate with your Midea AC unit via the XYE terminals.  These terminals are normally used for communication with a Central Controller Module (CCM) but the data has been reverse engineered (https://codeberg.org/xye/xye).

# Requirements
## Hardware
1. ESP32 device - The m5stack [Atom Lite](https://shop.m5stack.com/products/atom-lite-esp32-development-kit) with the [Tail485 adapter](https://shop.m5stack.com/products/atom-tail485) are recommended for easy setup, especially if you are not experienced with esp32. 
2. Some thermostat wire or 22 gauge wire
3. If you use different hardware:
    * You need an RS-485 transceiver with **automatic flow control** (if not you will need to edit my code to switch flow control manually after serial read/write)
4. Midea-like heat pump or air handler with **XYE terminals**.
    * If your system supports 24v communication, make sure its dip switches are configured for RS485
    * Similarly, the system will work best if you have RS485 communication between the indoor and outdoor units in a split system (not relevant for mini splits)

## Software
1. ESPHome
2. Home Assistant

# Setup
## Software
1. Flash your esp32 device with ESPHome
2. Modify the default boilerplate as needed with your wifi info and whatever else you prefer on your devices
3. Below the standard boilerplate, paste the following:
```yaml
packages:
  esphome-mideaXYE-rs485:
    url: https://github.com/wtahler/esphome-mideaXYE-rs485
    ref: main
    files:
      - path: midea_xye.yaml
        vars:
          tx_pin: 26
          rx_pin: 32
```
4. Modify the `tx_pin` and `rx_pin` to match your device. 

## Hardware
1. Open up your air handler and find the XYE terminal.
2. On your RS485 transceiver, connect A -> X, B -> Y, and E -> Ground. Note that on most midea systems the XYE terminal comes off, which makes it much easier to access the screws.
3. If your transceiver accepts 12v power, you can connect it to the HA terminal. If it accepts 24v power and you have a split system, you can connect it to the thermostat R terminal. Make sure you verify voltages of your specific air handler before connecting your esp32 to it. You could potentially fry your esp device!
    * If your air handler does not provide power compatible with your transceiver, you will have to power it externally with USB or some other power source.

# Configuration Variables
| Name | Type | Default | Description |
| -------- | -------- | -------- | -------- |
| **rx_pin** | GPIO Pin | N/A | RX pin for your RS485 Transceiver |
| **tx_pin** | GPIO Pin | N/A | TX pin for your RS485 Transceiver |
| **baud_rate** | int | `4800` | Baud rate--shouldn't need to change this |
| **use_fahrenheit** | bool | `true` | Whether to use F or C for the set point. Note that all other values will always be reported in C. You may need to set the set point again after changing this |
| **follow_me_timeout** | int | `7200` | Time in seconds after which to disable follow me if you have not updated the temperature. This is to prevent locking the system on or off if home assistant goes offline |
