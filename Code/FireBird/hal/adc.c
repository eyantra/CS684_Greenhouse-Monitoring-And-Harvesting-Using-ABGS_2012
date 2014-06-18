/* File: adc.c
 * Description: Hardware abstraction layer for ADC (analog to digital converter)
 * Written By: Devendra Bhave (devendra@cse.iitb.ac.in)
 * Copyright (c) IIT Bombay. All Rights Reserved.
 */
 
 #include <prjParams.h>
 #include <prjCommon.h>
 #include <assert.h>
 #include "adc.h"
  
/*****Function to Initialize ADC *****/
STATUS initAdc(void)
{
    /* Configure ADC pins */
    
    DDRF = 0x00;    /* Set PORTF direction as input */
    PORTF = 0x00;   /* Set PORTF pins floating      */
    DDRK = 0x00;    /* Set PORTK direction as input */
    PORTK = 0x00;   /* Set PORTK pins floating      */
    
    /* Configure ADC registers */
    
    ADCSRA = 0x00;  /* Turn off ADC before changing settings */ 
    ADCSRB = 0x00;  /* MUX5 = 0*/
    ADMUX = 0x20;   /* Vref=5V external , ADLAR=1, MUX4:0 = 0000 */
    ACSR = 0x80;    /* Analog comparator: disabled */
    ADCSRA = 0x86;  /* Enablee ADC, ADEN=1, Disable interrupt, ADIE=0,
                     * ADC prescaler, ADPS2:0 = 1 1 0 */
	return STATUS_OK;
}

STATUS getAdcValue(AdcChannel channel, UINT *value) {
    UINT timeout = 0x1000;
    
    *value = 0;

    ASSERT(channel < ADC_LAST);
	    
    INT_LOCK();
    
    /* Set MUX value */
    if(channel > 7) {
        ADCSRB = 0x08;
    }
    channel = channel & 0x07;   
    ADMUX = 0x20 | channel;
    
    /* Set start conversion bit */
    ADCSRA = ADCSRA | 0x40; 
    
    /* Wait for ADC conversion to complete */
    while(((ADCSRA & 0x10)==0) && (timeout > 0)) {
        timeout --;
    } 
    if(timeout == 0) {
        /* Failure to read ADC value */
        
        INT_UNLOCK();       
        return !STATUS_OK;
    }
    
    *value = ADCH;
    ADCSRA = ADCSRA | 0x10; /* Clear ADIF (ADC Interrupt Flag) by writing 1 to it */
    ADCSRB = 0x00;

    INT_UNLOCK();
    return STATUS_OK;
}
