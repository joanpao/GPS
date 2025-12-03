# GPS

Programs for the Lilygo T-Beam V1.2 (you can purchase it at a good price here:  
https://s.click.aliexpress.com/e/_c42M25rj)

This repository contains several programs adapted for the ESP32 Lilygo T-Beam v1.2 board.

It has been specifically tested with a **433 MHz version including OLED display and NEO-6M (GPS)**.

All programs use **TinyGPSPlus.h**, which must be added to your Arduino IDE libraries.

## Files

- **GPS.ino**  
  A simple program to test that the board is working. GPS data is sent via the serial port.

- **GPS_Display.ino**  
  An improved version of the previous program that also outputs data to the OLED display, allowing the board to be used with its battery.  
  For this program, you must include the following files in the project:
  - `Fixed8x16.h`
  - `Adafruit_SSD1306.h`
  - `Adafruit_SSD1306.cpp`  
  You must also have the following libraries installed in the Arduino IDE:
  - `SPI.h`
  - `Wire.h`

  Three output modes have been defined:
  - Static GPS
  - GPS while moving
  - Clock  
  By commenting or uncommenting sections, it can be adapted to your needs.

- **GPS_Beacon.ino**  
  An improved version of the previous one in which serial and OLED outputs have been commented out in order to include transmission of selected GPS data via LoRa encoded in Morse code.  
  This is based on the development in `morse-beacon.ino`.  
  You must include the following in your library:
  - `LoRa.h`
  - `LoRa.cpp`

## Future possibilities

Understanding the operation of the GPS chip included in the Lilygo T-Beam board opens the door to new developments such as:

- GPS-based NTP server
- WSPR transmitter (requires GPS for synchronization and a ham radio license)
- APRS

