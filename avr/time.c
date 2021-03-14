#include "time.h"

#include <avr/io.h>

static uint16_t time_ticks = 0;

void time_init() {
    // Use Timer0 overflow interrupt for system ticks
    TCCR0B |= (1 << CS01);  // Prescale of 8
    TIMSK0 |= (1 << TOIE0);  // Interrupt on overflow (8bit)
}

uint16_t get_time_ticks() {
    return time_ticks;
}

void time_timer0_ovf_isr() {
    ++time_ticks;
}
