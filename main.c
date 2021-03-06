#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>


ISR (ADC_vect)
{
    uint8_t val = ADCH;
    PORTB = (PORTB & ~7) | (val >> 5);
}

int main() {
    sei();

    DDRB = 0xFF;
    DDRD = 0xFF;
    PORTD = 0x02;

    ADMUX = (1 << ADLAR);   // AVREF, ADC0, left adjusted

    ADCSRA |= (1 << ADEN);  // enable ADC
    ADCSRA |= (1 << ADIE);  // enable interrupt
    ADCSRA |= (1 << ADATE);  // enable auto-trigger
    ADCSRA |= (1 << ADSC);  // start conversion

    while (1);
    
    return 0;
}
