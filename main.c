#define F_CPU 1000000UL
#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>


ISR (ADC_vect)
{
    uint8_t val = ADCH;
    PORTB = (PORTB & ~7) | (val >> 5);
}

int main() {
    sei();

    DDRB = 0xFF;  // LEDS
    DDRD = 0xFF;  // Shift reg ctrl
    PORTD = 0;

    ADMUX = (1 << ADLAR);   // AVREF, ADC0, left adjusted

    ADCSRA |= (1 << ADEN);  // enable ADC
    ADCSRA |= (1 << ADIE);  // enable interrupt
    ADCSRA |= (1 << ADATE);  // enable auto-trigger
    ADCSRA |= (1 << ADSC);  // start conversion

    #define PIN_SHIFT_CLK     0
    #define PIN_SHIFT_DATA    1
    #define PIN_SHIFT_REFRESH 2
    #define PIN_SHIFT_RESET   3

    PORTD &= ~(1 << PIN_SHIFT_RESET);
    _delay_ms(10);
    PORTD |= (1 << PIN_SHIFT_RESET);

    uint8_t counter = 0;
    while (1) {
        for (uint8_t i = 0; i < 16; ++i) {
            counter = (counter + 1) % 3;
            uint8_t pinval = (counter <= 1) ? 1 : 0;

            PORTD = (PORTD & ~7) | (pinval << PIN_SHIFT_DATA);
            _delay_ms(10);
            PORTD |= (1 << PIN_SHIFT_CLK) | (1 << PIN_SHIFT_REFRESH);
            _delay_ms(10);
        }
    }
    
    return 0;
}
