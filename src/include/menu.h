#pragma once

#include <avr/io.h>

namespace menu {
    uint8_t selectItem(char** items);
    void print(char* str);
}