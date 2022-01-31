#pragma once

#include <avr/io.h>

namespace menu {
    uint8_t selectItem(char** items, uint8_t lastPlayed);
    void print(char* str);
}