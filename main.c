#include <avr/interrupt.h>

#include "shift_ctrl.h"
#include "adc_poll.h"
#include "spi_ifc.h"

ISR(ADC_vect)
{
    adc_poll_isr();
}

ISR(SPI_STC_vect)
{
    spi_ifc_isr();
}

/*
void init_leds() {
    DDRC |= 0xE;
}
void write_led(uint8_t val) {
    PORTC = (PORTC & ~0xE) | (val & 7) << 1;
}
*/

int main() {

    shift_ctrl_init();

    ADCState adc_state;
    adc_poll_init(&adc_state);
    spi_ifc_init(adc_state.values);

    sei();
    adc_poll_start();

    while (1);
    return 0;
}
