/* File: assert.c
 * Description: Assertion utility
 * Written By: Devendra Bhave (devendra@cse.iitb.ac.in)
 * Copyright (c) IIT Bombay. All Rights Reserved.
 */
 
 #include <prjParams.h>
 #include <prjCommon.h>
 #include <stdio.h>
 #include <string.h>
 #include <assert.h>
 #include <hal/lcd.h>
 #include <hal/motor.h>
 #include <hal/buzzer.h>
  
 void assert(UINT compResult, char *file, UINT lineNum, char *msg) {
	UINT i;
	char line[33], line1[17], line2[17];
	
	snprintf(line, sizeof(line), "%d: %s %s", lineNum, file, msg);

	if(compResult == 0) {
		/* Halt entire system */
		motorDirectionSet(STOP);
		
		/* Safe copy */
		strncpy(line1, line, sizeof(line1));
		for(i = 0; i < 16 && line[i] != '\0'; i ++);
		if(i >= 16)	strncpy(line2, &(line[16]), sizeof(line2));
		
		lcdClear();
		lcdCursor(1,1);
		lcdString(line1);
		lcdCursor(2,1);
		lcdString(line2);
		
		INT_LOCK();
		while(1) {
			buzzerOn();
			_delay_ms(500);
			buzzerOff();
			_delay_ms(500);
		}
	}	
 }
 
