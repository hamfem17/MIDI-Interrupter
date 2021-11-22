#pragma once

#include <avr/io.h>
#include <stdlib.h>

enum EventType
{
    NOTE_ON, 
    NOTE_OFF, 
    END
};

struct Event
{
    uint32_t deltaTime;
    EventType actiontype;
    uint8_t note;
};

class ICL
{
    uint8_t* music;
    uint32_t pos; 

    uint8_t nextByte();
    uint16_t nextWord();

    public:
    ICL(uint8_t* music);
    uint8_t validateHeader();
    Event getNextEvent();
};