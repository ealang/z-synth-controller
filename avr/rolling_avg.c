#include "rolling_avg.h"

uint8_t compute_rolling_avg_next(RollingAvg *avg, uint8_t value) {
    avg->values[avg->i++] = value;
    avg->i %= ROLLING_AVG_NUM_VALUES;

    uint8_t *p = avg->values;
    uint16_t sum = 0;
    for (uint8_t i = 0; i < ROLLING_AVG_NUM_VALUES; ++i) {
        sum += *p++;
    }

    return sum / ROLLING_AVG_NUM_VALUES;
}
