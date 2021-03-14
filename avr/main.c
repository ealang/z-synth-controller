#include "config.h"
#include "adc_poll.h"
#include "shift_ctrl.h"
#include "time.h"
#include "usart.h"

#include <avr/interrupt.h>
#include <avr/io.h>

ISR(ADC_vect) {
    adc_poll_isr();
}

ISR (TIMER0_OVF_vect) {
    time_timer0_ovf_isr();
}

static void init_leds() {
    DDRC |= 0xE;
}

static void write_led(uint8_t val) {
    PORTC = (PORTC & ~0xE) | ((val & 7) << 1);
}

static void send_values(const uint8_t *values) {
    usart_transmit_byte(0x13);
    usart_transmit(values, ADC_NUM_VALUES);
    usart_transmit_byte(0x37);
}

void main_loop() {

    uint16_t last_send_time = 0;
    uint16_t min_send_interval = MS_TO_TICKS(10);
    uint16_t max_send_interval = MS_TO_TICKS(1000);
    char change_ready_to_send = 0;

    uint8_t last_updated_sensor_idx = -1;

    while (1) {
        uint16_t cur_time = get_time_ticks();

        if (adc_poll_has_change()) {
            ADCChange change = adc_poll_get_change();
            last_updated_sensor_idx = change.last_change_index;
            change_ready_to_send = 1;
        }

        uint16_t time_since_send = cur_time - last_send_time;
        if (
            (change_ready_to_send || time_since_send >= max_send_interval) &&
            (time_since_send >= min_send_interval)
        ) {
            ADCChange change = adc_poll_get_change();
            send_values(change.live_values);
            last_send_time = cur_time;
            change_ready_to_send = 0;
        }
    }
}

int main() {

    shift_ctrl_init();

    ADCState adc_state;
    adc_poll_init(&adc_state);

    usart_init();
    adc_poll_start();

    time_init();

    sei();

    init_leds();
    write_led(1);

    main_loop();
    return 0;
}
