#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include <Arduino.h>
#include <SD.h>
#include <SPI.h>

#include "notes.h"
#include "midi.h"
#include "uart.h"
#include "pwm.h"
#include "deltaTimer.h"

Sd2Card card;
SdVolume volume;
SdFile root;
File myFile;

const int chipSelect = 53;


void error()
{
	DDRB |= (1 << 7);	//Pin 13 als Ausgang (LED)

	while(1)
	{ 
		PORTB |= (1 << 7);
		_delay_ms(250);
		PORTB &= ~(1 << 7);
		_delay_ms(250);
	}
}

void initSD()
{
	uart::debugString("Initializing SD card...\n");

	if (!SD.begin(chipSelect)) {
		uart::debugString("initialization failed!\n");
		while (1);
	}
	uart::debugString("initialization done.\n");
}

int main()
{
	uart::init();
	DeltaTimer::init();
	PWM::init();


	int freq;
	int size = 0;
	uint8_t *midiFILE;

	
	
	initSD();

  	myFile = SD.open("Harry.mid");
	if (myFile)
	{
		uart::debugString("Harry.mid:\n");
		int u = 0;

		size = myFile.size();
		midiFILE = (uint8_t *)malloc(size);

		if(midiFILE == NULL)
		{
			error();
		}

		for(int i = 0; i < size; i++)
		{
			midiFILE[i] = myFile.read();
		}
		
		myFile.close();
	} else {

		uart::debugString("error opening test.txt\n");
	}


	MIDI decoder = MIDI(midiFILE);

	if(decoder.readHeader())
	{
		return 1;
	};

	uart::writeString("  DT | EVENT TYPE  | NOTE\n");

	while(1)
	{
		Event event = decoder.getNextEvent();

		//uart::writeInt(event.deltaTime);
		//uart::writeString("       ");

		uart::debugEvent(event);


		if(event.type == END_OF_TRACK)
		{
			break;
		}

		switch(event.type)
		{
			case NOTE_ON:
				freq = 440 * pow(2, (double)(event.note - 69) / 12);
				DeltaTimer::delay(event.deltaTime);
				PWM::start(freq);
				break;
			case NOTE_OFF:
			 	freq = 440 * pow(2, (double)(event.note - 69) / 12);
				DeltaTimer::delay(event.deltaTime);
				PWM::stop(freq);
				break;
			case SET_TEMPO:
				DeltaTimer::setTempo(event.tempo / decoder.divison);
				break;
			case END_OF_TRACK:
				break;

		}
	}

	for(int i = 0; i < size; i++)
	{
		uart::debugHex(midiFILE[i]);
	}
	


	return 0;
}