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

* Installing Optiboot on Pro Micro
  * NOTE: The official Optiboot github repo supports the Atmega32U4 now.
    https://www.instructables.com/How-to-upload-sketches-to-Pro-MicroLeonardo-via-se/
  * Optiboot home: https://github.com/Optiboot/optiboot/wiki

