/* File: motor.c
 * Description: Hardware abstraction layer for motor control
 * Written By: Devendra Bhave (devendra@cse.iitb.ac.in)
 * Copyright (c) IIT Bombay. All Rights Reserved.
 */
 
 #include <prjParams.h>
 #include <prjCommon.h>
 #include "motor.h"
 #include <assert.h>

static void (*lposIsr) (void); 
static void (*rposIsr) (void);

static void timer5Init(void) {
    /* Timer 5 initialized in PWM mode for velocity control
     * Prescale:256
     * PWM 8bit fast, TOP=0x00FF
     * Timer Frequency:225.000Hz
     */
    TCCR5B = 0x00;  //Stop
    TCNT5H = 0xFF;  //Counter higher 8-bit value to which OCR5xH value is compared with
    TCNT5L = 0x01;  //Counter lower 8-bit value to which OCR5xH value is compared with
    OCR5AH = 0x00;  //Output compare register high value for Left Motor
    OCR5AL = 0xFF;  //Output compare register low value for Left Motor
    OCR5BH = 0x00;  //Output compare register high value for Right Motor
    OCR5BL = 0xFF;  //Output compare register low value for Right Motor
    OCR5CH = 0x00;  //Output compare register high value for Motor C1
    OCR5CL = 0xFF;  //Output compare register low value for Motor C1
    TCCR5A = 0xA9;  /*{COM5A1=1, COM5A0=0; COM5B1=1, COM5B0=0; COM5C1=1 COM5C0=0}
                     For Overriding normal port functionality to OCRnA outputs.
                     {WGM51=0, WGM50=1} Along With WGM52 in TCCR5B for Selecting FAST PWM 8-bit Mode */

    TCCR5B = 0x0B;  //WGM12=1; CS12=0, CS11=1, CS10=1 (Prescaler=64)
}

 /* Interrupt safe */
 STATUS initMotor(void) {
    /* Configure pins */
    
    DDRA = DDRA | 0x0F;     /* Motor control bit direction: out */
    PORTA = PORTA & 0xF0;   /* Stop all motors */
    
    DDRL = DDRL | 0x18;     /* Setting PL3 and PL4 pins as output for PWM
                                generation */
    PORTL = PORTL | 0x18;   /* PL3 and PL4 pins are for velocity control
                                using PWM. */
    timer5Init();           /* Needed for velocity control */                           
    return STATUS_OK;
 }
 
 /* Interrupt unsafe */
 STATUS motorDirectionSet(MotorDirection direction) { 
    BYTE PortARestore = 0;

	INT_LOCK();
    direction &= 0x0F;          /* removing upper nibbel for the protection */
    PortARestore = PORTA;       /* reading the PORTA original status */
    PortARestore &= 0xF0;       /* making lower direction nibbel to 0 */
    PortARestore |= direction;  /* adding lower nibbel for forward command and 
                                    restoring the PORTA status */
    PORTA = PortARestore;       /* executing the command */

	INT_UNLOCK();
    return STATUS_OK;
 }
 
 /* Interrupt unsafe */
 STATUS motorVelocitySet(BYTE leftMotor, BYTE rightMotor) {
	INT_LOCK();
    OCR5AL = leftMotor;
    OCR5BL = rightMotor;
    INT_UNLOCK();
	
    return STATUS_OK;
 }
 
  /* Interrupt unsafe */
 STATUS motorVelocityGet(BYTE *leftMotor, BYTE *rightMotor) {
 	ASSERT(leftMotor != NULL);
	ASSERT(rightMotor != NULL);

	INT_LOCK();
    *leftMotor = OCR5AL;
    *rightMotor = OCR5BL;
    INT_UNLOCK();
	
    return STATUS_OK;
 }

 /* Interrupt unsafe */
 STATUS motorLeftPositionEncoderInit(void (*callbackLIntr)(void)) {

	/* Check if argument is invalid or callback is already registered */

	if(callbackLIntr == NULL || lposIsr != NULL)
		return !STATUS_OK;
		
	INT_LOCK();
	EICRB = EICRB | 0x02;	/* INT4 is set to falling edge */
	EIMSK = EIMSK | 0x10;	/* Enable INT4 */
	INT_UNLOCK();
	
	lposIsr = callbackLIntr;
	return STATUS_OK;
}

/* Interrupt unsafe */
STATUS motorRightPositionEncoderInit(void (*callbackRIntr)(void)) {

	/* Check if argument is invalid or callback is already registered */

	if(callbackRIntr == NULL || rposIsr != NULL)
		return !STATUS_OK;
		
	INT_LOCK();
	EICRB = EICRB | 0x08;	/* INT5 is set to falling edge */
	EIMSK = EIMSK | 0x20;	/* Enable INT5 */
	INT_UNLOCK();
	
	rposIsr = callbackRIntr;
	return STATUS_OK;
}

/* Interrupt safe */
void motorLeftPositionEncoderInterruptConfig(UINT state) {
		
	if(state != INTR_OFF)
		EIMSK = EIMSK | 0x10;	/* Enable INT4 */
	else
		EIMSK = EIMSK & 0xEF;	/* Disable INT4 */	
}

/* Interrupt safe */
void motorRightPositionEncoderInterruptConfig(UINT state) {
		
	if(state != INTR_OFF)
		EIMSK = EIMSK | 0x20;	/* Enable INT5 */
	else
		EIMSK = EIMSK & 0xDF;	/* Disable INT5 */
}

ISR(INT4_vect) {
	if(lposIsr) {
		(*lposIsr)();
	}
}

ISR(INT5_vect) {
	if(rposIsr) {
		(*rposIsr)();
	}
}
