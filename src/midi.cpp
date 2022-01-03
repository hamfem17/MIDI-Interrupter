#include "midi.h"
#include "uart.h"

MIDI::MIDI(uint8_t* music)
{
    this->music = music;
    this->pos = 0;
    this->divison = 0;
}

uint8_t MIDI::readHeader()
{
    if(nextByte() != 'M' || nextByte() != 'T' || nextByte() != 'h' || nextByte() != 'd') // ASCII "MThd"
    {
        return 1;
    }

    if(nextDWord() != 6) // Length Header
    {
        return 2;
    }

    if(nextWord() != 0) // File format
    {
        return 3;
    }

    if(nextWord() != 1) // Number of tracks
    {
        return 4;
    }

    divison = nextWord(); // Divisons

    return 0;
}

Event MIDI::getNextEvent()
{
    struct Event event;
    event.deltaTime = 0;
    for(int i = 0; i < 4; i++)
    {
        uint8_t byte = nextByte();
        event.deltaTime = (event.deltaTime << 7) | (byte & 0x7F);
        if(~byte & 0x80)
        {
             break;
        }
    }


    uint8_t byte = nextByte();

    if(byte & 0xF0 == 0x80)
    {
        event.type = NOTE_OFF;
    } 
    else if(byte & 0xF0 == 0x90)
    {
        event.type = NOTE_ON;
    }
    else if (byte == 0xFF)
    {
        byte = nextByte();
        
        if(byte == 0x2F)
        {
            event.type = END_OF_TRACK;
            nextByte();
        }
        else if(byte == 0x51)
        {
            nextByte();
            event.tempo = ((uint32_t)nextWord() << 8) | (nextByte());
            event.type = SET_TEMPO;
        }
    }


    if(event.type == NOTE_ON || event.type == NOTE_OFF)
    {
        event.note =  nextByte();
        nextByte();
    }

    return event;
}

uint8_t MIDI::nextByte()
{
    return music[pos++];
}

uint16_t MIDI::nextWord()
{
    return ((int16_t)music[pos++] << 8) | music[pos++];
}

uint16_t MIDI::nextDWord()
{
    return ((int32_t)music[pos++] << 24) | music[pos++];
}

