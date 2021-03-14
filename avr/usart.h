#ifndef USART_H
#define USART_H

#include <stdint.h>

void usart_init();
void usart_transmit_byte(uint8_t data);
void usart_transmit(const uint8_t *data, uint8_t count);

#endif
