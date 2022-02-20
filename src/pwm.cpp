#include "include/pwm.h"

PWMTimer::PWMTimer(){}

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
    // wait until 
    while(_SFR_MEM16(TCNTn_addr) > (top * (double)duty_cycle / 100.0));
    _SFR_MEM8(TCCRnB_addr) &= ~(1 << CSn1);
    freq = 0;
    isFree = true;
}

void PWM::start(uint16_t freq, uint8_t dc) {
    for(PWMTimer & timer : timers) {
        if(timer.isFree) {
            timer.start(freq, dc);
            break;
        }
    }
}

void PWM::stop(uint16_t freq) {
    for(PWMTimer & timer : timers) {
        if(timer.freq == freq) {
            timer.stop();
            break;
        }
    }
}

void PWM::init() {
    timers[0] = PWMTimer(OCNB_PORT1, OC1B, OCR1A_addr, OCR1B_addr, TCCR1A_addr, TCCR1B_addr, TCNT1_addr);
	timers[1] = PWMTimer(OCNB_PORT3, OC3B, OCR3A_addr, OCR3B_addr, TCCR3A_addr, TCCR3B_addr, TCNT3_addr);
	timers[2] = PWMTimer(OCNB_PORT4, OC4B, OCR4A_addr, OCR4B_addr, TCCR4A_addr, TCCR4B_addr, TCNT4_addr);
	timers[3] = PWMTimer(OCNB_PORT5, OC5B, OCR5A_addr, OCR5B_addr, TCCR5A_addr, TCCR5B_addr, TCNT5_addr);
}