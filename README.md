# Overview
This is a private repository for my project at WISEST lab during Summer 2018 as an undergraduate research assistant. This project is a dairy IoT, using chiefly Arduino (C++/C) and MATLAB. MATLAB is responsible for gathering data from experiments and finding calibration coefficients. Arduino programs a microcontroller board. The goal is to monitor the core temperature of livestock and to wirelessly send the data through the internet.

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
10. Wires
11. Prototype board
12. Header pins

# Useful electronic devices
1. Soldering station
2. Power supply
3. DMM
4. Oscilloscope

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

# Codes (inside Codes folder)
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
4. Some files in Extras folder

# TODO-Issue list
1. Change a number of devices connected to the hub.
2. Change a period of the reading.
3. Collision handling could be better by echoing some parts of data sent from a device to a hub back to the device.
4. Floating pin on a reader when NMOS creates an open circuit will need to be addressed. Now the current likely goes through RX-TX, which is not good.
5. There is still a grounding problem with a device and it can pick up a wrong part of data to be sent or receive a wrong data.
6. Sending all reading data to the hub might be a good idea than processing some of them on a device. This comes at a cost of power consumption.
7. Make a solid protocol between devices and a hub and across all devices.

# Words of caution when integrating parts
1. Do not connect V+ and GND of a battery. It could explode.
2. Be careful when using a DMM. Do not let a probe touch any 2 pins. This creates a short circuit.
3. Unplug or turn off a power supply first before removing any components.
4. MCU and IC are sensitive parts. Be especially careful when connecting them and notice the Vin of them.
5. When doing a hardware debugging, start at which the problem most likely happens.

# Setting up a device
1. Change period, LoRa signal power, and other variables if needed.
2. Program a device. Double-push the reset button and wait for a LED to blink, if needed.
3. Be careful when messing with Sleep mode/Standby mode of MCU. It might brick the device. 
4. In a standby mode (sleep mode), you cannot really use serial communication with a PC since the sleep mode cut the communication.
5. If needed, use LoRa_TX_NMOS to debug a device. That program does not have a sleep mode in it, while still printing a reading through a serial communication.

# Setting up a hub
1. Change a number of devices, sample per wake or other variables if needed.
2. Program a hub.
3. The LED blinks every time a hub gets any inputs.

# Setting up a Thingspeak.com
1. Create a channel that takes inputs from a hub.
2. Read data using readAPIkey and channelID.
3. Use Matlab Analysis to analyze the data.
4. Post data to a different channel using writeAPIkey and channelID.
5. Edit charts and set public view.
6. If needed, data can be exported in a CSV file.

# Charging batteries guide
1. Use a power supply and set a constant voltage (max voltage) and a constant current (1 A recommended)
2. Do not a power supply directly to a battery (for safety reason?)
3. Use a prototype board and a JST connector to create a charging device.

# Troubleshooting guide
1. Use a DMM to test a voltage at any points.
2. Use an oscilloscope to check UART communication.
3. Measure an impedance of an antenna.
4. Print out a reading and such on a serial communication.
5. Look at inputs from a hub to Thingspeak.
6. Check Thingspeak code.

# Locations of possible problems
1. Check the antenna connection.
2. Check the power connection and power level.
3. Check the software problem.
4. Check the prototype board connection.

# Final words
This project was not possible without Prof. Kim's, Jingjie's, Hanwook's and others' helps. I really appreciate all helps and advices I received and I am grateful to be given a trust to handle this project.

Contact: Anapat Chairithinugull Email: chairithinug@wisc.edu Cell: +1 6087729985