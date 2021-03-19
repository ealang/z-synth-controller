#include "led.h"
#include "bit_util.h"

#include <string.h>
#include <util/atomic.h>

static LedState *singleton = 0;

#define PIN_LED  5
#define DD_REG   DDRC
#define PORT_REG PORTC

#define COUNTER_NUM_BITS 6
#define COUNTER_MASK ((1 << COUNTER_NUM_BITS) - 1)

void led_init(LedState *state) {
    memset(state, 0, sizeof(LedState));
    singleton = state;
    BIT_SET(DD_REG, PIN_LED);
}

static uint8_t scale_user_brightness(uint8_t brightness) {
    return brightness >> (8 - COUNTER_NUM_BITS);
}

void led_set_brightness(uint8_t brightness) {
    singleton->default_brightness = scale_user_brightness(brightness);
}

void led_set_sequence(uint8_t brightness, uint8_t num_pulses, uint16_t pulse_length) {
    singleton->sequence.brightness = scale_user_brightness(brightness);
    singleton->sequence.pulse_length = pulse_length;
    singleton->sequence.pulse_countdown = pulse_length;
    singleton->sequence.frame_num = num_pulses * 2;
}

static char seq_update(LedSequenceState *state) {
    uint8_t brightness = state->frame_num % 2 == 0
        ? state->brightness : 0;

    if (--state->pulse_countdown == 0) {
        state->pulse_countdown = state->pulse_length;
        --state->frame_num;
    }

    return brightness;
}

void led_on_clock_tick_update() {
    char seq_in_progress = singleton->sequence.frame_num > 0;

    uint8_t brightness =
        seq_in_progress
        ? seq_update(&singleton->sequence)
        : singleton->default_brightness;

    singleton->counter = (singleton->counter + 1) & COUNTER_MASK;
    uint8_t new_led_val = singleton->counter >= brightness ? 0 : 1;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        BIT_WRITE(PORT_REG, PIN_LED, new_led_val);
    }
}
