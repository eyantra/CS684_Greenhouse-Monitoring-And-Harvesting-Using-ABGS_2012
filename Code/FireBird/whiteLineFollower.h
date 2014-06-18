/* File: whiteLineFollower.h
 * Description: Headers for white line follower
 * Written By: Devendra Bhave (devendra@cse.iitb.ac.in)
 * Copyright (c) IIT Bombay. All Rights Reserved.
 */

 #define POS_COUNT_PER_METRE	183
 #define DELAY_COUNT  			20
 #define BOT_VELOCITY  			180
 #define VEL_ADJUST 			15 
 #define VEL_DEVIATION          40
 #define BRAKE_POWER			2		
 #define BOT_ROTATION_SPEED		150
 #define ROT_VEL_ADJUST			15
 #define BRAKE_THRESHOLD        1 
 #define NOTCH 					4
 #define THRESHOLD 				40
 #define COLOR(x)				((x > THRESHOLD)? 0:1)

 typedef enum tagRotationAutomataState {
 	RAS_START,
	RAS_SEARCH,
	RAS_OVER_LINE1,
	RAS_SEARCH2,
	RAS_OVER_LINE2,
	RAS_SEARCH3,
	RAS_LOCKED,
	RAS_STOP
 } RotationAutomataState;

 STATUS initWhiteLineFollower(void);
 STATUS rotateBot(MotorDirection dir, UINT angle);
 STATUS moveForwardFollwingLineByCheckpoint(UINT chkpts, BOOL stop);
 STATUS moveForwardFollwingLineByDistance(ULINT distInMm, BOOL stop);
 
	
