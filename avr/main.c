#include "adc_poll.h"
#include "button_input.h"
#include "led.h"
#include "preset_store.h"
#include "shift_ctrl.h"
#include "time.h"
#include "usart.h"
#include "params.h"

#include <avr/interrupt.h>

#define PACKET_HEADER_ACTIVE_PARAMS 1
#define PACKET_HEADER_LOGGING       2

ISR(ADC_vect) {
    adc_poll_isr();
}

ISR (TIMER0_OVF_vect) {
    time_timer0_ovf_isr();
    led_on_clock_tick_update();
    button_input_on_clock_tick_update();
}

ISR(PCINT0_vect) {
    button_input_pcint0_isr();
}

ISR(PCINT2_vect) {
    button_input_pcint2_isr();
}

static void send_values(const uint8_t *values) {
    uint8_t checksum = 1 + PACKET_HEADER_ACTIVE_PARAMS;
    for (uint8_t i = 0; i < ADC_NUM_VALUES; ++i) {
        checksum += values[i];
    }
    usart_transmit_byte(PACKET_HEADER_ACTIVE_PARAMS);
    usart_transmit(values, ADC_NUM_VALUES);
    usart_transmit_byte(checksum);
}

static uint8_t difference(uint8_t a, uint8_t b) {
    return a > b ? a - b : b - a;
}

static void play_led_error_sequence() {
    uint8_t brightness = 0x40;
    led_set_sequence(brightness, 4, MS_TO_TICKS(50));
}

static void play_led_ack_sequence() {
    uint8_t brightness = 0x40;
    led_set_sequence(brightness, 2, MS_TO_TICKS(150));
}

void main_loop() {

    uint16_t last_send_time = 0;
    uint16_t min_send_interval = MS_TO_TICKS(10);
    uint16_t max_send_interval = MS_TO_TICKS(1000);
    char change_ready_to_send = 0;

    ButtonEvent button_event;

    ADCChange change;

    ParamsState params_state;
    params_init(&params_state);

    led_set_brightness(8);

    while (1) {
        uint16_t cur_time = get_time_ticks();

        if (adc_poll_get_update(&change)) {
            uint8_t index = change.last_change_index;

            uint8_t pristine_value;
            if (params_get_pristine_index_value(&params_state, index, &pristine_value)) {
                uint8_t cur_value = change.live_values[index];
                uint16_t delta = difference(cur_value, pristine_value) * 2;
                if (delta > 0x78) {
                    delta = 0x78;
                }
                led_set_brightness(0x80 - delta);
            }

            params_make_index_dirty(&params_state, index);
            params_set_live_values(&params_state, change.live_values);
            change_ready_to_send = 1;
        }

        if (button_input_get_event(&button_event)) {
            uint8_t preset = button_event.button_number;
            char changed_state = 0;
            if (button_event.gesture == ButtonShortPress) {
                uint8_t buffer[ADC_NUM_VALUES];
                if (load_presets(preset, buffer)) {
                    params_set_pristine_values(&params_state, buffer);
                    changed_state = 1;
                } else {
                    play_led_error_sequence();
                }
            } else {
                params_make_pristine(&params_state);
                save_presets(preset, params_get_active_values(&params_state));
                changed_state = 1;
                play_led_ack_sequence();
            }
            if (changed_state) {
                led_set_brightness(8);
                change_ready_to_send = 1;
            }
        }

        uint16_t time_since_send = cur_time - last_send_time;
        if (
            (change_ready_to_send || time_since_send >= max_send_interval) &&
            (time_since_send >= min_send_interval)
        ) {
            const uint8_t *values = params_get_active_values(&params_state);
            send_values(values);
            last_send_time = cur_time;
            change_ready_to_send = 0;
        }

    }
}

int main() {
    shift_ctrl_init();

    LedState led_state;
    led_init(&led_state);

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
