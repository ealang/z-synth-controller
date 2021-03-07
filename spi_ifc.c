#include "spi_ifc.h"
#include "adc_poll.h"

#include <avr/io.h>

#define SPI_MAGIC_NUM 0xEE

static uint8_t *singleton_adc_values = 0;

void spi_ifc_init(uint8_t *adc_data) {
    singleton_adc_values = adc_data;

    // init SPI
    #define PIN_MISO 4
    DDRB |= 1 << PIN_MISO; // data pin as output
    SPCR |= 1 << SPE;  // enable spi
    SPCR |= 1 << SPIE;  // enable interrupt
    SPDR = SPI_MAGIC_NUM;
}

void spi_ifc_isr() {
    uint8_t val = SPDR;
    uint8_t out = SPI_MAGIC_NUM;
    if (val < ADC_NUM_VALUES) {
        out = singleton_adc_values[val];
    }
    SPDR = out;
}
