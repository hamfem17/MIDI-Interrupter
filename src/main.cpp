#include <avr/io.h>
#include <util/delay.h>
#include <math.h>

#include "notes.h"
#include "midi.h"
#include "uart.h"
#include "pwm.h"
#include "deltaTimer.h"
#include "LCD/lcd.h"
#include "pff/diskio.h"

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
		
		// transmit start bits and command index
		SPI_transmit(0b01000000 | index);

		// transmit argument
		for (uint8_t i = 0; i < 32; i+=8) {
			SPI_transmit((argument >> (24 - i)) & 0xFF);
		}

		// transmit CRC and stop bit
		if(index == 8) {
			SPI_transmit(0x87);
		} else {
			SPI_transmit(0x01);
		}

		// get R1 response
		uint8_t r1;
		for(uint8_t i = 0; i < 20; i++) {
			r1 = SPI_transmit(0xFF);
			if(!(r1 & 0x80)) {
				break;
			}
		}

		// get R3 response if present
		if(index == 8) {
			for(uint8_t i = 0; i < 4; i++) {
				*r3 = (*r3 << 8) | SPI_transmit(0xFF);
			}
		}

		return r1;
	}

	void init() {
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

	}
};

FATFS working_area;

int main()
{
	uart::init();
	
	DeltaTimer::init();
	PWM::init();


	int res1 = pf_mount(&working_area);
	//uart::writeInt(res1);

	DIR dp;
	res1 = pf_opendir(&dp, "/");

	FILINFO fno;


	// get number of entries
	uint16_t numberOfFiles = 0; // up to 65536 files
	do {
		pf_readdir(&dp, &fno);
	} while(fno.fname[0] != 0);

	

	

	res1 = pf_open("out.mid");
	//uart::writeInt(res1);

	uint8_t outmid[568];
	for(int i = 0; i < 568; i++) {
		outmid[i] = 0xab;
	}
	unsigned int bytes_read;
	bytes_read = 0; // otherwise pf_read won't work
	res1 = pf_read(outmid, 568, &bytes_read);
	uart::writeInt(res1);
	uart::writeString("   ");
	uart::writeInt(bytes_read);

	/*for(int i = 0; i < 568; i++) {
		uart::debugHex(outmid[i]);
		uart::writeString("\n");
	}*/

	int freq;
	int size = 0;
	//uint8_t *midiFILE;
	
	MIDI decoder = MIDI(outmid);

	if(decoder.readHeader())
	{
		return 1;
	};

	uart::writeString("  DT | EVENT TYPE  | NOTE\n");

	while(1)
	{
		Event event = decoder.getNextEvent();

		uart::writeInt(event.deltaTime);
		uart::writeString("       ");

		uart::debugEvent(event);
		uart::writeString("\n");


		if(event.type == END_OF_TRACK)
		{
			uart::writeString("done");
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
	

	lcd_init(LCD_DISP_ON_CURSOR); /*initialize lcd,display on, cursor on */
	lcd_clrscr();             /* clear screen of lcd */
	lcd_home();               /* bring cursor to 0,0 */
	lcd_puts("Hallo");        /* type something random */
	lcd_gotoxy(0,1);          /* go to 2nd row 1st col */
	lcd_puts("Felix");  /* type something random */

	while(1);
	return 0;
}