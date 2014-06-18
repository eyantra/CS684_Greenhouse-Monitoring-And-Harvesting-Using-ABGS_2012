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
 #include <hal/motor.h>
 #include <hal/lcd.h>
 #include <stdio.h>

 #define DELAY_COUNT  2000
 
 static int lPosCount;
 static char buf[20];

 static void leftPosEncoderIsr(void) {
	lPosCount --;
	if(lPosCount <= 0) {
		motorDirectionSet(STOP);
	}
 }

 int main() {
	MotorDirection dirs[] = {FORWARD, BACKWARD, RIGHT, LEFT,
			SOFT_RIGHT, SOFT_LEFT, SOFT_RIGHT2, SOFT_LEFT2, STOP};
	char strDirs[9][16] = {"FORWARD", "BACKWARD", "RIGHT", "LEFT",
			"SOFT_RIGHT", "SOFT_LEFT", "SOFT_RIGHT2", "SOFT_LEFT2", "STOP"};

	int idx;
	
	initMotor();
	initLcd();

#if 0
	/* Test #1: Check direction */
	
	motorVelocitySet(255, 255);

	for(idx = 0; idx < (sizeof(dirs)/sizeof(dirs[0])); idx ++) {
		lcdClear();
		lcdCursor(1,1);
		lcdString(strDirs[idx]);
		_delay_ms(DELAY_COUNT*2);
		motorDirectionSet(dirs[idx]);
		
		/* Delay for some time */
		
		_delay_ms(DELAY_COUNT);
				
		motorDirectionSet(STOP);

	}
	
	/* Test #2: Check speed */

	lcdClear();
	lcdCursor(1,1);
	lcdString("Speed : ");
	lcdCursor(2,1);
	lcdString("L 100, R 100");
	_delay_ms(DELAY_COUNT*2);

	motorVelocitySet(100, 100);
	motorDirectionSet(FORWARD);
		
	/* Delay for some time */

	_delay_ms(DELAY_COUNT);
		
	motorDirectionSet(STOP);

	/* Test #3: Check individual wheel speed (L < R) */
	
	lcdClear();
	lcdCursor(1,1);
	lcdString("Speed : ");
	lcdCursor(2,1);
	lcdString("L 100, R 255");
	_delay_ms(DELAY_COUNT*2);

	motorVelocitySet(100, 255);	/* Left turn expected */
	motorDirectionSet(FORWARD);
	
	/* Delay for some time */
	
	_delay_ms(DELAY_COUNT);	
	
	motorDirectionSet(STOP);

	/* Test #4: Check individual wheel speed (L > R) */

	lcdClear();
	lcdCursor(1,1);
	lcdString("Speed : ");
	lcdCursor(2,1);
	lcdString("L 255, R 100");
	_delay_ms(DELAY_COUNT*2);

	motorVelocitySet(255, 100);	/* Right turn expected */
	motorDirectionSet(FORWARD);
	
	/* Delay for some time */
	
	_delay_ms(DELAY_COUNT);
		
	motorDirectionSet(STOP);


	/* Test #5: Check left position encoder */

	motorLeftPositionEncoderInit(leftPosEncoderIsr);
	lPosCount = 100;
	motorVelocitySet(255, 255);
	motorDirectionSet(FORWARD);
#endif

	
	/* Test #6: Check orientation */
	motorLeftPositionEncoderInit(leftPosEncoderIsr);
	lPosCount = 23;
	motorVelocitySet(150, 150);
	motorDirectionSet(LEFT);


	while(1);
	return 0;
 }
