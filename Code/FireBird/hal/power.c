/* File: power.c
 * Description: Hardware abstraction layer for power control of various devices
 * Written By: Devendra Bhave (devendra@cse.iitb.ac.in)
 * Copyright (c) IIT Bombay. All Rights Reserved.
 */
 
 #include <prjParams.h>
 #include <prjCommon.h>
 #include <hal/power.h>
 
 STATUS initPower(void) {
    /* Configure MOSFET switch */
    
    DDRH = DDRH | 0x0C; //make PORTH 3 and PORTH 2 pins as output
    PORTH = PORTH & 0xF3;   /* set PORTH 3 and PORTH 1 pins to 0 */
    DDRG = DDRG | 0x04;     /* make PORTG 2 pin as output */
    PORTG = PORTG & 0xFB;   /* set PORTG 2 pin to 0 */
    
    /* Power down all sensors */
    powerOff(SG_ALL);
	return STATUS_OK;
 }
 
 STATUS powerOn(SensorGroup sg) {
    if(sg == SG_GROUP1 || sg == SG_ALL) {
        PORTG = PORTG & 0xFB;
    }
    if(sg == SG_GROUP2 || sg == SG_ALL) {
        PORTH = PORTH & 0xFB;
    }
    if(sg == SG_GROUP3 || sg == SG_ALL) {
        PORTH = PORTH & 0xF7;
    }
    return STATUS_OK;
 }
 
 STATUS powerOff(SensorGroup sg) {
    if(sg == SG_GROUP1 || sg == SG_ALL) {
        PORTG = PORTG | 0x04;
    }
    if(sg == SG_GROUP2 || sg == SG_ALL) {
        PORTH = PORTH | 0x04;
    }
    if(sg == SG_GROUP3 || sg == SG_ALL) {
        PORTH = PORTH | 0x08;
    }
    return STATUS_OK;    
 }
 
