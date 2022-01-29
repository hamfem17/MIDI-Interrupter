#pragma once

#include <avr/interrupt.h>

#include "uart.h"

namespace rotary {
    void init();
    void watch();
    int8_t getChange(); // returns 1 or -1 if turned and 0 if not
    uint8_t buttonPressed(); // returns 1 if button has been pressed
}