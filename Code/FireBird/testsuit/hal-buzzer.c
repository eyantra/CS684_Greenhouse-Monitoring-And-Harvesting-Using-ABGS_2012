/* File: hal-buzzer.c
 * Description: Test cases for hardware abstraction layer for buzzer control
 * Written By: Devendra Bhave (devendra@cse.iitb.ac.in)
 * Copyright (c) IIT Bombay. All Rights Reserved.
 */

#ifndef USE_AVR_STUDIO 
#error "Use AVR Studio to build testsuit. Use compiler flag -DUSE_AVR_STUDIO."
#endif

 #include <prjParams.h>
 #include <prjCommon.h>
 #include <hal/buzzer.h>
  
 #define DELAY_COUNT  200
 
 
 int main() {

	initBuzzer();

	while(1) {
		buzzerOn();
		_delay_ms(DELAY_COUNT);
		buzzerOff();
		_delay_ms(DELAY_COUNT);
	}
	return 0;
 }
