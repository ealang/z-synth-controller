#include "preset_store.h"

#include <avr/eeprom.h>

static uint8_t *get_preset_offset(uint8_t bank_num) {
    return (uint8_t*)(ADC_NUM_VALUES * bank_num + bank_num);
}

void save_presets(uint8_t bank_num, const uint8_t *src) {
    uint8_t *dest = get_preset_offset(bank_num);

    uint8_t sum = 1;
    for (uint8_t i = 0; i < ADC_NUM_VALUES; ++i) {
        uint8_t src_val = *(src++);
        sum += src_val;
        eeprom_update_byte(dest++, src_val);
    }
    eeprom_update_byte(dest, sum);
}

static char csum_is_valid(uint8_t bank_num) {
    uint8_t sum = 1;
    const uint8_t *src = get_preset_offset(bank_num);
    for (uint8_t i = 0; i < ADC_NUM_VALUES; ++i) {
        sum += eeprom_read_byte(src++);
    }
    return eeprom_read_byte(src) == sum;
}

char load_presets(uint8_t bank_num, uint8_t *dest) {
    if (!csum_is_valid(bank_num)) {
        return 0;
    }

    const uint8_t *src = get_preset_offset(bank_num);
    for (uint8_t i = 0; i < ADC_NUM_VALUES; ++i) {
        *(dest++) = eeprom_read_byte(src++);
    }
    return 1;
}
