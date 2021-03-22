# z-synth-controller

WIP physical controller for [z-synth](https://github.com/ealang/z-synth).

8Mhz clock assumed. Set clock for ATmega328P:
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
