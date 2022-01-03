#pragma once

#include <avr/io.h>
#include "midi.h"

#define DEBUG

namespace uart
{
    void init();
    void writeString (char str[]);
    void debugString (char str[]);
    void printEvent (Event event);
    void debugEvent (Event event);
    void writeInt(uint32_t nummber, uint8_t padding = 4);
    void debugInt (uint32_t number, uint8_t padding = 4);
    void debugHex (uint32_t number, uint8_t padding = 4);
}