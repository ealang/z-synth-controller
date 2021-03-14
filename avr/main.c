#include "shift_ctrl.h"
#include "adc_poll.h"
#include "usart.h"

#include <avr/interrupt.h>
#include <avr/io.h>

volatile uint16_t system_time = 0;

ISR(ADC_vect) {
    adc_poll_isr();
}

ISR (TIMER0_OVF_vect) {
    ++system_time;
}

void init_leds() {
    DDRC |= 0xE;
}

void write_led(uint8_t val) {
    PORTC = (PORTC & ~0xE) | ((val & 7) << 1);
}

int main() {
    init_leds();

    shift_ctrl_init();

    ADCState adc_state;
    adc_poll_init(&adc_state);
    usart_init();

    // Use Timer0 overflow interrupt for system ticks
    TCCR0B |= (1 << CS01);  // Prescale of 8
    TIMSK0 |= (1 << TOIE0);  // Interrupt on overflow (8bit)

    sei();
    adc_poll_start();

    while (1) {
        if (system_time >= 30) {
            system_time = 0;

            usart_transmit_byte(0x13);
            usart_transmit(adc_state.values, 16);
            usart_transmit_byte(0x37);
        }
    }
    return 0;
}
