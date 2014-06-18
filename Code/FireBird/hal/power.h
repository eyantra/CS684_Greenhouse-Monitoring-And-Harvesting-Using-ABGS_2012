/* File: power.h
 * Description: Power control headers
 * Written By: Devendra Bhave (devendra@cse.iitb.ac.in)
 * Copyright (c) IIT Bombay. All Rights Reserved.
 */

 #ifndef POWER_H
 #define POWER_H

typedef enum tagSensorGroup {
    SG_GROUP1,  /* Sharp IR range sensors 2,3,4 and white line LEDs */
    SG_GROUP2,  /* Sharp IR range sensors 1,5 */
    SG_GROUP3,  /* IR proximity sensors */
    SG_ALL
} SensorGroup;
extern STATUS initPower(void);
extern STATUS powerOn(SensorGroup sg);
extern STATUS powerOff(SensorGroup sg);
 #endif /* POWER_H */
 
