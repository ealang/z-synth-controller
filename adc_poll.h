#ifndef ADC_POLL_H
#define ADC_POLL_H

#include <stdint.h>

#define ADC_NUM_VALUES 16

typedef struct
{
  uint16_t values[ADC_NUM_VALUES];
  uint8_t cur_value;
  uint8_t change_count;
} ADCState;

// Initialize hw & set global instance
void adc_poll_init(ADCState *instance);
// Begin polling
void adc_poll_start();
// ADC_vect interrupt handler
void adc_poll_isr();

#endif
