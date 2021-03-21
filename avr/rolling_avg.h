#ifndef ROLLING_AVG_H
#define ROLLING_AVG_H

#include <stdint.h>

#define ROLLING_AVG_NUM_VALUES 16

typedef struct {
    uint8_t i;
    uint8_t values[ROLLING_AVG_NUM_VALUES];
} RollingAvg;

uint8_t compute_rolling_avg_next(RollingAvg *avg, uint8_t value);

#endif
