#include "adc_poll.h"
#include "shift_ctrl.h"

#include <avr/io.h>
#include <string.h>

static ADCState* singleton = 0;

void adc_poll_init(ADCState *instance) {
    memset(instance, 0, sizeof(ADCState));
    singleton = instance;

    ADMUX = 0;  // AVREF, ADC0, left adjusted
    ADCSRA |= (1 << ADEN);  // enable ADC
    ADCSRA |= (1 << ADIE);  // enable interrupt
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

    uint16_t new_val = valh << 8 | vall;
    uint16_t old_val = singleton->values[singleton->cur_value];
    if (old_val != new_val) {
        ++singleton->change_count;
    }

    singleton->values[singleton->cur_value] = new_val;
    singleton->cur_value = (singleton->cur_value + 1) % ADC_NUM_VALUES;

    if (singleton->cur_value == 0) {
        shift_ctrl_shift(1);
    } else {
        shift_ctrl_shift(0);
    }

    shift_ctrl_display();

    ADCSRA |= (1 << ADSC);  // start conversion
}
