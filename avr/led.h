#ifndef LED_H
#define LED_H

#include <stdint.h>

void led_init();
void led_set_brightness(uint8_t brightness);
void led_timer_isr();

#endif
