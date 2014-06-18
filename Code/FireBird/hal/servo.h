/* File: servo.h
 * Description: Servo motor control headers
 * Written By: Devendra Bhave (devendra@cse.iitb.ac.in)
 * Copyright (c) IIT Bombay. All Rights Reserved.
 */

 #ifndef SERVO_H
 #define SERVO_H
typedef enum TagServoMotor {
	SERVO1,
	SERVO2,
	SERVO3
} ServoMotor;

extern STATUS initServo(void);
extern STATUS servoSet(ServoMotor servo, UINT position);
extern STATUS servoGet(ServoMotor servo, UINT *position);
extern STATUS servoFree (ServoMotor servo);
 #endif /* SERVO_H */
 
