/* File: hal-adc.c
 * Description: Test cases for hardware abstraction layer for motor control
 * Written By: Devendra Bhave (devendra@cse.iitb.ac.in)
 * Copyright (c) IIT Bombay. All Rights Reserved.
 */

#ifndef USE_AVR_STUDIO 
#error "Use AVR Studio to build testsuit. Use compiler flag -DUSE_AVR_STUDIO."
#endif

 #include <prjParams.h>
 #include <prjCommon.h>
 #include <stdio.h>
 #include <hal/adc.h>
 #include <hal/lcd.h>
 
 #define DELAY_COUNT  2000
 

 int main() {
	AdcChannel channel[] = {
		ADC_BATTERY_VOLTAGE,
		ADC_WHITE_LINE1,
		ADC_WHITE_LINE2,
		ADC_WHITE_LINE3,
		ADC_IR_PROXIMITY1,
		ADC_IR_PROXIMITY2,
		ADC_IR_PROXIMITY3,
		ADC_IR_PROXIMITY4,
		ADC_IR_PROXIMITY5,
		ADC_IR_RANGE1,
		ADC_IR_RANGE2,
		ADC_IR_RANGE3,
		ADC_IR_RANGE4,
		ADC_IR_RANGE5,
		ADC_SERVO_POD1,
		ADC_SERVO_POD1,
		ADC_LAST
	};

	char strChannel[17][20] = {
		"ADC_BATTERY_VOLTAGE",
		"ADC_WHITE_LINE1",
		"ADC_WHITE_LINE2",
		"ADC_WHITE_LINE3",
		"ADC_IR_PROXIMITY1",
		"ADC_IR_PROXIMITY2",
		"ADC_IR_PROXIMITY3",
		"ADC_IR_PROXIMITY4",
		"ADC_IR_PROXIMITY5",
		"ADC_IR_RANGE1",
		"ADC_IR_RANGE2",
		"ADC_IR_RANGE3",
		"ADC_IR_RANGE4",
		"ADC_IR_RANGE5",
		"ADC_SERVO_POD1",
		"ADC_SERVO_POD2",
		"ADC_LAST"
	};

	char buf[17];
	UINT value, idx;

	initAdc();
	initLcd();
	
	for(idx = 0; idx < (sizeof(channel)/sizeof(channel[0])); idx ++) {
		lcdClear();
		lcdCursor(1,1);
		lcdString(strChannel[idx]);
		lcdCursor(2,1);

		if(getAdcValue(channel[idx], &value) == STATUS_OK) {
			snprintf(buf, sizeof(buf), "%d", value);
			lcdString(buf);
		}
		else {
			lcdString("Error");
		}
		_delay_ms(DELAY_COUNT);
	}
	

	return 0;
 }
