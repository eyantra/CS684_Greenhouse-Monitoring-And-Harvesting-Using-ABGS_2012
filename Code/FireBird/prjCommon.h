/* File: prjCommon.h
 * Description: Common definitions for the project
 * Written By: Devendra Bhave (devendra@cse.iitb.ac.in)
 * Copyright (c) IIT Bombay. All Rights Reserved.
 */

 #ifndef PRJ_COMMON_H
 #define PRJ_COMMON_H

 /* Common definitions */

 #define sbit(reg,bit)  reg |= (1<<bit)
 #define cbit(reg,bit)  reg &= ~(1<<bit)
 #define STATUS         int
 #define STATUS_OK      0
 #define INT_LOCK()		cli()
 #define INT_UNLOCK()	sei()
 #define INTR_ON		1
 #define INTR_OFF		0
 #define MIN(x, y)		(((x) < (y))? (x):(y))
 #define MAX(x, y)		(((x) < (y))? (y):(x))
 #define TRUE			1
 #define FALSE			0
 
 typedef unsigned char  BYTE;
 typedef unsigned int  	UINT;
 typedef unsigned long int 	ULINT;
 typedef unsigned int  	BOOL;
 #ifndef NULL
 #define NULL 			(void*)0
 #endif

 /* Common headers */

#ifdef USE_AVR_STUDIO
 #include <avr/io.h>
 #include <avr/interrupt.h>
 #include <util/delay.h>
#endif

 #endif /* PRJ_COMMON_H */
 
