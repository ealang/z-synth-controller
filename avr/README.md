# z-synth-controller

WIP physical controller for [z-synth](https://github.com/ealang/z-synth).

8Mhz clock assumed. Set clock for ATmega328P:
```
avrdude -c usbtiny -p atmega328p -U lfuse:w:0xE2:m
```
