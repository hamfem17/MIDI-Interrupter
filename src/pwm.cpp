#include "include/pwm.h"

PWMTimer::PWMTimer(uint16_t _top_addr, uint16_t _com_addr, uint16_t _TCCRnB_addr, uint16_t _TCNTn_addr)
{
    top_addr = _top_addr;
    com_addr = _com_addr;
    TCCRnB_addr = _TCCRnB_addr;
    TCNTn_addr = _TCNTn_addr;
    isFree = true;
}
void PWMTimer::start(int f)
{
    isFree = false;
    freq = f;
    top = 1000000 / f;
    _SFR_MEM16(top_addr) = top;
    _SFR_MEM16(com_addr) = top / 2; // if this is changed, also change PWMTimer::stop()
    _SFR_MEM8(TCCRnB_addr) |= (1 << 1);
}
void PWMTimer::stop()
{
    //wwhile(_SFR_MEM16(TCNTn_addr) < (top / 2)); // this waits until the output is off
    _SFR_MEM8(TCCRnB_addr) &= ~(1 << 1);
    freq = 0;
    isFree = true;
}
void PWM::start(int freq)
{
    if(t1.isFree)
    {
        t1.start(freq);
    } else if(t3.isFree) {
        t3.start(freq);
    } else if(t4.isFree) {
        t4.start(freq);
    } else if(t5.isFree) {
        t5.start(freq);
    }
}

void PWM::stop(int freq)
{
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
