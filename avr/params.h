#ifndef PARAMS_H
#define PARAMS_H

#include <stdint.h>

typedef struct {
    char is_pristine[ADC_NUM_VALUES];  // if 1, live value is ignored
    uint8_t active_values[ADC_NUM_VALUES];  // values sent to host
    uint8_t pristine_values[ADC_NUM_VALUES];  // values last loaded/saved
    char synced;
    char pristine_values_valid;
} ParamsState;

void params_init(ParamsState *state);

// Get the "active" parameter values (which should be sent to the host)
const uint8_t *params_get_active_values(const ParamsState *params_state);

// Save any updates to parameter values
void params_set_live_values(ParamsState *state, const uint8_t *values);

// Make state pristine by providing pristine values
void params_set_pristine_values(ParamsState *state, const uint8_t *values);

// Copy active state to pristine state
void params_make_pristine(ParamsState *state);

void params_make_index_dirty(ParamsState *state, uint8_t index);
char params_get_pristine_index_value(ParamsState *state, uint8_t index, uint8_t *val);

#endif
