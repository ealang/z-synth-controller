#include "adc_poll.h"
#include "button_input.h"
#include "led.h"
#include "shift_ctrl.h"
#include "time.h"
#include "usart.h"

#include <string.h>
#include <stdio.h>
#include <avr/interrupt.h>

ISR(ADC_vect) {
    adc_poll_isr();
}

ISR (TIMER0_OVF_vect) {
    time_timer0_ovf_isr();
    led_timer_isr();
    button_input_timer_isr();
}

ISR(PCINT0_vect)
{
    button_input_pcint0_isr();
}

ISR(PCINT2_vect)
{
    button_input_pcint2_isr();
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

    ButtonEvent button_event;

    while (1) {
        uint16_t cur_time = get_time_ticks();

        if (adc_poll_has_change()) {
            ADCChange change = adc_poll_get_change();
            last_updated_sensor_idx = change.last_change_index;
            change_ready_to_send = 1;
        }

        if (button_input_get_event(&button_event)) {
            char buffer[18];
            memset(buffer, 0, 18);
            snprintf(buffer, 18, "button: %d %d", button_event.button_number, button_event.gesture);
            usart_transmit(buffer, 18);
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

            // demo
            led_set_brightness(
                change.live_values[0]
            );
        }

    }
}

int main() {
    shift_ctrl_init();
    led_init();

    ButtonInputState button_input_state;
    button_input_init(&button_input_state);

    ADCState adc_state;
    adc_poll_init(&adc_state);

    usart_init();
    adc_poll_start();

    time_init();

    sei();

    main_loop();
    return 0;
}
