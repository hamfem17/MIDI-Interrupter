#include "icl.h"
#include "uart.h"

ICL::ICL(uint8_t* music)
{
    this->music = music;
    this->pos = 0;
}

uint8_t ICL::validateHeader()
{
    if(nextByte() != 'I' || nextByte() != 'C' || nextByte() != 'L') // ASCII "ICL"
    {
        return 1;
    }

    if(nextWord() != 1) // Version
    {
        return 2;
    }

    if(nextByte() != 1) //max. number of notes	
    {
        return 3;
    }

    int nameEnd = pos;

    while(music[nameEnd] != 0x00)
    {
        nameEnd++;
    }

    uint32_t nameLength = nameEnd - pos + 1;
    char* songName = (char*) malloc(sizeof(char) * nameLength);

    for(uint32_t i = 0; i < nameLength; i++)
    {
        songName[i] = nextByte();
    }

    // Debug
    uart::writeString("\nSong name: ");
    uart::writeString(songName);

    return 0;
}


Event ICL::getNextEvent()
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

    switch(byte & 0xF0)
    {
        case 0x00:

            event.type = NOTE_ON;
            break;

        case 0x10:
            
            event.type = NOTE_OFF;
            break;

        case 0x20:

            event.type = SET_TEMPO;
            break;

        case 0xF0:

            event.type = END_OF_FILE;
            break;
    }

    if(event.type == NOTE_ON)
    {
        event.velocity = byte & 0x0F;
    }

    if(event.type == NOTE_ON || event.type == NOTE_OFF)
    {
        event.note =  nextByte();
    }

    if(event.type == SET_TEMPO)
    {
        event.tempo = (int32_t)nextWord() << 8 | nextByte();

    }


    return event;
}

uint8_t ICL::nextByte()
{
    return music[pos++];
}

uint16_t ICL::nextWord()
{
    return ((int16_t)music[pos++] << 8) | music[pos++];
}
