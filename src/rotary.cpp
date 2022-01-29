#include "include/rotary.h"

#include <avr/io.h>
#include <util/delay.h>

namespace rotary {
    volatile int8_t direction; // 1 for CW, -1 for CCW
    volatile uint8_t rotary_flag;
    volatile uint8_t button_flag;
}

ISR(INT5_vect) {
    rotary::rotary_flag = 1;
    rotary::direction = (PING & (1 << PG5)) ? 1 : -1;
    _delay_ms(100);
    EIFR |= (1 << INTF5); // clear pending interrupts
}

ISR(INT4_vect) {
    rotary::button_flag = 1;
    _delay_ms(50);
    EIFR |= (1 << INTF4);
}

void rotary::init() {
    // PG5 -> Pin 4 (Input)
    // PE5 -> Pin 3 (Interrupt)
    DDRE &= ~(1 << PE5);
    DDRG &= ~(1 << PG5);
    PORTE |= (1 << PE5); // Enabling internal pullup
    PORTG |= (1 << PG5);
    EIMSK |= (1 << INT5); // PE5
    EICRB |= (1 << ISC51); // Interrupt on falling edge
    rotary_flag = 0;
    // button
    DDRE &= ~(1 << PE4);
    PORTE |= (1 << PE4);
    EIMSK |= (1 << INT4);
    EICRB |= (1 << ISC40) | (1 << ISC41);
    button_flag = 0;
}

void rotary::watch() {
    uart::writeString("watching rotary:\n");
    while(1) {
        if(rotary_flag) {
            uart::writeInt(direction);
            uart::writeString("\n");
            rotary_flag = 0;
        }
        if(button_flag) {
            uart::writeString("button\n");
            button_flag = 0;
        }
    }
}

int8_t rotary::getChange() {
    if(rotary_flag) {
        rotary_flag = 0;
        return direction;
    } else {
        return 0;
    }
}

uint8_t rotary::buttonPressed() {
    if(button_flag) {
        button_flag = 0;
        return 1;
    }
    return 0;
}