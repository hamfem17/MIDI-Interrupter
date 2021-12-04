#pragma once

#include <avr/io.h>
#include "icl.h"

namespace uart {
    void init();
    void writeString (char str[]);
    void printEvent (Event event);
    void writeInt(uint32_t nummber);
}