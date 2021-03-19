#ifndef LED_H
#define LED_H

#include <stdint.h>

typedef struct {
    uint8_t frame_num;
    uint16_t pulse_length;
    uint16_t pulse_countdown;
    uint8_t brightness;
} LedSequenceState;

typedef struct {
    uint8_t default_brightness;
    LedSequenceState sequence;
    uint8_t counter;
} LedState;

void led_init();
void led_set_brightness(uint8_t brightness);
void led_set_sequence(uint8_t brightness, uint8_t num_pulses, uint16_t pulse_len);
void led_on_clock_tick_update();

#endif
