#ifndef TIME_H
#define TIME_H

#include <stdint.h>

#ifdef CLOCK_SPEED
// 8 prescale, tick at timer0 overflow (8 bit)
#define MS_TO_TICKS(ms) ((uint16_t)(CLOCK_SPEED/(8 * 256.) * ms / 1000.0))
#endif

void time_init();
uint16_t get_time_ticks();
void time_timer0_ovf_isr();

#endif
