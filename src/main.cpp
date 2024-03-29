#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "include/notes.h"
#include "include/midi.h"
#include "include/uart.h"
#include "include/pwm.h"
#include "include/deltaTimer.h"
#include "include/rotary.h"
#include "include/menu.h"
#include "LCD/lcd.h"
#include "pff/diskio.h"

const int chipSelect = 53;

int main()
{
	uart::init();
	DeltaTimer::init();
	PWM::init();
	rotary::init();
	lcd_init(LCD_DISP_ON_CURSOR); /*initialize lcd,display on, cursor on */

	uart::writeString("done initializing\n");

	EIFR |= (1 << INTF4); // otherwise INT4_vect will be executed immediatly
	sei();

	FATFS working_area;
	int res1 = pf_mount(&working_area);
	if(res1) {
		uart::writeString("Failed mounting SD device\n");
		menu::print("Mount Error");
		return 1;
	} else {
		uart::writeString("mounted SD device\n");
	}

	DIR dp;
	res1 = pf_opendir(&dp, "/");
	if(res1) {
		uart::writeString("Failed opening \"/\"\n");
		menu::print("Opening Error");
		return 1;
	} else {
		uart::writeString("Opened \"/\"\n");
	}

	FILINFO fno;

	// get number of entries
	uint16_t numberOfFiles = 0; // up to 65536 files
	for(int i = 0; i < 20; i++) {
		pf_readdir(&dp, &fno);
		if(fno.fname[0] == 0) break;
		else if(fno.fattrib & AM_DIR) continue;
		else numberOfFiles++;
	}
	pf_readdir(&dp, nullptr); // rewind read index
	uart::writeString("Number of files: ");
	uart::writeInt(numberOfFiles);
	uart::writeString("\n");

	char** files = (char**)malloc(numberOfFiles * sizeof(char*) + 1); // maybe add 2?
	UINT* fileSize = (UINT*)malloc(numberOfFiles * sizeof(UINT) + 1); // maybe add 2?

	for(size_t i = 0; i < numberOfFiles;) {
		pf_readdir(&dp, &fno);

		if(fno.fattrib & AM_DIR) continue;
		files[i] = (char*)malloc(strlen(fno.fname)+1);
		fileSize[i] = fno.fsize;
		strcpy(files[i], fno.fname);
		i++;
	}

	files[numberOfFiles] = nullptr;

	uint8_t selection = 0;
	while (1)
	{
		selection = menu::selectItem(files, selection);

		pf_open(files[selection]);

		uint8_t* midiFile = (uint8_t*)malloc(fileSize[selection]);
		if (midiFile == nullptr)
		{
			menu::print("Malloc Error");
			return 1;
		}

		UINT bytes_read = 0; // otherwise pf_read won't work
		res1 = pf_read(midiFile, fileSize[selection], &bytes_read);
		if(res1) {
			uart::writeString("Could not read file\n");
			uart::writeString("Error ");
			uart::writeInt(res1);
			menu::print("Read Error");
			return 1;
		} else {
			uart::writeString("Read file\n");
		}

		int freq;
		int size = 0;
		
		MIDI decoder = MIDI(midiFile);

		if(decoder.readHeader())
		{
			return 1;
		}

		menu::print("Playing...");
		while(1)
		{
			Event event = decoder.getNextEvent();

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
		free(midiFile);
	}
	return 0;
}