#pragma once

#include <avr/io.h>

namespace uart {
    void init();
    void writeString (char str[]);
}