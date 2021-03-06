#define F_CPU 1000000UL
#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "shift_ctrl.h"


ISR (ADC_vect)
{
    uint8_t val = ADCH;
    PORTB = (PORTB & ~7) | (val >> 5);
}

int main() {
    sei();

    // Init LEDs
    DDRB = 0xFF;  // LEDS

    // Init shifter
    shift_ctrl_init();

    // Init ADC
    ADMUX = (1 << ADLAR);   // AVREF, ADC0, left adjusted
    ADCSRA |= (1 << ADEN);  // enable ADC
    ADCSRA |= (1 << ADIE);  // enable interrupt
    ADCSRA |= (1 << ADATE);  // enable auto-trigger
    ADCSRA |= (1 << ADSC);  // start conversion

    shift_ctrl_shift_word(0xF0F0);
    shift_ctrl_display();
    // uint8_t counter = 0;
    // while (1) {
    //   counter = (counter + 1) % 3;
    //   shift_ctrl_shift(counter <= 1 ? 1 : 0);
    //   shift_ctrl_display();
    //   _delay_ms(10);
    // }
    
    return 0;
}
