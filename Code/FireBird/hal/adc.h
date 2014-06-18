/* File: adc.h
 * Description: ADC control headers
 * Written By: Devendra Bhave (devendra@cse.iitb.ac.in)
 * Copyright (c) IIT Bombay. All Rights Reserved.
 */

 #ifndef ADC_H
 #define ADC_H

 typedef enum TagAdcChannel {
	ADC_BATTERY_VOLTAGE = 0x00,
	ADC_WHITE_LINE3 	= 0x01,
	ADC_WHITE_LINE2 	= 0x02,
	ADC_WHITE_LINE1 	= 0x03,
	ADC_IR_PROXIMITY1 	= 0x04,
	ADC_IR_PROXIMITY2 	= 0x05,
	ADC_IR_PROXIMITY3 	= 0x06,
	ADC_IR_PROXIMITY4 	= 0x07,
	ADC_IR_PROXIMITY5 	= 0x20,
	ADC_IR_RANGE1 		= 0x21,
	ADC_IR_RANGE2 		= 0x22,
	ADC_IR_RANGE3 		= 0x23,
	ADC_IR_RANGE4 		= 0x24,
	ADC_IR_RANGE5 		= 0x25,
	ADC_SERVO_POD1 		= 0x26,
	ADC_SERVO_POD2 		= 0x27,
	ADC_LAST
 } AdcChannel;
 
 extern STATUS initAdc(void);
 extern STATUS getAdcValue(AdcChannel, UINT *);
 #endif /* ADC_H */
 