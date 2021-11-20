#include "icl.h"

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
}


Event ICL::getNextEvent()
{
    struct Event event;
}

uint8_t ICL::nextByte()
{
    return music[pos++];
}

uint16_t ICL::nextWord()
{
    return (music[pos++] << 8) || music[pos++];
}
