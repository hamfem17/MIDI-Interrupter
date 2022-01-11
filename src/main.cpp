#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include <Arduino.h>
#include <SPI.h>

#include "notes.h"
#include "midi.h"
#include "uart.h"
#include "pwm.h"
#include "deltaTimer.h"
#include "LCD/lcd.h"
#include "SD.h"


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

class superFatFile : public SdFat_h
{
    public:
    void ls(print_t* pr, uint8_t flags /* = 0 */, uint8_t indent /* = 0 */)
	{
        dir_t* p;

		rewind();
		while ((p = readDirCache())) 
		{
			// done if past last used entry
			if (p->name[0] == DIR_NAME_FREE) break;

			// skip deleted entry and entries for . and  ..
			if (p->name[0] == DIR_NAME_DELETED || p->name[0] == '.') continue;

			// only list subdirectories and files
			if (!DIR_IS_FILE_OR_SUBDIR(p)) continue;

			// print any indent spaces
			for (int8_t i = 0; i < indent; i++) Serial.print(' ');

			// print file name with possible blank fill
			printDirName(*p, flags & (LS_DATE | LS_SIZE) ? 14 : 0);

			// print modify date/time if requested
			if (flags & LS_DATE) {
			printFatDate(p->lastWriteDate);
			Serial.print(' ');
			printFatTime(p->lastWriteTime);
			}
			// print size if requested
			if (!DIR_IS_SUBDIR(p) && (flags & LS_SIZE)) {
			Serial.print(' ');
			Serial.print(p->fileSize);
			}
			Serial.println();

			// list subdirectory content if requested
			if ((flags & LS_R) && DIR_IS_SUBDIR(p)) {
			uint16_t index = curPosition()/32 - 1;
			SdFile s;
			if (s.open(this, index, O_READ)) s.ls(flags, indent + 2);
			seekSet(32 * (index + 1));
			}
		}
	}
	file.close();
	// end of new LS method
};  // end of superClass

int main()
{
	uart::init();
	DeltaTimer::init();
	PWM::init();


	int freq;
	int size = 0;
	uint8_t *midiFILE;
	
	initSD();

  	myFile = SD.open("out.mid");
	if (myFile)
	{
		uart::debugString("out.mid:\n");
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
		
		
	} else {

		uart::debugString("error opening test.txt\n");
	}

	root.openRoot(volume);
	root.ls(LS_R | LS_DATE | LS_SIZE);
	

/*
	dir_t* p;
	rewind();
  	while ((p = readDirCache())) {
    // done if past last used entry
    if (p->name[0] == DIR_NAME_FREE) break;

    // skip deleted entry and entries for . and  ..
    if (p->name[0] == DIR_NAME_DELETED || p->name[0] == '.') continue;

    // only list subdirectories and files
    if (!DIR_IS_FILE_OR_SUBDIR(p)) continue;

    // print any indent spaces
    for (int8_t i = 0; i < indent; i++) Serial.print(' ');

    // print file name with possible blank fill
    printDirName(*p, flags & (LS_DATE | LS_SIZE) ? 14 : 0);

    // print modify date/time if requested
    if (flags & LS_DATE) {
       printFatDate(p->lastWriteDate);
       Serial.print(' ');
       printFatTime(p->lastWriteTime);
    }
    // print size if requested
    if (!DIR_IS_SUBDIR(p) && (flags & LS_SIZE)) {
      Serial.print(' ');
      Serial.print(p->fileSize);
    }
    Serial.println();

    // list subdirectory content if requested
    if ((flags & LS_R) && DIR_IS_SUBDIR(p)) {
      uint16_t index = curPosition()/32 - 1;
      SdFile s;
      if (s.open(this, index, O_READ)) s.ls(flags, indent + 2);
      seekSet(32 * (index + 1));
    }
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
	*/
	
	myFile.close();

	lcd_init(LCD_DISP_ON_CURSOR); /*initialize lcd,display on, cursor on */
	lcd_clrscr();             /* clear screen of lcd */
	lcd_home();               /* bring cursor to 0,0 */
	lcd_puts("hello");        /* type something random */
	lcd_gotoxy(0,1);          /* go to 2nd row 1st col */
	lcd_puts("Mechatronicslab");  /* type something random */


	return 0;
}