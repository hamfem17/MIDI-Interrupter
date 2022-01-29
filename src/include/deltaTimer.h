#pragma once

#include <avr/io.h>

namespace DeltaTimer
{
    namespace
    { 
	    uint32_t overflow_total;
	    uint32_t tempo = 2000;						//μs per MIDI-Tick
    }

	extern uint32_t volatile overflow_count;

	void init();

	void delay(uint32_t deltatime);

	void setTempo(uint32_t music_tempo);
}