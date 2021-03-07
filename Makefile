MCU = atmega328p
PROGRAMMER = usbtiny
CFLAGS += -mmcu=$(MCU)

main: main.c shift_ctrl.h shift_ctrl.c adc_poll.c adc_poll.h bit_util.h spi_ifc.h spi_ifc.h
	avr-gcc $(CFLAGS) main.c shift_ctrl.c adc_poll.c spi_ifc.c -o main

.PHONY: clean
clean:
	rm -f main

.PHONY: avrdude-write
avrdude-write: main
	avrdude -c $(PROGRAMMER) -p $(MCU) -U flash:w:main

.PHONY: avrdude-ping
avrdude-ping:
	avrdude -c $(PROGRAMMER) -p $(MCU)
