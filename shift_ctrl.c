#include "shift_ctrl.h"

#include <avr/io.h>

#define PIN_CLK     0
#define PIN_DATA    1
#define PIN_DISPLAY 2
#define PIN_RESET   3

#define ALL_PINS_MASK  0xF

#define BIT_CLEAR(VAR, PIN) (VAR &= ~(1 << PIN))
#define BIT_SET(VAR, PIN) (VAR |= (1 << PIN))
#define BIT_WRITE(VAR, PIN, VALUE) (VAR = (VAR & ~(1 << PIN)) | ((VALUE & 1) << PIN))
#define MASK_CLEAR(VAR, MASK) (VAR &= ~MASK)
#define MASK_SET(VAR, MASK) (VAR |= MASK)

void shift_ctrl_init() {
    MASK_SET(DDRD, ALL_PINS_MASK);
    MASK_CLEAR(PORTD, ALL_PINS_MASK);
    BIT_SET(PORTD, PIN_RESET);
}

void shift_ctrl_clear() {
    BIT_CLEAR(PORTD, PIN_RESET);
    BIT_SET(PORTD, PIN_RESET);
}

void shift_ctrl_shift(uint8_t bitval) {
    BIT_WRITE(PORTD, PIN_DATA, bitval);
    BIT_SET(PORTD, PIN_CLK);
    BIT_CLEAR(PORTD, PIN_CLK);
}

void shift_ctrl_shift_word(uint16_t value) {
    for (uint8_t i = 0; i < 16; ++i) {
        shift_ctrl_shift(value & 1);
        value >>= 1;
    }
}

void shift_ctrl_display() {
    BIT_SET(PORTD, PIN_DISPLAY);
    BIT_CLEAR(PORTD, PIN_DISPLAY);
}
