#include "usart.h"

#include <avr/io.h>

void usart_init() {
    UBRR0L = 9;  // 8mhz / (16 * 50000) - 1
    UCSR0B = (1 << TXEN0);  // enable transmitter
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00) |  // 8 bit
             (1 << USBS0);                    // 2 stop bits
}

void usart_transmit_byte(uint8_t data) {
    // wait for empty transmit buffer
    while (!(UCSR0A & (1<<UDRE0)));
    UDR0 = data;
}

void usart_transmit(const uint8_t *data, uint8_t count) {
    for (uint8_t i = 0; i < count; ++i) {
        usart_transmit_byte(*(data++));
    }
}
