#include "uart.h"

void uart::init()
{
    DDRB |= (1 << PB5);
    UBRR0H = 0;
    UBRR0L = 34; //Baud = 57600

    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 0x06;
    UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0); // 0x98;
    UCSR0A = (1 << U2X0); // 0x02;
}

void uart::writeString (char str[])
{
    for (int i = 0; str[i] != 0; i++)
    {
        while ( !( UCSR0A & (1 << UDRE0)) )
        {
            // wait, until UDR0 is available again
        }
        UDR0 = str[i];
    }
}

void uart::printEvent (Event event)
{    
    uart::writeInt(event.deltaTime);
    uart::writeString(" | ");

    switch(event.type)
    {
        case NOTE_ON:
            uart::writeString("NOTE ON    ");
            break;

        case NOTE_OFF:
            uart::writeString("NOTE OFF   ");
            break;

        case SET_TEMPO:
            uart::writeString("SET TEMPO  ");
            break;

        case END_OF_FILE:
            uart::writeString("END_OF_FILE");
    }

    uart::writeString(" | ");

    if(event.type == NOTE_ON || event.type == NOTE_OFF)
    {
        uart::writeInt(event.velocity);
        uart::writeString(" | ");

        uart::writeInt(event.note);
    }

    uart::writeString("\n");
}


void uart::writeInt (uint32_t number, uint8_t padding = 4)
{
    char s[100];

    ltoa(number,s,10);    //itoa is integer with 16 Bit but need 32 bit
    uint8_t len;
    for(int i = 0; i < 100; i++)
    {
        if(s[i] == 0)
        {
            len = i;
            break;
        }
    }

    for(int i = 0; i < padding - len; i++) {
        uart::writeString(" ");
    }

    uart::writeString(s);
}

   