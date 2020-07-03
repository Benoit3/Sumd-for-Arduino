# Sumd-for-Arduino

A simple implementation to decode Graupner Hott Sumd frame.

The code has been tested using :
- using an ESP8266 as an arduino. But it is not specific to it and shall run properly on any arduino with a serial port (just remove "include <ESP8266WiFi.h>" at the beginning)
- with RX serial input connected to the SUMD serial output of a GR16 receiver. Take care that the receiver shall be set correctly to ouput SUMD frame on a chosen channel
- the arduino console connected to the Tx output of the arduino (ESP8266 in our case) to display the channel values

It is now up to you to customize the code to control what you need with the received data for each channel.
