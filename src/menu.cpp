#include "include/menu.h"

#include "LCD/lcd.h"
#include "include/rotary.h"
#include <util/delay.h>

uint8_t menu::selectItem(char** items) {
    uint8_t numberOfItems = 0;
    for(uint8_t i = 0; items[i]; i++) {
        numberOfItems++;
    }
    uint8_t current = 0;
    int8_t change;
    lcd_clrscr();             /* clear screen of lcd */
    lcd_home();               /* bring cursor to 0,0 */
    lcd_puts(items[0]);        /* type something random */
    lcd_gotoxy(0,1);
    char abc[4];
    abc[1] = '/';
    abc[3] = 0;
    abc[0] = current + 49;
    abc[2] = numberOfItems + 48;
    lcd_puts(abc);
    while(1) {
        change = rotary::getChange();
        if(change && current + change >= 0 && current + change < numberOfItems) {
            current += change;
            lcd_clrscr();
            lcd_home();
            lcd_puts(items[current]);
            lcd_gotoxy(0,1);
            abc[0] = current + 49;
            lcd_puts(abc);
        }
        if(rotary::buttonPressed()) {
            return current;
        }
    }
}