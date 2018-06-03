# Overview
This is a private repository for my project at WISEST lab during Summer 2018 as an undergraduate research assistant. This project is a dairy IoT, using chiefly Matlab and Arduino (C++). Matlab is responsible for gathering data from experiments and finding a calibration coefficients. Arduino programs a microcontroller board. The ultimate goal is to monitor cows' temperature and to wirelessly send the data through the internet.

# Requirements
1.	Arduino Uno connected to your PC via cable
2.	The RFID and sensor circuit 
3.	The sensor
4.	Matlab 2018a (earlier versions might not support all functionalities) installed 
5.	Matlab Support Package for Arduino Hardware installed and setup properly

# What this program does
This program gathers the number of samples and displays the raw data plot and the calibrated plot. Linear regression coefficients can be applied and adjusted before the reading starts. After the reading has done, the data will be save automatically and once again before the program is closed. The LED light indicates the data availability. It becomes yellow when there is a problem with the input data, becomes red if no input data is read, turns black when the program finishes or is ready. Once data is collected, it is saved to a CSV file with a timestamp.

# How to use
1.	Connect Arduino to your PC.
2.	Double-click on the program “Run_Improved_V2.mlapp”.
3.	Change port name to match the port Arduino connected to. You can also check with Device Manager in your Control Panel and look for Port section.
4.	Adjust the number of samples you wish.
5.	Click “Connect” button to open serial communication between your PC and Arduino.
6.	Place RFID antenna close to the sensor. The LED on the RFID reader should blink, indicating that there is a data.
7.	Click “Start” button when you are ready.
8.	Wait until the LED turns black again.
9.	If you wish to collect more data, simply click “Reset” again and press “Start”
10.	Repeat step 9-10.
11.	Once you are satisfied with data, close the window. Doing this prompts a dialogue block.
12.	Select “Yes” if you wish to exit and save data, or select “No” if you accidentally click on the close button.

# Troubleshooting

