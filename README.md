# DIY_BKM10R

Reproducing the main functionality of a SONY BKM 10R using the following components:

## Hardware
* 1x Raspberry Pi Pico
* 1x Makerbase MKS Mini 12864 v3 display module
* 2x MAX485 RS485 module 
* 1x Infrared Receiver
* 2x IDC connector (10 pin)
* 1x DSub 9 connector (female)
* 1x perfboard

## Software
* Arduino-Pico Core (https://github.com/earlephilhower/arduino-pico)
* U8G2 display library (https://github.com/olikraus/u8g2)
* Rotary Encoder library (https://github.com/mathertel/RotaryEncoder)
* Adafruit NeoPixel library (https://github.com/adafruit/Adafruit_NeoPixel)
* IRRemote library (https://github.com/Arduino-IRremote/Arduino-IRremote)

The communication to the monitor is based on the excellent work of Martin Hejnfelts BKM-10rlternative (https://github.com/skumlos/bkm-10rlternative).

https://www.youtube.com/watch?v=oZMb17oDjyI
