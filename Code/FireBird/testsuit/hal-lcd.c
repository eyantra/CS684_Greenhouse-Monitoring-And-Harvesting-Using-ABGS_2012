/* File: hal-motor.c
 * Description: Test cases for hardware abstraction layer for motor control
 * Written By: Devendra Bhave (devendra@cse.iitb.ac.in)
 * Copyright (c) IIT Bombay. All Rights Reserved.
 */

#ifndef USE_AVR_STUDIO 
#error "Use AVR Studio to build testsuit. Use compiler flag -DUSE_AVR_STUDIO."
#endif

 #include <prjParams.h>
 #include <prjCommon.h>
 #include <hal/lcd.h>
 #include <stdio.h>

 #define DELAY_COUNT  3000
 
 int main() {
	int column;
	char str[24];
	
	/* Test #1: Initialize LCD screen */
	initLcd();
	_delay_ms(DELAY_COUNT);
	
	
	/* Test #2: Write letter in each cell on screen */
	
	for(column = 0; column < 16; column ++) {
		lcdCursor(1, column+1);
		snprintf(str, sizeof(str), "%X", column);
		lcdString(str);
		_delay_ms(DELAY_COUNT/6);
	}
	
	for(column = 0; column < 16; column ++) {
		lcdCursor(2, column+1);
		snprintf(str, sizeof(str), "%x", column);
		lcdString(str);
		_delay_ms(DELAY_COUNT/6);
	}
	
	/* Test #3: Clear screen */
	
	lcdClear();
	_delay_ms(DELAY_COUNT);
		
	/* Test #4: Write first line */
	
	lcdCursor(1, 1);
	snprintf(str, sizeof(str), "----++++----++++0000");
	lcdString(str);
	_delay_ms(DELAY_COUNT);
	
	/* Test #5: Write second line */
	
	lcdCursor(2, 1);
	snprintf(str, sizeof(str), "AAAABBBBAAAABBBB====");
	lcdString(str);
	_delay_ms(DELAY_COUNT);
	
	return 0;
 }
