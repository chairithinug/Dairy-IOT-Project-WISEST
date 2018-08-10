# Overview
This is a private repository for my project at WISEST lab during Summer 2018 as an undergraduate research assistant. This project is a dairy IoT, using chiefly MATLAB and Arduino (C++). MATLAB is responsible for gathering data from experiments and finding calibration coefficients. Arduino programs a microcontroller board. The goal is to monitor the core temperature of livestock and to wirelessly send the data through the internet.

# Components used
1. Adafruit Feather M0 LoRa MCU x2
2. ESP8266 WiFi Module
3. Priority1 RFIDRW-E-TTL Reader with an antenna
4. Bio temperature sensors
5. UBBL24-Fl-TH 7.4V 4800 mAh Batteries
6. MCP1702-3303 LDO
7. VN4012 NMOS
8. 1 uF Capacitors
9. Clipping Diodes (not used previously but recommended on RX-TX)

# Softwares used
1. Arduino IDE
2. Matlab R2018a
3. Altium Designer
4. Git
5. Microsoft Office
6. Google Chrome

# Libraries used
1. RH_RF95 LoRa library
2. RTCZero Timer/Low power library
3. SPI (may not actually needed)

# Codes
1. LoRa_RX_Multi		HUB's code
2. LoRa_TX_RTC_V2		Device's code
3. LoRa_TX_NMOS			Debugging code without a sleep mode
4. Online_Matlab_Code	Thingspeak code

# Schematics
1. High-level Schematics
2. Detailed Schematics of a Hub
3. Detailed Schematics of a Device

# Useful resources
1. ESP8266 AT Command
2. Adafruit Feather M0 LoRa schematics
3. Datasheets of components

# Words of caution when integrating parts
1. Do not connect V+ and GND of a battery. It could explode.
2. Be careful when using a DMM. Do not let a probe touch any 2 pins. This creates a short circuit.
3. Unplug or turn off a power supply first before removing any components.
4. MCU and IC are sensitive parts. Be especially careful when connecting them and notice the Vin of them.
5. When doing a hardware debug, start where the problem most likely happens.

# Setting up a device
1. Program a device. Double-push the reset button and wait for a LED to blink, if needed.
2. Be careful when messing with Sleep mode/Standby mode of MCU. It might brick the device.
3. 
4.
5.
6. If needed, use LoRa_TX_NMOS to debug a device. That program does not have a sleep mode in it.

# Setting up a hub

# Setting up a Thingspeak.com
1. Create a channel that takes inputs from a hub. And then use Matlab Analysis to analyze the data.
2. 
3.
4.
5.


# Troubleshooting guide
1. Check the antenna connection.
2. Check the power connection and power level.
3. Check the prototype board connection.
4. Check the software problem.
5. Use a DMM to test a voltage at any points.
6. Use an o

Contact: Anapat Chairithinugull Email: chairithinug@wisc.edu
