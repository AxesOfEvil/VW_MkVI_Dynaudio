# Dynaudio Amplifier control for VW MkVI GTI

The MkVI Volkswagen GTI uses CAN bus commands to power on/off as well as
for fade/balance settings

This means that an aftermarket head-unit may not be able to power-on the
amplifier, and thus no sound will be heard.

This project addresses this issue by sending the appropriate power on/off 
commands to the Stock Dynaudio amplifier when the 'Amp Power' wire has
12V on it (alternately, attach the common 'Accessory' wire if the head-unit
has no 'Amp Power' wire)

# How it works
The cirucit uses a self-latching power circuit for low-power operation.
When 12V is detected on the 'Amp Power' wire, the circuit will power up.
The Arduino will then keep the circuit powered even if 'Amp Power' is turned
off.

When the Arduino detects the 'Amp Power' at 12V, it will send the Dynaudio
amplifier the power up signals via the CAN bus, and go to sleep.

When the 'Amp Power' wire drops to 0V, the Arduino will wakeup, send the
power-down sequence to the Dynaudio amplifier, and then turn itself off.
Once off, the only way to power up the Arduino again is to apply 12V to the
'Amp Power' wire.

# About the schematic
An AVR based arduino is used because of its 5V tolerance. Specifically,
either a 'Pro Micro' or 'Pro Mini' Arduino is recommended.  It should not
matter if the Arduino board is a 5V or 3.3V variant (both are capable of being
powered at 5V).

The LDO can be replaced with a DC/DC supply to further reduce current, though
the difference is only ~100mW of power (when active, the circuit consumes
negligible power when the 'Power Amp' wire is inactive)

I prefered to use an Arduino 'Pro Micro' instead of the 'Pro Mini' primarily
because it has a USB interface that could be used in the future to control
additional CAN bus functions.  However, the 'Pro Micro' uses USB programming
which does not play very nicely with the low-power states used by software.
When using a 'Pro Micro' it is recommended, to flash the 'Optiboot' firmware
such that the board can be programmed via Serial1 instead of via USB.

# Building the software
The software uses the following Arduino libraries:

* `Arduino-MCP2515` : https://github.com/autowp/arduino-mcp2515
* `Low Power`

# CAN bus protocol
The Dynaudio amplifier listens on interface 0x6C7 for amplifier commands.
As the commands were snooped from a working head-unit, their individual
function is not fully known.

Power up seems to require a minimum of 5 frames, with the 1st byte being
0x1B or 0x2B.  After power-on, and additional 13 frames are sent to set the
fade/balance/etc settings, but the meaning of these frames is not yet known.

Power-down requires only a single frame (with 1st byte = 0x2b)

# VW Media Can bys data (100kbps)

