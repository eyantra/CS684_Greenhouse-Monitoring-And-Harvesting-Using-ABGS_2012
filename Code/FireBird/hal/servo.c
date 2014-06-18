/* File: servo.c
 * Description: Hardware abstraction layer for servo motor control
 * Written By: Devendra Bhave (devendra@cse.iitb.ac.in)
 * Copyright (c) IIT Bombay. All Rights Reserved.
 */
 
 #include <prjParams.h>
 #include <prjCommon.h>
 #include "servo.h"
 #include <assert.h>

 #define SERVO1_DELAY 150
 #define SERVO2_DELAY 150
 #define SERVO3_DELAY 1
 
//TIMER1 initialization in 10 bit fast PWM mode  
//prescale:256
// WGM: 7) PWM 10bit fast, TOP=0x03FF
// actual value: 52.25Hz 
void timer1Init(void)
{
 TCCR1B = 0x00; //stop
 TCNT1H = 0xFC; //Counter high value to which OCR1xH value is to be compared with
 TCNT1L = 0x01;	//Counter low value to which OCR1xH value is to be compared with
 OCR1AH = 0x03;	//Output compare Register high value for servo 1
 OCR1AL = 0xFF;	//Output Compare Register low Value For servo 1
 OCR1BH = 0x03;	//Output compare Register high value for servo 2
 OCR1BL = 0xFF;	//Output Compare Register low Value For servo 2
 OCR1CH = 0x03;	//Output compare Register high value for servo 3
 OCR1CL = 0xFF;	//Output Compare Register low Value For servo 3
 ICR1H  = 0x03;	
 ICR1L  = 0xFF;
 TCCR1A = 0xAB; /*{COM1A1=1, COM1A0=0; COM1B1=1, COM1B0=0; COM1C1=1 COM1C0=0}
 					For Overriding normal port functionality to OCRnA outputs.
				  {WGM11=1, WGM10=1} Along With WGM12 in TCCR1B for Selecting FAST PWM Mode*/
 TCCR1C = 0x00;
 TCCR1B = 0x0C; //WGM12=1; CS12=1, CS11=0, CS10=0 (Prescaler=256)
}

STATUS initServo(void) {
	INT_LOCK();
	// Servo1 pin config
	DDRB  = DDRB | 0x20;  //making PORTB 5 pin output
	PORTB = PORTB | 0x20; //setting PORTB 5 pin to logic 1
	
	// Servo2 pin config
	DDRB  = DDRB | 0x40;  //making PORTB 6 pin output
	PORTB = PORTB | 0x40; //setting PORTB 6 pin to logic 1
	
	// Servo3 pin config
	DDRB  = DDRB | 0x80;  //making PORTB 7 pin output
	PORTB = PORTB | 0x80; //setting PORTB 7 pin to logic 1
	
	// Initialize timers
	timer1Init();
	
	/* Set initial motor positions */
	OCR1AL = (SERVO1_MIN + SERVO1_MAX)/2;
	OCR1AH = 0x00;
	OCR1BL = (SERVO2_MIN + SERVO2_MAX)/2;
	OCR1BH = 0x00;
	OCR1CL = (SERVO3_MIN + SERVO3_MAX)/2;
	OCR1CH = 0x00;
	
	INT_UNLOCK();
	
	return STATUS_OK;
 }
 
STATUS servoSet(ServoMotor servo, UINT degree) {
	ULINT min, max;
	BYTE data, curr;
	UINT delay = 0;
		
	ASSERT(servo >= SERVO1 && servo <= SERVO3);
	
	switch(servo) {
	case SERVO1: min = SERVO1_MIN; max = SERVO1_MAX; curr = OCR1AL; delay = SERVO1_DELAY; break;
	case SERVO2: min = SERVO2_MIN; max = SERVO2_MAX; curr = OCR1BL; delay = SERVO2_DELAY; break;
	case SERVO3: min = SERVO3_MIN; max = SERVO3_MAX; curr = OCR1CL; delay = SERVO3_DELAY; break;
	default: return !STATUS_OK;
	}
	
	data = (BYTE)(((max - min) * degree) / 180ul + min);
	if(data - curr > 0) {		
		while(curr < data) {
			curr ++;
			switch(servo) {
			case SERVO1: OCR1AL = curr; OCR1AH = 0x00; break;
			case SERVO2: OCR1BL = curr; OCR1BH = 0x00;  break;
			case SERVO3: OCR1CL = curr; OCR1CH = 0x00;  break;
			default: return !STATUS_OK;
			}
			_delay_ms(delay);
		}
	}
	else {
		while(curr > data) {
			curr --;
			switch(servo) {
			case SERVO1: OCR1AL = curr; OCR1AH = 0x00;  break;
			case SERVO2: OCR1BL = curr; OCR1BH = 0x00;  break;
			case SERVO3: OCR1CL = curr; OCR1CH = 0x00;  break;
			default: return !STATUS_OK;
			}
			_delay_ms(delay);
		}
	}	
	
	switch(servo) {
	case SERVO1: OCR1AL = data; OCR1AH = 0x00;  break;
	case SERVO2: OCR1BL = data; OCR1BH = 0x00;  break;
	case SERVO3: OCR1CL = data; OCR1CH = 0x00;  break;
	default: return !STATUS_OK;
	}
	
	return STATUS_OK;
}

STATUS servoGet(ServoMotor servo, UINT *degree) {
	ASSERT(servo >= SERVO1 && servo <= SERVO3);
	ASSERT(degree != NULL);
	
	return !STATUS_OK;
}

//servo_free functions unlocks the servo motors from the any angle 
//and make them free by giving 100% duty cycle at the PWM. This function can be used to 
//reduce the power consumption of the motor if it is holding load against the gravity.

STATUS servoFree (ServoMotor servo) {
	ASSERT(servo >= SERVO1 && servo <= SERVO3);
	
	switch(servo) {
	case SERVO1: OCR1AH = 0x03; OCR1AL = 0xFF; break;
	case SERVO2: OCR1BH = 0x03; OCR1BL = 0xFF; break;
	case SERVO3: OCR1CH = 0x03; OCR1CL = 0xFF; break;
	default: return !STATUS_OK;
	}
	
	return STATUS_OK; 
}
