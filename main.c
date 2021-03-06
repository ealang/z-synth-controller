#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#include "shift_ctrl.h"
#include "adc_poll.h"

ISR (ADC_vect)
{
    adc_poll_isr();
}

int main() {

    shift_ctrl_init();

    ADCState adc_state;
    adc_poll_init(&adc_state);

    DDRB = 0xFF;  // LEDS

    sei();
    adc_poll_start();

    while (1) {
        PORTB = adc_state.values[0] >> 7;
    }
    return 0;
}
