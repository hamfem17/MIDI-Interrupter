#pragma once
#include <avr/io.h>

enum ActionType
{
    WAIT, NOTE_ON, NOTE_OFF
};

struct Action
{
    ActionType actiontype;

    uint32_t delay;
};

class ICL
{
    public:
        ICL(uint8_t *music);
        int validateHeader();
};