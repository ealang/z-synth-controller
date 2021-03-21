#ifndef ADC_POLL_H
#define ADC_POLL_H

#include "rolling_avg.h"

#include <stdint.h>

typedef struct {
  uint8_t lower_bound;
  uint8_t upper_bound;
  uint8_t last_value;
  uint16_t time;
  RollingAvg samples;
} SampleFilter;

typedef struct {
  uint8_t values[ADC_NUM_VALUES];
  uint8_t cur_value;
  char change_flag;
  uint8_t last_change_index;

  SampleFilter filters[ADC_NUM_VALUES];
} ADCState;

typedef struct {
  uint8_t last_change_index;
  const uint8_t *live_values;
} ADCChange;

// Initialize hw & set global instance
void adc_poll_init(ADCState *instance);
// Begin polling
void adc_poll_start();
// ADC_vect interrupt handler
void adc_poll_isr();

// Return 1 if an unconsumed update is available and write value into `update`.
char adc_poll_get_update(ADCChange *update);
const uint8_t *adc_poll_get_live_values();

#endif
