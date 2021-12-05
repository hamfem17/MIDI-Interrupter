#pragma once

#include <avr/io.h>
#include <stdlib.h>

enum EventType
{
    NOTE_ON, 
    NOTE_OFF,
    SET_TEMPO,
    END_OF_FILE
};

struct Event
{
    uint32_t deltaTime;
    EventType type;
    uint8_t note;
    uint8_t velocity;
    uint32_t tempo;
};

class ICL
{
    uint8_t* music;
    uint32_t pos = 0; 

    uint8_t nextByte();
    uint16_t nextWord();

    public:
    ICL(uint8_t* music);
    uint8_t validateHeader();
    Event getNextEvent();
};