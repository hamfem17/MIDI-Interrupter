#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "notes.h"

// Register Addresses
#define OCR1A_addr 0x88
#define OCR3A_addr 0x98
#define OCR4A_addr 0xA8
#define OCR5A_addr 0x128

#define OCR1B_addr 0x8A
#define OCR3B_addr 0x9A
#define OCR4B_addr 0xAA
#define OCR5B_addr 0x12A

#define TCCR1B_addr 0x81
#define TCCR3B_addr 0x91
#define TCCR4B_addr 0xA1
#define TCCR5B_addr 0x121


// Pins
#define OC1B PB6 // Pin 12
#define OC3B PE4 // Pin 2
#define OC4B PH4 // Pin 7
#define OC5B PL4 // Pin 45

class Timer {
	private:
		uint16_t top_addr;
		uint16_t com_addr; // compare match address
		uint16_t TCCRnB_addr;

	public:
		bool isFree;
		int freq;

		Timer(uint16_t _top_addr, uint16_t _com_addr, uint16_t _TCCRnB_addr) {
			top_addr = _top_addr;
			com_addr = _com_addr;
			TCCRnB_addr = _TCCRnB_addr;
			isFree = true;
		}
		void start(int f) {
			isFree = false;
			freq = f;
			int top = 31250.0 / f;
			_SFR_MEM16(top_addr) = top;
			_SFR_MEM16(com_addr) = top / 2;
			_SFR_MEM8(TCCRnB_addr) |= (1 << 2);
		}
		void stop() {
			_SFR_MEM8(TCCRnB_addr) &= ~(1 << 2);
			freq = 0;
			isFree = true;
		}
};

namespace TM {
	namespace {
		Timer t1 = Timer(OCR1A_addr, OCR1B_addr, TCCR1B_addr);
		Timer t3 = Timer(OCR3A_addr, OCR3B_addr, TCCR3B_addr);
		Timer t4 = Timer(OCR4A_addr, OCR4B_addr, TCCR4B_addr);
		Timer t5 = Timer(OCR5A_addr, OCR5B_addr, TCCR5B_addr);
	}

	void start(int freq) {
		if(t1.isFree) {
			t1.start(freq);
		} else if(t3.isFree) {
			t3.start(freq);
		} else if(t4.isFree) {
			t4.start(freq);
		} else if(t5.isFree) {
			t5.start(freq);
		}
	}

	void stop(int freq) {
		if(t1.freq == freq) {
			t1.stop();
		} else if(t3.freq == freq) {
			t3.stop();
		} else if(t4.freq == freq) {
			t4.stop();
		} else if(t5.freq == freq) {
			t5.stop();
		}
	}

}

void setUpTimer() {

	// Set all PWM pins as output
	DDRB |= (1 << OC1B);
	DDRE |= (1 << OC3B);
	DDRH |= (1 << OC4B);
	DDRL |= (1 << OC5B);

	// COMnB1       ... Clear OCnB on Compare Match while upcounting, clear while downcounting
	// WGMn1, WGMn3 ... Phase and Frequency correct PWM Mode with OCRnA as TOP
	// CSn2         ... Prescaler 256

	TCCR1A |= (1 << COM1B1) | (1 << WGM10);
	TCCR3A |= (1 << COM3B1) | (1 << WGM20);
	TCCR4A |= (1 << COM4B1) | (1 << WGM30);
	TCCR5A |= (1 << COM5B1) | (1 << WGM50);

	TCCR1B |= (1 << CS12) | (1 << WGM13);
	TCCR3B |= (1 << CS32) | (1 << WGM33);
	TCCR4B |= (1 << CS42) | (1 << WGM43);
	TCCR5B |= (1 << CS52) | (1 << WGM53);
}

int main() {
	setUpTimer();

	TM::start(NOTE_A4);
	TM::start(NOTE_C5);
	_delay_ms(1000);
	TM::stop(NOTE_A4);
	TM::start(NOTE_E6);
	TM::stop(NOTE_C5);
	_delay_ms(1000);
	TM::stop(NOTE_E6);

	return 0;
} 