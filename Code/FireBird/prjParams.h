/* File: prjParams.h
 * Description: Parameter values for the project
 * Written By: Devendra Bhave (devendra@cse.iitb.ac.in)
 * Copyright (c) IIT Bombay. All Rights Reserved.
 */

 #ifndef PRJ_PARAMS_H
 #define PRJ_PARAMS_H

 #ifndef F_CPU
 #define F_CPU 14745600ul	/* CPU Frequency in Hertz */
 #endif /* F_CPU */
 #define BAUD 9600

 #define MOTOR_SAFETY_DELAY    1000
 
 /* Servo motor parameters:
    SERVOx_MIN: Value of OCR1xL register corresponding to zero degrees
    SERVOx_MAX: Value of OCR1xL register corresponding to 180 degrees
  */
 #define SERVO1_MIN 	40
 #define SERVO1_MAX 	135
 #define SERVO2_MIN 	35
 #define SERVO2_MAX 	135
 #define SERVO3_MIN 	35
 #define SERVO3_MAX 	130
 
 #endif /* PRJ_PARAMS_H */
 
