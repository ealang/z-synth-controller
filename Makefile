MCU = atmega328p
PROGRAMMER = usbtiny
CFLAGS += -mmcu=$(MCU)

main: main.c shift_ctrl.h shift_ctrl.c
	avr-gcc $(CFLAGS) main.c shift_ctrl.c -o main

.PHONY: clean
clean:
	rm -f main

.PHONY: avrdude-write
avrdude-write: main
	avrdude -c $(PROGRAMMER) -p $(MCU) -U flash:w:main

.PHONY: avrdude-ping
avrdude-ping:
	avrdude -c $(PROGRAMMER) -p $(MCU)