| Identifier | Data                    | Description |
| -----------| ----------------------- | ----------- |
| 0x5C1      | 06 00 00 60             | vol up (repeats periodically while button is held) |
| 0x5C1      | 07 00 00 60             | vol down (repeats periodically while button is held) |
| 0x5C1      | 03 00 00 60             | left button (repeats periodically while button is held) |
| 0x5C1      | 02 00 00 60             | right button (repeats periodically while button is held) |
| 0x5C1      | 2A 00 00 60             | mic button (repeats periodically while button is held) |
| 0x5C1      | 1A 00 00 60             | phone button (repeats periodically while button is held) |
| 0x5C1      | 29 00 00 60             | return button (repeats periodically while button is held) |
| ---------- | ----------------------- | ----------- |
| 0x6C7      | 80 04 2B 57 00 05 01 xx | Dynaudio amp left/right fade (8-bit signed int. 0x80 = full left, 0x7f = full right |
| 0x6C7      | 80 04 2B 58 00 05 01 xx | Dynaudio amp front/back balance (8-bit signed int. 0x80 = full back, 0x7f = full front |
| 0x6C7      | 2B 50 00                | Dynaudio amp power down |
| 0x6C7      | 1B 42                   | Dynaudio amp ??? (powerup frame 1) |
| 0x6C7      | 1B 43                   | Dynaudio amp ??? (powerup frame 2) |
| 0x6C7      | 1B 41                   | Dynaudio amp ??? (powerup frame 3) |
| 0x6C7      | 2B 50 01                | Dynaudio amp ??? (powerup frame 4) - power up ? |
| 0x6C7      | 2B 53 00                | Dynaudio amp ??? (powerup frame 5) |
| 0x6C7      | 80 07 2B 55 78 00 00 00 | Dynaudio amp ??? (powerup frame 6) - resent periodically |
| 0x6C7      | C0 22 22 00             | Dynaudio amp ??? (powerup frame 7) - resent periodically |
| 0x6C7      | 80 07 2B 51 00 00 04 05 | Dynaudio amp ??? (powerup frame 8) |
| 0x6C7      | C0 0A 08 FF             | Dynaudio amp ??? (powerup frame 9) |
| 0x6C7      | 80 03 1B 52 00 00 0D    | Dynaudio amp ??? (powerup frame 10) |
| 0x6C7      | 80 03 1B 54 00 00 0D    | Dynaudio amp ??? (powerup frame 11) |
| 0x6C7      | 80 03 1B 57 00 00 0D    | Dynaudio amp ??? (powerup frame 12) |
| 0x6C7      | 80 03 1B 58 00 00 0D    | Dynaudio amp ??? (powerup frame 13) |
| 0x6C7      | 80 03 1B 59 00 00 0D    | Dynaudio amp ??? (powerup frame 14) |
| 0x6C7      | 80 03 1B 5A 00 00 0D    | Dynaudio amp ??? (powerup frame 15) |
| 0x6C7      | 80 03 1B 5B 00 00 0D    | Dynaudio amp ??? (powerup frame 16) |
| 0x6C7      | 80 03 1B 5C 00 00 04    | Dynaudio amp ??? (powerup frame 17) |
| 0x6C7      | 80 03 1B 5E 00 00 04    | Dynaudio amp ??? (powerup frame 18) |
| ---------- | ----------------------- | The following data was collected by hugovw1976 (see link below) |
| 0x2C3      | aa                      | aa: bit0 power on |
| 0x635      | aa bb cc                | aa: bit0 lights on |
| 0x466      | aa bb cc dd ee ff gg hh | bb: door-open - bit0: driver, bit1: passenger, bit2: rear-driver, bit3: rear passenger, bit4: hood, bit5: trunk |
| 0x35B      | aa bb cc dd ee ff gg hh | ccbb: RPM*4 (unconfirmed) |
| 0x527      | aa bb cc dd ee ff gg hh | ccbb: Speed (units?) (unconfirmed), ff: (outside temperature(C) +50)/2 |
| 0x621      | aa bb cc dd ee ff gg    | aa: bit 2: wiper state, dd: fuel level (liters) (bit 7 is set to indicate low-fuel when <10l) |
| 0x65D      | aa bb cc dd ee ff gg hh | ddeecc: odometer (unconfirmed) |
| 0x351      | aa bb cc dd ee ff gg hh | aa: bit0: reverse (unconfirmed), cc: bit0: brake (unconfirmed)|
| 0x571      | aa bb cc dd ee ff       | aa: (battery voltage * 10 + 50) / 2 |
| 0x151      | aa bb cc dd             | bb: seat-belt (unconfirmed) |

# Additional Resources
* Using a Raspberry Pi to snoop the CAN bus with a $3 Ebay MCP2515
  * This modification allows running the SPI interface at 3.3V while running
    the CAN interface at 5V: https://forums.raspberrypi.com/viewtopic.php?t=141052
  * Savvy CANbus GUI: https://www.savvycan.com/

* Pinout for VW MkBI head unit
  * https://pinoutguide.com/Car-Stereo-Volkswagen-Porsche/Volkswagen_RCD_310_Head_U_pinout.shtml
  * https://connector.pinoutguide.com/40__16_12_12__pin_Head_Unit_Car_Stereo_Quadlock/

* Interesting articles on CAN bus hacking
  * General overview: https://theksmith.com/software/hack-vehicle-bus-cheap-easy-part-2/
  * Analyzing a VW Gauge cluster using CAN bus hacking (also shows how to use
    the 'savvy' CAN bus analyzer GUI: https://www.youtube.com/watch?v=GOuglqFtmOE&t=23s
  * CANid database for various vendors: https://github.com/iDoka/awesome-automotive-can-id
  * DTB files for CANids for various vendors: https://github.com/commaai/opendbc
  * XDA posting by hugovw1976 with some Golf6 Canids: https://forum.xda-developers.com/t/mtcd-kgl-px5-canbus-problem-with-vw-golf-6.3632690/post-73939457

* Installing Optiboot on Pro Micro
  * NOTE: The official Optiboot github repo supports the Atmega32U4 now.
    https://www.instructables.com/How-to-upload-sketches-to-Pro-MicroLeonardo-via-se/
  * Optiboot home: https://github.com/Optiboot/optiboot/wiki


