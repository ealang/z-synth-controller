#ifndef SPI_IFC_H
#define SPI_IFC_H

#include <stdint.h>

void spi_ifc_init(uint8_t *adc_data);
void spi_ifc_isr();

#endif
