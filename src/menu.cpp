#include "include/menu.h"

#include "LCD/lcd.h"
#include "include/rotary.h"
#include <util/delay.h>

uint8_t menu::selectItem(char** items, uint8_t lastPlayed) {
    uint8_t numberOfItems = 0;
    for(uint8_t i = 0; items[i]; i++) {
        numberOfItems++;
    }
    uint8_t current = lastPlayed;
    int8_t change;

    char s[100];

    lcd_clrscr();             /* clear screen of lcd */
    lcd_home();               /* bring cursor to 0,0 */
    lcd_puts(items[0]);
    lcd_gotoxy(0,1);
    lcd_puts("1");            //LCD File number
    lcd_puts("/");
    itoa(numberOfItems,s,10);
    lcd_puts(s);

    while(1) {
        change = rotary::getChange();
        if(change && current + change >= 0 && current + change < numberOfItems) {
            current += change;
            lcd_clrscr();
            lcd_home();
            lcd_puts(items[current]);
            lcd_gotoxy(0,1);
            itoa(current + 1,s,10);
            lcd_puts(s);            //LCD File number
            lcd_puts("/");
            itoa(numberOfItems,s,10);
        	lcd_puts(s);
        }
        if(rotary::buttonPressed()) {
            return current;
        }
    }
}

void menu::print(char* str) {
    lcd_clrscr();
    lcd_home();
    lcd_puts(str);
}