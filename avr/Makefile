MCU = atmega328p
PROGRAMMER = usbtiny

CDEFS += -DCLOCK_SPEED=8000000
CDEFS += -DADC_NUM_VALUES=32
CFLAGS += $(CDEFS) -mmcu=$(MCU) -Wall -Wextra -Wfatal-errors

SOURCES = $(shell find . -name '*.c')
HEADERS = $(shell find . -name '*.h')

main: $(SOURCES) $(HEADERS)
	avr-gcc -O3 $(CFLAGS) $(SOURCES) -o main

.PHONY: clean
clean:
	rm -f main

.PHONY: avrdude-write
avrdude-write: main
	avrdude -c $(PROGRAMMER) -p $(MCU) -U flash:w:main

.PHONY: avrdude-ping
avrdude-ping:
	avrdude -c $(PROGRAMMER) -p $(MCU)
