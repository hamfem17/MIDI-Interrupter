#include <avr/io.h>
#include <util/delay.h>
#include <math.h>

#include "notes.h"
#include "midi.h"
#include "uart.h"
#include "pwm.h"
#include "deltaTimer.h"
#include "LCD/lcd.h"

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

uint8_t SPI_transmit(uint8_t data) {
	SPDR = data;
	// Wait until the transmission is complete
	while(!(SPSR & (1<<SPIF))){}
	//_delay_us(10);
	return SPDR;
}

namespace SD {
	uint8_t sendCommand(uint8_t index, uint32_t argument, uint32_t *r3 = nullptr) {
		// Transmit reset command
		#define startBits 0b01000000
		
		SPI_transmit(startBits | index);
		// argument
		if(index == 8) {
			DDRC |= (1 << PC1);
			PORTC |= (1 << PC1);
		}
		for (uint8_t i = 0; i < 4; i++) {
			SPI_transmit((argument > i) & 0xFF);
		}

		if(index == 8) {
			SPI_transmit(0x87);
		} else {
			SPI_transmit(0x01);
		}

		uint8_t r1;
		do {
			r1 = SPI_transmit(0xFF);
		} while (r1 & 0x80);
		return r1;
	}

};

int main()
{
	uart::init();
	//DeltaTimer::init();
	//PWM::init();

	// Set SCK and MOSI as output
	DDRB |= (1 << PB1) | (1 << PB2);

	// Set SS as output and set HIGH to set SD card in SPI mode
	DDRB |= (1 << PB0); // If the SS pin isn't manually set as output, SPI won't do anything
	PORTB |= (1 << PB0);

	// Enable SPI, set Master Mode, (set LSB first,) set prescaler to 128
	SPCR = (1 << SPE) | (1 << MSTR) /*| (1 << DORD)*/ | (1 << SPR0) | (1 << SPR1);

	
	_delay_us(10);

	for(int i = 0; i < 8; i++)
	{
		SPI_transmit(0xFF);
	}

	_delay_us(10);

	PORTB &= ~(1 << PB0);

	_delay_us(10);

	uint8_t data = SD::sendCommand(0, 0);
	uart::writeInt(data);

	data = SD::sendCommand(8, 0x1AA);
	uart::writeInt(data);

	while(1);


	int freq;
	int size = 0;
	uint8_t *midiFILE;

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
	
	lcd_init(LCD_DISP_ON_CURSOR); /*initialize lcd,display on, cursor on */
	lcd_clrscr();             /* clear screen of lcd */
	lcd_home();               /* bring cursor to 0,0 */
	lcd_puts("Hallo");        /* type something random */
	lcd_gotoxy(0,1);          /* go to 2nd row 1st col */
	lcd_puts("Felix");  /* type something random */

	while(1);
	return 0;
}