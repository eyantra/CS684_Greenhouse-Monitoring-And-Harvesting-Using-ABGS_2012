/* File: hal-servo.c
 * Description: Test cases for hardware abstraction layer for servo motor control
 * Written By: Devendra Bhave (devendra@cse.iitb.ac.in)
 * Copyright (c) IIT Bombay. All Rights Reserved.
 */

#ifndef USE_AVR_STUDIO 
#error "Use AVR Studio to build testsuit. Use compiler flag -DUSE_AVR_STUDIO."
#endif

 #include <prjParams.h>
 #include <prjCommon.h>
 #include <hal/buzzer.h>
 #include <hal/motor.h>
 #include <hal/lcd.h>
 #include <hal/servo.h>
 #include <stdio.h>
 #include <assert.h>

 #define DELAY_COUNT  2000
 
 int main() {
	char msg[256];
	int offset, min, max, center;
	STATUS ret;
	
	ret = initBuzzer();
	ASSERT(ret == STATUS_OK);
	ret = initLcd();
	ASSERT(ret == STATUS_OK);
	ret = initMotor();
	ASSERT(ret == STATUS_OK);
	ret = initServo();
	ASSERT(ret == STATUS_OK);

	for(offset = 0; offset < 5; offset ++){
		servoSet(SERVO3, 0);
		servoSet(SERVO3, 120);
	}
#if 0
	for(offset = 0; offset <= 180; offset += 5) {
		servoSet(SERVO3, offset);
		lcdCursor(1, 1);
		snprintf(msg, sizeof(msg), "%16d", offset);
		lcdString(msg);

		_delay_ms(1000);
	}
#endif	
#if 0
	min = 40;
	max = 140;
	center = (max + min) / 2;	
	
	/* Servo1 calibration */
	for (offset = 0; offset < (max - center); offset += 5) {
		OCR1AH = 0;
		OCR1AL = MIN(center+offset, max);
		lcdCursor(1, 1);
		snprintf(msg, sizeof(msg), "%16d", center+offset);
		lcdString(msg);
		
		_delay_ms(DELAY_COUNT);
		
		OCR1AH = 0;
		OCR1AL = MAX(center-offset, min);
		lcdCursor(1, 1);
		snprintf(msg, sizeof(msg), "%16d", center-offset);
		lcdString(msg);
		
		_delay_ms(DELAY_COUNT);

	}
#endif	
	while(1);
	return 0;
 }
