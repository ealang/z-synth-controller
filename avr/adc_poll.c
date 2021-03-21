#include "adc_poll.h"
#include "shift_ctrl.h"
#include "time.h"

#include <avr/io.h>
#include <string.h>

static ADCState* singleton = 0;

#define MIN_EXPECTED_SAMPLE 512
#define MAX_EXPECTED_SAMPLE 1024
#define SAMPLE_BIAS 16

#define FILTER_BOUNDARY  6
#define FILTER_TIME MS_TO_TICKS(200)

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

static char filter_sample(SampleFilter *filter, uint8_t new_value, uint8_t *out) {
    uint16_t cur_time = get_time_ticks();

    char changed = 0;
    if (new_value < filter->lower_bound) {
        // going down
        filter->lower_bound = new_value;
        changed = 1;

        uint8_t delta = filter->upper_bound - filter->lower_bound;
        if (delta > FILTER_BOUNDARY) {
            filter->upper_bound = filter->lower_bound + FILTER_BOUNDARY;
        }
    } else if (new_value > filter->upper_bound) {
        // going up
        filter->upper_bound = new_value;
        changed = 1;

        uint8_t delta = filter->upper_bound - filter->lower_bound;
        if (delta > FILTER_BOUNDARY) {
            filter->lower_bound = filter->upper_bound - FILTER_BOUNDARY;
        }
    }

    if (changed) {
        filter->time = cur_time;
        filter->last_value = new_value;
        *out = new_value;
        return 1;
    }

    if (cur_time - filter->time >= FILTER_TIME) {
        filter->time = cur_time;

        uint8_t half = FILTER_BOUNDARY >> 1;
        if (filter->last_value >= half) {
            filter->lower_bound = filter->last_value - half;
        }
        if (filter->last_value <= 0xFF - half) {
            filter->upper_bound = filter->last_value + half;
        }
    }

    return 0;
}


void adc_poll_isr() {
    if (!singleton) {
        return;
    }

    // low must be read first
    uint8_t vall = ADCL;
    uint8_t valh = ADCH;

    uint8_t new_value = truncate_sample(valh << 8 | vall);
    uint8_t i = singleton->cur_value;

    uint8_t out;
    if (filter_sample(&singleton->filters[i], new_value, &out)) {
        singleton->values[i] = out;
        singleton->change_flag = 1;
        singleton->last_change_index = i;
    }

    singleton->cur_value = (i + 1) % ADC_NUM_VALUES;

    if (singleton->cur_value == 0) {
        shift_ctrl_shift(1);
    } else {
        shift_ctrl_shift(0);
    }

    shift_ctrl_display();

    ADCSRA |= (1 << ADSC);  // start conversion
}

char adc_poll_get_update(ADCChange *change) {
    if (singleton->change_flag) {
        singleton->change_flag = 0;
        change->last_change_index = singleton->last_change_index;
        change->live_values = singleton->values;
        return 1;
    }
    return 0;
}

const uint8_t *adc_poll_get_live_values() {
    return singleton->values;
}
