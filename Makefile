MCU = atmega328p
PROGRAMMER = usbtiny
CFLAGS += -mmcu=$(MCU)

main: main.c
	avr-gcc $(CFLAGS) main.c -o main

.PHONY: clean
clean:
	rm -f main

.PHONY: avrdude-write
avrdude-write: main
	avrdude -c $(PROGRAMMER) -p $(MCU) -U flash:w:main

.PHONY: avrdude-ping
avrdude-ping:
	avrdude -c $(PROGRAMMER) -p $(MCU)
