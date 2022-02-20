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

#define TCCR1A_addr 0x80
#define TCCR3A_addr 0x90
#define TCCR4A_addr 0xA0
#define TCCR5A_addr 0x120

#define TCCR1B_addr 0x81
#define TCCR3B_addr 0x91
#define TCCR4B_addr 0xA1
#define TCCR5B_addr 0x121

#define TCNT1_addr 0x84
#define TCNT3_addr 0x94
#define TCNT4_addr 0xA4
#define TCNT5_addr 0x124

#define DDRB_addr 0x04
#define DDRE_addr 0x0D
#define DDRH_addr 0x101
#define DDRL_addr 0x10A

#define OCNB_PORT1 DDRB_addr
#define OCNB_PORT3 DDRE_addr
#define OCNB_PORT4 DDRH_addr
#define OCNB_PORT5 DDRL_addr

#define COMnB1 5
#define CSn1 1
#define WGMn0 0
#define WGMn3 4

// Pins
#define OC1B PB6 // Pin 12
#define OC3B PE4 // Pin 2
#define OC4B PH4 // Pin 7
#define OC5B PL4 // Pin 45

class PWMTimer
{
	private:
	uint16_t ocnb_port_addr;	// xDRB
	uint16_t ocnb_pin_addr;
	uint16_t top_addr;			// OCRnA (TOP)
	uint16_t com_addr; 			// OCRnB (compare match)
	uint16_t TCCRnA_addr;
	uint16_t TCCRnB_addr;
	uint16_t TCNTn_addr;
	uint16_t top;
	uint8_t duty_cycle;

	public:
	bool isFree;
	uint16_t freq;

	PWMTimer(uint16_t _ocnb_port_addr,
			 uint16_t _ocnb_pin_addr,
			 uint16_t _top_addr,
			 uint16_t _com_addr,
			 uint16_t _TCCRnA_addr,
			 uint16_t _TCCRnB_addr, 
			 uint16_t _TCNTn_addr);
	void start(uint16_t f, uint8_t dc);
	void stop();
};

namespace PWM
{
	namespace
    {
		PWMTimer t1 = PWMTimer(OCNB_PORT1, OC1B, OCR1A_addr, OCR1B_addr, TCCR1A_addr, TCCR1B_addr, TCNT1_addr);
		PWMTimer t3 = PWMTimer(OCNB_PORT3, OC3B, OCR3A_addr, OCR3B_addr, TCCR3A_addr, TCCR3B_addr, TCNT3_addr);
		PWMTimer t4 = PWMTimer(OCNB_PORT4, OC4B, OCR4A_addr, OCR4B_addr, TCCR4A_addr, TCCR4B_addr, TCNT4_addr);
		PWMTimer t5 = PWMTimer(OCNB_PORT5, OC5B, OCR5A_addr, OCR5B_addr, TCCR5A_addr, TCCR5B_addr, TCNT5_addr);
	}

	void start(uint16_t freq, uint8_t dc = 20);

	void stop(uint16_t freq);

    void init();
}