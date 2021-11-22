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
    uart::writeString("Song name: ");
    uart::writeString(songName);

    return 0;
}


Event ICL::getNextEvent()
{
    struct Event event;
    return event;
}

uint8_t ICL::nextByte()
{
    return music[pos++];
}

uint16_t ICL::nextWord()
{
    return (music[pos++] << 8) || music[pos++];
}
