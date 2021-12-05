#pragma once

#include <avr/io.h>

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


class PWMTimer
{
	private:
		uint16_t top_addr;
		uint16_t com_addr; // compare match address
		uint16_t TCCRnB_addr;

	public:
		bool isFree;
		int freq;

		PWMTimer(uint16_t _top_addr, uint16_t _com_addr, uint16_t _TCCRnB_addr);
		void start(int f);
		void stop();
};

namespace PWM
{
	namespace
    {
		PWMTimer t1 = PWMTimer(OCR1A_addr, OCR1B_addr, TCCR1B_addr);
		PWMTimer t3 = PWMTimer(OCR3A_addr, OCR3B_addr, TCCR3B_addr);
		PWMTimer t4 = PWMTimer(OCR4A_addr, OCR4B_addr, TCCR4B_addr);
		PWMTimer t5 = PWMTimer(OCR5A_addr, OCR5B_addr, TCCR5B_addr);
	}

	void start(int freq);

	void stop(int freq);

    void setUp();
}