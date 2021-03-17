#ifndef PRESET_STORE_H
#define PRESET_STORE_H

#include <stdint.h>

void save_presets(uint8_t bank_num, const uint8_t *src);

// Try to load presets. Return 1 if presets loaded into dest,
// else return 0 and dest will be unchanged.
char load_presets(uint8_t bank_num, uint8_t *dest);

#endif
