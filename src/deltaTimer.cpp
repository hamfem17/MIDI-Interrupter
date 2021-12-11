#include <avr/interrupt.h>

#include "uart.h"
#include "deltaTimer.h"

namespace DeltaTimer
{
    uint32_t volatile overflow_count;
}

void DeltaTimer::init()
{
    TCCR2A = 0;
    TCCR2B |= (1 << CS22) | (1 << CS21);

    TIMSK2 |= (1 << TOIE2);
    sei();
}

void DeltaTimer::delay(uint32_t deltatime)
{
    TCNT2 = 0;
    
    uint64_t delayTime = deltatime * tempo; // Delta Time zur Microsekunden rechnen
    DeltaTimer::overflow_total = delayTime / 4096;

    overflow_count = 0;

    uart::writeInt(overflow_total);
    uart::writeString("\n");
    
    while(overflow_count != overflow_total);
}

void DeltaTimer::setTempo(uint32_t music_tempo)
{
    tempo = music_tempo;
}

ISR (TIMER2_OVF_vect)
{
	DeltaTimer::overflow_count++;
}