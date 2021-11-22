#include "uart.h"

void uart::init() {
    DDRB |= (1 << PB5);
    UBRR0H = 0;
    UBRR0L = 34; //Baud = 57600

    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 0x06;
    UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0); // 0x98;
    UCSR0A = (1 << U2X0); // 0x02;
}

void uart::writeString (char str[]) {
    for (int i = 0; str[i] != 0; i++) {
        while ( !( UCSR0A & (1 << UDRE0)) ) {
            // wait, until UDR0 is available again
        }
        UDR0 = str[i];
    }
}
