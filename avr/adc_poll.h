#ifndef ADC_POLL_H
#define ADC_POLL_H

#include <stdint.h>

typedef struct
{
  uint8_t values[ADC_NUM_VALUES];
  uint8_t cur_value;
  char change_flag;
  uint8_t last_change_index;
} ADCState;

typedef struct
{
  uint8_t last_change_index;
  const uint8_t *live_values;
} ADCChange;

// Initialize hw & set global instance
void adc_poll_init(ADCState *instance);
// Begin polling
void adc_poll_start();
// ADC_vect interrupt handler
void adc_poll_isr();

// Return 1 if an unconsumed change is available
char adc_poll_has_change();
// Access current data
ADCChange adc_poll_get_change();

#endif
