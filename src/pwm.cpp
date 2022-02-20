#include "include/pwm.h"

PWMTimer::PWMTimer(uint16_t  _ocnb_port_addr,
				   uint16_t  _ocnb_pin_addr,
				   uint16_t _top_addr,
				   uint16_t _com_addr,
				   uint16_t _TCCRnA_addr,
				   uint16_t _TCCRnB_addr, 
				   uint16_t _TCNTn_addr) {

    top_addr = _top_addr;
    com_addr = _com_addr;
    TCCRnB_addr = _TCCRnB_addr;
    TCNTn_addr = _TCNTn_addr;

    // Set OCnx as output
    _SFR_MEM8(_ocnb_port_addr) |= (1 << _ocnb_pin_addr);

    // Set Phase and Frequency correct PWM Mode with OCRnA as TOP
    _SFR_MEM16(_TCCRnA_addr) |= (1 << WGMn0);
    _SFR_MEM16(_TCCRnB_addr) |= (1 << WGMn3);
    
    // Set OCnx on Compare Match when upcounting, clear when downcounting
    _SFR_MEM16(_TCCRnA_addr) |= (1 << COMnB1);

    isFree = true;
}

void PWMTimer::start(uint16_t f, uint8_t dc) {
    isFree = false;
    freq = f;
    duty_cycle = dc;
    top = 1000000 / f;
    _SFR_MEM16(top_addr) = top;
    _SFR_MEM16(com_addr) = top * (double)duty_cycle / 100.0;
    _SFR_MEM8(TCCRnB_addr) |= (1 << CSn1);
}

void PWMTimer::stop() {
    while(_SFR_MEM16(TCNTn_addr) < (top * (double)duty_cycle / 100.0));
    _SFR_MEM8(TCCRnB_addr) &= ~(1 << CSn1);
    freq = 0;
    isFree = true;
}

void PWM::start(uint16_t freq, uint8_t dc) {
    if(t1.isFree) {
        t1.start(freq, dc);
    } else if(t3.isFree) {
        t3.start(freq, dc);
    } else if(t4.isFree) {
        t4.start(freq, dc);
    } else if(t5.isFree) {
        t5.start(freq, dc);
    }
}

void PWM::stop(uint16_t freq) {
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

void PWM::init() {

	// Set all PWM pins as output
	DDRB |= (1 << OC1B);
	DDRE |= (1 << OC3B);
	DDRH |= (1 << OC4B);
	DDRL |= (1 << OC5B);

	// COMnB1       ... Clear OCnB on Compare Match while upcounting, clear while downcounting
	// WGMn1, WGMn3 ... Phase and Frequency correct PWM Mode with OCRnA as TOP

	TCCR1A |= (1 << COM1B1) | (1 << WGM10);
	TCCR3A |= (1 << COM3B1) | (1 << WGM30);
	TCCR4A |= (1 << COM4B1) | (1 << WGM40);
	TCCR5A |= (1 << COM5B1) | (1 << WGM50);

	TCCR1B |= (1 << WGM13);
	TCCR3B |= (1 << WGM33);
	TCCR4B |= (1 << WGM43);
	TCCR5B |= (1 << WGM53);
}
