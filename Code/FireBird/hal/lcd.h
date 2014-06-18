/* File: lcd.h
 * Description: LCD control headers
 * Written By: Devendra Bhave (devendra@cse.iitb.ac.in)
 * Copyright (c) IIT Bombay. All Rights Reserved.
 */

 #ifndef LCD_H
 #define LCD_H

#define RS 0
#define RW 1
#define EN 2
#define lcd_port PORTC

STATUS initLcd(void);
void lcdHome();
void lcdClear();
void lcdString(char *str);
void lcdCursor (int row, int column);
 #endif /* LCD_H */
 