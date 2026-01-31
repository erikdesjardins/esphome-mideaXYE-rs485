# esphome-mideaXYE-rs485
ESPhome files to communicate with Midea AC units via RS485 XYE (CCM) terminals.

This project will allow you to wirelessly communicate with your Midea AC unit via the XYE terminals.  These terminals are normally used for communication with a Central Controller Module (CCM) but the data has been reverse engineered (https://codeberg.org/xye/xye).  I initially used the code from bunicutz project https://github.com/Bunicutz/ESP32_Midea_RS485/tree/main, but as of ESPHome 2025.2.0 custom components are no longer supported and that code does not work.  This project doesn't use custom components, just several lambda functions and a header file.  I'm not a C++ developer, so forgive me if the code isn't very elegant. It gets the job done though!

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
    * <details>
        <summary>Spoiler: Image</summary>
      
        ![XYE Terminal Photo](https://github.com/user-attachments/assets/d5fb2233-c0dc-42a9-9eae-0333b8cd70b1)
      </details>

3. On your RS485 transceiver, connect A -> X, B -> Y, and E -> Ground. Note that on most midea systems the XYE terminal comes off, which makes it much easier to access the screws.
4. If your transceiver accepts 12v power, you can connect it to the HA terminal (included in photo above). If it accepts 24v power and you have a split system, you can connect it to the thermostat R terminal. Make sure you verify voltages of your specific air handler before connecting your esp32 to it. You could potentially fry your esp device!
    * If your air handler does not provide power compatible with your transceiver, you will have to power it externally with USB or some other power source.

# Home Assistant
The device that is created will have the following entities:

| Name | Type | Description |
| -------- | -------- | -------- |
| **Fan Mode** | Select | Select the fan mode |
| **Follow Me** | Button | Tell the unit what the current indoor temperature is. Used in conjunction with the Follow Me Temp entity |
| **Follow Me Temp** | Number | The temperature we should pass to the indoor unit. |
| **Operating Mode** | Select | Select the operating mode of the unit |
| **Set Point** | Number | The set point of the HVAC system |
| **Compressor status** | Binary Sensor | Whether the outdoor unit is on or off |
| **Defrost** | Binary Sensor | Whether the outdoor unit is currently defrosting|
| **Error Codes** | Sensor | The error codes from the device displayed in hex. Should match what you see on the indoor unit if it has a display  |
| **Fan Mode** | Sensor | The current fan mode we've read from the device. If in auto fan mode it will indicate which speed the unit has chosen |
| **Operating Mode** | Sensor | The current operating mode we've read from the device |
| **T1 Inlet/Room Temp** | Sensor | Either the temperature of the return air sensor in your air handler, or the follow me temperature value we sent to the device |
| **T2A Indoor Coil Temp A** | Sensor | One of two indoor coil temperatures. In heating mode it should be after the air has passed over the coil. In cooling mode it should be before the air has passed over the coil |
| **T2B Indoor Coil Temp A** | Sensor | Same as above, except the opposite order |
| **T3 Outdoor Coil Temp** | Sensor | Temperature of the outdoor coil. Used mostly by the outdoor unit to determine when to defrost |
| **T4 Outdoor Temp** | Sensor | Outdoor temp--I wouldn't expect too much accuracy from this sensor |

# Configuration Variables
| Name | Type | Default | Description |
| -------- | -------- | -------- | -------- |
| **rx_pin** | GPIO Pin | N/A | RX pin for your RS485 Transceiver |
| **tx_pin** | GPIO Pin | N/A | TX pin for your RS485 Transceiver |
| **baud_rate** | int | `4800` | Baud rate--shouldn't need to change this |
| **use_fahrenheit** | bool | `true` | Whether to use F or C for the set point. Note that all other values will always be reported in C. You may need to set the set point again after changing this |
| **follow_me_timeout** | int | `7200` | Time in seconds after which to disable follow me if you have not updated the temperature. This is to prevent locking the system on or off if home assistant goes offline |
| **log_rs485_comms** | bool | `true` | Whether or not to log the RS485 communications. If you need to submit logs with a bug report please keep this enabled |

# How it works
The code sends a `query` set every 15s.  It checks `mySerial.available()` every 1s and reads data (there will only be data there after sending a query or a command).  The "receiver" sensor is running the 1s read check and updating internal variables with the read values.  Each of the actual sensors (temperatures, setpoints, modes, etc) are checking the internal values every 1s.  If any of the input elements are changed from home assistant, they fire a lambda function that forms a `command` set and writes it to the serial bus.  The response to a command to change something is always the old state (not sure why) so I dumped the first response after sending a command. 

# Debugging
Change your log level to DEBUG in the esphome yaml file if you want to see more info.  If you set it to "info" then you'll just get the little debug lines I wrote that will tell you what the program is currently doing (or **thinks** it's doing!).  You can add more `ESP_LOGI("custom","log this to the terminal!")` lines anywhere you want to get more feedback of what's going on.

# Background
I have a Carrier heatpump (38MARB) and ducted air handler (40MBAB), and the heat pump is a rebranded Midea unit.  The included controls are fine, but I wanted to add wireless control and integrate with Home Assistant without using a 24V thermostat.  I can't find anything definitive but I suspect using a 24V thermostat would prevent any benefits from the variable frequency drive of the heat pump (the unit won't know the setpoint so it won't know how to modulate).  

There are already ESPHome components for communication with Midea heat pumps via H+/- terminals and IR, but my controller is HA/HB which is not compatible with the hardwired code for H+/- and I wanted 2-way communication of data which isn't possible with the IR component.  I found bunicutz's code for XYE terminals and discovered my air handler had these terminals, so I tried it out and it worked! Unfortunately, it relies on custom components so I had to rewrite it without them using lambda functions. 

# Things I've discovered
- Huge resource has been these training videos: https://siglernorcal.com/video-category/
- These heat pumps and air handlers have a built-in offset of anywhere between 2-4 degrees Celsius to try to account for stratification of the air.  This MAY be configurable on SOME units (I think wall units) but I couldn't change it with my air handler.  Reference p.34: https://cematraining.com/wp-content/uploads/2021/06/CEs-Carrier-Bryant-DLS-Service-Manual.pdf
- That same reference shows the "operating band" and it's pretty big, several degrees Celsius.  So if you feel like you're getting big swings in temperature, that's why. Possibly configurable with a remote, I can't tell if I was able to change anything (no feedback mechanism).
- If you look a few pages further in that reference you'll see that with the remotes you can change some of these factory settings. This is also discussed in the wireless remote manual (RG10 for me) but uses a different set of codes.  Neither method worked for me... but in this video he uses it to change settings on a wall unit: https://siglernorcal.com/aiovg_videos/service-remote-7-of-16/
- If you have the same air handler as me (40MBAB) and it is installed on it's RIGHT side (right horizontal in the installation manual) then the T1 sensor has to be pulled out and moved to the other side of the coils.  Your installer should know what that means, but it's not in the manual.  If you don't, then your unit will sense T1 AFTER the coils so when it turns on it will immediately think the room got hot/cold and turn right back off. If it's acting crazy, that's probably why!  This video explains it: https://siglernorcal.com/aiovg_videos/understanding-the-t1-sensor/
- Even in "follow me" mode the unit will include the T1 sensor in the calculation with a 30% weight. Found that in a random video, and can confirm based on behavior of my unit.  When the T1 was in the wrong spot and reading 90F when heating it skewed my follow-me temperature way up even though the wall unit was seeing 67F.  Once my installer moved the T1 sensor it's much happier.
- I've probably got other resources where I've found some nugget of information... just not thinking of everything right now. Feel free to ask!
