#include "led.h"
#include "bit_util.h"

#include <util/atomic.h>

static uint8_t _brightness = 0;
static uint8_t _counter = 0;

#define PIN_LED  5
#define DD_REG   DDRC
#define PORT_REG PORTC

void led_init() {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        BIT_SET(DD_REG, PIN_LED);
    }
}

void led_set_brightness(uint8_t brightness) {
    _brightness = brightness >> 2;
}

void led_isr() {
    _counter = (_counter + 1) & 0x3F;
    uint8_t new_led_val = (_counter >= _brightness) ? 0 : 1;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        BIT_WRITE(PORT_REG, PIN_LED, new_led_val);
    }
}


