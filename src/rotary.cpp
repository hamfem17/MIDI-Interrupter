#include "rotary.h"

#include <avr/io.h>
#include <util/delay.h>

namespace rotary {
    volatile int8_t direction; // 1 for CW, -1 for CCW
    volatile uint8_t rotary_flag;
}

ISR(INT5_vect) {
    rotary::rotary_flag = 1;
    rotary::direction = (PINE & (1 << PE4)) ? 1 : -1;
    _delay_ms(100);
    EIFR |= (1 << INTF5); // clear pending interrupts
}

void rotary::init() {
    // PE4 -> Pin 2 (Input)
    // PE5 -> Pin 3 (Interrupt)
    DDRE &= ~(1 << PE5);
    DDRE &= ~(1 << PE4);
    PORTE |= (1 << PE4) | (1 << PE5); // Enabling internal pullup
    EIMSK |= (1 << INT5); // PE5
    EICRB |= (1 << ISC51); // Interrupt on falling edge
    rotary_flag = 0;
}

void rotary::watch() {
    uart::writeString("watching rotary:\n");
    while(1) {
        if(rotary_flag) {
            uart::writeInt(direction);
            uart::writeString("\n");
            rotary_flag = 0;
        }
    }
}

int8_t rotary::getChange() {
    if(rotary_flag) {
        return direction;
    } else {
        return 0;
    }
}
