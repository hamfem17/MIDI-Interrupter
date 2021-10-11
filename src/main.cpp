#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "defines.h"


void play_b(double freq, int pause) {
  int top = 31250.0/freq;
  OCR1A = top;
  OCR1B = top * 0.5;
  for(int i = 0; i < pause * 124; i++) {
    _delay_ms(1);
  }
}

void play(double freq, int pause) {
  int top = 31250.0/freq;
  OCR1A = top;
  OCR1B = top * 0.5;
  for(int i = 0; i < pause * 114; i++) {
    _delay_ms(1);
  }
  TCCR1B &= ~(1 << CS12);
  for(int i = 0; i < pause * 10; i++) {
    _delay_ms(1);
  }
  TCCR1B |= (1 << CS12);

}

int main()
{
  sei();

  DDRB |= (1 << OC1B); // Pin wird als Ausgang gesetzt

  TCCR1A |= (1 << COM1B1) | (1 << WGM10);
  //TCCR3A |= (1 << COM3A1) | (1 << WGM20);
  //TCCR4A |= (1 << COM4A1) | (1 << WGM30);
  //TCCR5A |= (1 << COM5A1) | (1 << WGM50);

  TCCR1B = (1 << CS12) | (1 << WGM13);   //Prescaler = 256 und wave generation mode
  //TCCR3B = (1 << CS32) | (1 << WGM33);
  //TCCR4B = (1 << CS42) | (1 << WGM43);
  //TCCR5B = (1 << CS52) | (1 << WGM53);
 
  OCR1A =  31250/NOTE_C5;
  OCR1B =   31250/(NOTE_C5 * 2);
 
  //OCR3A =  31250/NOTE_D5;
  //OCR3B =   31250/(NOTE_D5 * 2);
 
  //OCR4A =  31250/NOTE_E5;
  //OCR4B =   31250/(NOTE_E5 * 2);
 
  //OCR5A =  31250/NOTE_F5;
  //OCR5B =   31250/(NOTE_F5 * 2);

    play(NOTE_A4, 1);
    play(NOTE_B4, 1);
    play(NOTE_D5, 1);
    play(NOTE_B4, 1);
    play(NOTE_FS5, 3);
    play(NOTE_FS5, 3);
    play(NOTE_E5, 6);

    play(NOTE_A4, 1);
    play(NOTE_B4, 1);
    play(NOTE_D5, 1);
    play(NOTE_B4, 1);
    play(NOTE_E5, 3);
    play(NOTE_E5, 3);
    play(NOTE_D5, 3);
    play(NOTE_CS5, 1);
    play(NOTE_B4, 2);

    play(NOTE_A4, 1);
    play(NOTE_B4, 1);
    play(NOTE_D5, 1);
    play(NOTE_B4, 1);
    play(NOTE_D5, 4);
    play(NOTE_E5, 2);
    play_b(NOTE_CS5, 3);
    play_b(NOTE_B4, 1);
    play(NOTE_A4, 2);
    play(NOTE_A4, 2);
    play(NOTE_A4, 2);
    play(NOTE_E5, 4);
    play(NOTE_D5, 8);

    play(NOTE_A4, 1);
    play(NOTE_B4, 1);
    play(NOTE_D5, 1);
    play(NOTE_B4, 1);
    play(NOTE_FS5, 3);
    play(NOTE_FS5, 3);
    play(NOTE_E5, 6);

    play(NOTE_A4, 1);
    play(NOTE_B4, 1);
    play(NOTE_D5, 1);
    play(NOTE_B4, 1);
    play(NOTE_A5, 4);
    play(NOTE_E5, 3);
    play(NOTE_D5, 3);
    play(NOTE_CS5, 1);
    play(NOTE_B4, 2);

    play(NOTE_A4, 1);
    play(NOTE_B4, 1);
    play(NOTE_D5, 1);
    play(NOTE_B4, 1);
    play(NOTE_D5, 4);
    play(NOTE_E5, 2);
    play_b(NOTE_CS5, 3);
    play_b(NOTE_B4, 1);
    play(NOTE_A4, 4);
    play(NOTE_A4, 2);
    play(NOTE_E5, 4);
    play(NOTE_D5, 8);
    TCCR1B &= ~(1 << CS12);


  return 0;
} 