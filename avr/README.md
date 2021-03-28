# z-synth-controller

Embedded controller board code. Runs on a ATmega328P. See Makefile.

Purpose:
- Read many potentiometers (32) using an ADC and shift registers
- Send updates over USART (tested with [Adafruit CP2104](https://www.adafruit.com/product/3309))
- Allow parameter saving/loading from EEPROM

Set to 8Mhz:
```
avrdude -c usbtiny -p atmega328p -U lfuse:w:0xE2:m
```

Preserve EEPROM during chip erase (optional):
```
avrdude -c usbtiny -p atmega328p -U hfuse:w:0xD1:m
```

5V brown-out detection (optional):
```
avrdude -c usbtiny -p atmega328p -U efuse:w:0xFB:m
```
