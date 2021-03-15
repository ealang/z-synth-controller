MCU = atmega328p
PROGRAMMER = usbtiny
CFLAGS += -mmcu=$(MCU) -DCLOCK_SPEED=8000000

main: main.c shift_ctrl.h shift_ctrl.c adc_poll.c adc_poll.h bit_util.h usart.c usart.h config.h time.h time.c led.h led.c button_input.h button_input.c
	avr-gcc -O3 $(CFLAGS) main.c shift_ctrl.c adc_poll.c usart.c time.c led.c button_input.c -o main

.PHONY: clean
clean:
	rm -f main

.PHONY: avrdude-write
avrdude-write: main
	avrdude -c $(PROGRAMMER) -p $(MCU) -U flash:w:main

.PHONY: avrdude-ping
avrdude-ping:
	avrdude -c $(PROGRAMMER) -p $(MCU)
