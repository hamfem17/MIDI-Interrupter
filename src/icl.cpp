#include "icl.h"
#include <Arduino.h>

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

    uint32_t byteNameLength = 0;
    int n = pos;

    while(music[n] != 0x00)
    {
        n++;
    }

    byteNameLength = n - pos;
    char* musicName = (char*) malloc( sizeof(char) * byteNameLength+1);

    for(uint32_t i = 0; i < pos + byteNameLength + 1; i++)
    {
        musicName[i] = music[pos];
    }

    char i [] = "Hallo";

    Serial.println(i);

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
