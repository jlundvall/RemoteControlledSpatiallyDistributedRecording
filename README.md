# Remote controlled distributed audio recording system

This is a prototype for a remote controller developed in the fall semester of 2023 in association with an NTNU specialization project. It consists of up to 16 wirelessly connected nodes which can be used to trigger recordings on the following audio recorders:
- MixPre I and II series
- 833
- SQuadriga III

The nodes are all based on the Arduino Uno microcontroller, with one server node called the "controller node" and up to 15 client nodes called "recorder nodes". 

## HW components
For the controller node:
- Adafruit RFM69HCW Transceiver Radio Breakout - 433 MHz - RadioFruit 
- Adafruit DS3231 Precision RTC
- Adafruit SSD1306 OLED display
- DFRobot Gravity: Analog Sound Sensor For Arduino
- 3 button switches
- 2 LEDs

For each recorder node:
- Adafruit RFM69HCW Transceiver Radio Breakout - 433 MHz - RadioFruit
- 1 button switch
- 1 LED


## Instructions
# File placements
All the files inside _libraries_ must be placed inside your local _Arduino/libraries_ folder, along with all the libraries linked to in every file. They will be found in the .h files. Running the program on an Arduino Uno will highlight the missing libraries. 

# Firmware update on the Arduino Unos
Using an Arduino Uno as a USB keyboard requires a firmware update, which must be reversed again if the Arduino is to be programmed anew. Follow [this](https://www.youtube.com/watch?v=tvqA-JcTQNg) tutorial, using the files under _arduinoFirmwares_. 

# debug mode
Due to the fact that the arduinos are used as keyboards, that means that printing other things than scancodes give unforeseen results. Therefore, the debug mode variable is consulted before every print line to ensure that the intention is to actually print. The variable should only be true when the arduinos have their original firmware, not the keyboard firmware. 
