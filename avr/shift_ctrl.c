#include "shift_ctrl.h"
#include "bit_util.h"

#include <avr/io.h>
#include <util/atomic.h>

#define DD_REG     DDRC
#define PORT_REG   PORTC

#define PIN_CLK     1
#define PIN_DATA    2
#define PIN_DISPLAY 3
#define PIN_RESET   4

#define ALL_PINS_MASK  (MASK(PIN_CLK) | MASK(PIN_DATA) | MASK(PIN_DISPLAY) | MASK(PIN_RESET))

void shift_ctrl_init() {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        MASK_SET(DD_REG, ALL_PINS_MASK);
        MASK_CLEAR(PORT_REG, ALL_PINS_MASK);
        BIT_SET(PORT_REG, PIN_RESET);
    }
}

void shift_ctrl_clear() {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        BIT_CLEAR(PORT_REG, PIN_RESET);
        BIT_SET(PORT_REG, PIN_RESET);
    }
}

void shift_ctrl_shift(uint8_t bitval) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        BIT_WRITE(PORT_REG, PIN_DATA, bitval);
        BIT_SET(PORT_REG, PIN_CLK);
        BIT_CLEAR(PORT_REG, PIN_CLK);
    }
}

void shift_ctrl_display() {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        BIT_SET(PORT_REG, PIN_DISPLAY);
        BIT_CLEAR(PORT_REG, PIN_DISPLAY);
    }
}
