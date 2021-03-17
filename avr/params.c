#include "params.h"

#include <string.h>

void params_init(ParamsState *state) {
    memset(state, 0, sizeof(ParamsState));
}

const uint8_t *params_get_active_values(const ParamsState *params_state) {
    return params_state->active_values;
}

void params_set_live_values(ParamsState *state, const uint8_t *values) {
    const char *is_pristine = state->is_pristine;
    uint8_t *active_values = state->active_values;
    for (uint8_t i = 0; i < ADC_NUM_VALUES; ++i) {
        if (!(*is_pristine++)) {
            *active_values = *values;
        }
        ++values;
        ++active_values;
    }
}

void params_set_pristine_values(ParamsState *state, const uint8_t *values) {
    char *is_pristine = state->is_pristine;
    uint8_t *active_values = state->active_values;
    uint8_t *pristine_values = state->pristine_values ;
    for (uint8_t i = 0; i < ADC_NUM_VALUES; ++i) {
        *is_pristine++ = 1;
        *active_values++ = *values;
        *pristine_values++ = *values++;
    }
    state->pristine_values_valid = 1;
}

void params_make_pristine(ParamsState *state) {
    char *is_pristine = state->is_pristine;
    const uint8_t *active_values = state->active_values;
    uint8_t *pristine_values = state->pristine_values ;
    for (uint8_t i = 0; i < ADC_NUM_VALUES; ++i) {
        *is_pristine++ = 1;
        *pristine_values++ = *active_values++;
    }
    state->pristine_values_valid = 1;
}

void params_make_index_dirty(ParamsState *state, uint8_t index) {
    state->is_pristine[index] = 0;
}

char params_get_pristine_index_value(ParamsState *state, uint8_t index, uint8_t *val) {
    if (state->pristine_values_valid) {
        *val = state->pristine_values[index];
        return 1;
    }
    return 0;
}
