#include "shift_ctrl.h"
#include "bit_util.h"

#include <avr/io.h>

#define PIN_CLK     0
#define PIN_DATA    1
#define PIN_DISPLAY 2
#define PIN_RESET   3

#define ALL_PINS_MASK  0xF

void shift_ctrl_init() {
    MASK_SET(DDRB, ALL_PINS_MASK);
    MASK_CLEAR(PORTB, ALL_PINS_MASK);
    BIT_SET(PORTB, PIN_RESET);
}

void shift_ctrl_clear() {
    BIT_CLEAR(PORTB, PIN_RESET);
    BIT_SET(PORTB, PIN_RESET);
}

void shift_ctrl_shift(uint8_t bitval) {
    BIT_WRITE(PORTB, PIN_DATA, bitval);
    BIT_SET(PORTB, PIN_CLK);
    BIT_CLEAR(PORTB, PIN_CLK);
}

void shift_ctrl_shift_word(uint16_t value) {
    for (uint8_t i = 0; i < 16; ++i) {
        shift_ctrl_shift(value & 1);
        value >>= 1;
    }
}

void shift_ctrl_display() {
    BIT_SET(PORTB, PIN_DISPLAY);
    BIT_CLEAR(PORTB, PIN_DISPLAY);
}
