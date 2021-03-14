#include "adc_poll.h"
#include "shift_ctrl.h"

#include <avr/io.h>
#include <string.h>

static ADCState* singleton = 0;

#define MIN_EXPECTED_SAMPLE 512
#define MAX_EXPECTED_SAMPLE 1024
#define SAMPLE_BIAS 16

static uint8_t truncate_sample(uint16_t sample) {
    sample += SAMPLE_BIAS;
    if (sample >= MAX_EXPECTED_SAMPLE) {
        return 0xFF;
    }
    if (sample <= MIN_EXPECTED_SAMPLE) {
        return 0;
    }
    return (sample >> 1) & 0xFF;
}

void adc_poll_init(ADCState *instance) {
    memset(instance, 0, sizeof(ADCState));
    singleton = instance;

    ADMUX = 0;  // AVREF, ADC0, left adjusted
    ADCSRA = (1 << ADEN) | // enable ADC
             (1 << ADIE) | // enable interrupt
             (1 << ADPS2) | (1 << ADPS1); // 64 prescale (8mhz/64 = 125khz)
}

void adc_poll_start() {
    shift_ctrl_clear();
    shift_ctrl_shift(1);
    shift_ctrl_display();
    ADCSRA |= (1 << ADSC);  // start conversion
}

void adc_poll_isr() {
    if (!singleton) {
        return;
    }

    // low must be read first
    uint8_t vall = ADCL;
    uint8_t valh = ADCH;

    uint8_t existing_value = singleton->values[singleton->cur_value];
    uint8_t new_value = truncate_sample(valh << 8 | vall);

    singleton->values[singleton->cur_value] = new_value;
    singleton->change_flag = existing_value != new_value;

    singleton->cur_value = (singleton->cur_value + 1) % ADC_NUM_VALUES;

    if (singleton->cur_value == 0) {
        shift_ctrl_shift(1);
    } else {
        shift_ctrl_shift(0);
    }

    shift_ctrl_display();

    ADCSRA |= (1 << ADSC);  // start conversion
}

char adc_poll_has_change() {
    return singleton->change_flag;
}

ADCChange adc_poll_get_change() {
    singleton->change_flag = 0;
    ADCChange change = {
        singleton->last_change_index,
        singleton->values
    };
    return change;
}
