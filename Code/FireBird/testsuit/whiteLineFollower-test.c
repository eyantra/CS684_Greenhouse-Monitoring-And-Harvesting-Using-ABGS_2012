/* File: whiteLineFollower-test.c
 * Description: Test cases for white line follower module
 * Written By: Devendra Bhave (devendra@cse.iitb.ac.in)
 * Copyright (c) IIT Bombay. All Rights Reserved.
 */

#ifndef USE_AVR_STUDIO 
#error "Use AVR Studio to build testsuit. Use compiler flag -DUSE_AVR_STUDIO."
#endif

 #include <prjParams.h>
 #include <prjCommon.h>
 #include <hal/adc.h>
 #include <hal/lcd.h>
 #include <hal/motor.h>
 #include <hal/buzzer.h>
 #include <whiteLineFollower.h>
 #include <assert.h>
 #include <stdio.h>
 
 
 int main() {
 	UINT wl1, wl2, wl3;
	STATUS ret;
	char buf[32];
	int i;

	initBuzzer();
	initLcd();
	initMotor();
	initAdc();
	initWhiteLineFollower();
    initPower();
    
    moveForwardFollwingLineByDistance(1000ul, TRUE);
    while(1);
	for(i = 0; i < 8; i ++) {
		moveForwardFollwingLineByCheckpoint(2, 1);
		_delay_ms(1000);
		moveForwardFollwingLineByDistance(1000ul, 1);
		_delay_ms(1000);
		rotateBot(RIGHT, 90);
		_delay_ms(1000);
	}

	while(1) {
		ret = getAdcValue(ADC_WHITE_LINE1, &wl1);   /* Left */
		ASSERT(ret == STATUS_OK); 
		ret = getAdcValue(ADC_WHITE_LINE2, &wl2);	/* Middle */
		ASSERT(ret == STATUS_OK);
		ret = getAdcValue(ADC_WHITE_LINE3, &wl3);	/* Right */
		ASSERT(ret == STATUS_OK); 

		lcdCursor(1,1);
		sprintf(buf, "%-4u %-4u %-4u", wl1, wl2, wl3);
		lcdString(buf);
	}


	while(1);
	return 0;
 }
