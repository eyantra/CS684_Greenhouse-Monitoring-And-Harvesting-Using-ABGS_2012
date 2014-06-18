/* File: whiteLineFollower-test.c
 * Description: Test cases for white line follower module
 * Written By: Devendra Bhave (devendra@cse.iitb.ac.in)
 * Copyright (c) IIT Bombay. All Rights Reserved.
 */

#ifndef USE_AVR_STUDIO 
#error "Use AVR Studio to build testsuit. Use compiler flag -DUSE_AVR_STUDIO."
#endif

 #include <stdio.h>
 #include <prjParams.h>
 #include <prjCommon.h>
 #include <hal/adc.h>
 #include <hal/lcd.h>
 #include <hal/motor.h>
 #include <hal/buzzer.h>
 #include <hal/zigbee.h>
 #include <whiteLineFollower.h>
 #include <fileSystem.h>
 #include <assert.h> 
 #include <string.h>
 
 
 int main() {
 	STATUS ret;
	char msg[256];

	ret = initLcd();
	ASSERT(ret == STATUS_OK);
 	//ret = initAdc();
	//ASSERT(ret == STATUS_OK);
	ret = initMotor();
	ASSERT(ret == STATUS_OK);
	ret = initZigbee();
	ASSERT(ret == STATUS_OK);
	ret = initFileSystem();
	ASSERT(ret == STATUS_OK);

	while (!feof(MAP_FILE)){
		fscanf(MAP_FILE, "%s", msg);
		printf("%s", msg);
	}
	while(1);
	return 0;
 }
