 #include <stdio.h>
 #include <string.h>
 #include <prjParams.h>
 #include <prjCommon.h>
 #include <hal/adc.h>
 #include <hal/lcd.h>
 #include <hal/motor.h>
 #include <hal/servo.h>
 #include <hal/buzzer.h>
 #include <hal/zigbee.h>
 #include <hal/power.h>
 #include <guidanceSystem.h>
 #include <fileSystem.h>
 #include <whiteLineFollower.h>
 #include <assert.h>

 #define COMMAND_SUCCESS         65 // A
 #define COMMAND_FAIL            66 // B
 #define COMMAND_DATA            67 // C
 #define COMMAND_LEFT            68 // D
 #define COMMAND_RIGHT           69 // E
 #define COMMAND_UP              70 // F
 #define COMMAND_DOWN            71 // G
 #define COMMAND_CUT             72 // H
 #define COMMAND_FORWARD         73 // I
 #define COMMAND_HORZ_RESET      74 // J
 #define COMMAND_VERT_RESET      75 // K
 #define COMMAND_BATTERY_STATUS  76 // L
 #define COMMAND_HARVEST_DONE    77 // M
 #define COMMAND_GOTO            78 // N
 #define COMMAND_FREE_MOTOR      79 // O
 #define COMMAND_CUTTER          80 // P

#define PACKET_SIZE 14
#define PACKET_SENTINEL "!"

STATUS sendCommand(UINT cmd, UINT arg0, UINT arg1) {
    unsigned char rCmd;
    UINT rArg0, rArg1;
    int i;
    char txPacket[20], rxPacket[20];

	/* Send data */
    snprintf(txPacket, sizeof(txPacket), "%c %05u %05u"PACKET_SENTINEL, (unsigned char)cmd,
        arg0, arg1);
	for(i = 0; i < PACKET_SIZE; i ++) {
		putchar(txPacket[i]);
	}
#if 0
	/* Receive data */
	for(i = 0; i < PACKET_SIZE; i ++) {
		rxPacket[i] = getchar();
	}
	
    sscanf(rxPacket, "%c %u %u", &rCmd, &rArg0, &rArg1);
#endif
	return STATUS_OK;
}

STATUS receiveCommand(UINT *cmd, UINT *arg0, UINT *arg1) {
    char sCmd, rCmd;
    int sArg0, sArg1, rArg0, rArg1;
    int i;
    char txPacket[20], rxPacket[20];

    /* Receive data */
    for(i = 0; i < PACKET_SIZE; i ++) {
         rxPacket[i] = getchar();
    }

    sscanf(rxPacket, "%c %d %d", &rCmd, &rArg0, &rArg1);
#if 0
	/* Send data */
    snprintf(txPacket, sizeof(txPacket), "%c %05d %05d"PACKET_SENTINEL, (unsigned char)rCmd,
        rArg0, rArg1);

	for(i = 0; i < PACKET_SIZE; i ++) {
		putchar(txPacket[i]);
	}
#endif
	if(cmd != NULL)
		*cmd = rCmd;
	if(arg0 != NULL)
		*arg0 = rArg0;
	if(arg1 != NULL)
		*arg1 = rArg1;
	return STATUS_OK;
}

void syncCommandStream(){
	while(getchar() != '!');
}

#if 0
STATUS sendCommand(UINT cmd, UINT arg0, UINT arg1) {
	unsigned char rCmd, rArg0, rArg1;
    unsigned char scmd, sarg0,sarg1;
	scmd = (unsigned char) cmd;
    sarg0 = (unsigned char) arg0;
	sarg1 = (unsigned char) arg1;

    fwrite(&scmd,  2, 1, stdout);
    fwrite(&sarg0, 2, 1, stdout);
    fwrite(&sarg1, 2, 1, stdout);

    fread(&rCmd,  1, 1, stdin);
    fread(&rArg0, 1, 1, stdin);
    fread(&rArg1, 1, 1, stdin);

    if(cmd == rCmd && arg0 == rArg0 && rArg1 == arg1) {
        return STATUS_OK;
    }
    return !STATUS_OK;
}

STATUS receiveCommand(UINT *data, UINT *arg0, UINT *arg1) {
    unsigned char rCmd, rArg0, rArg1;

    fread(&rCmd,  1, 1, stdin);
    fread(&rArg0, 1, 1, stdin);
    fread(&rArg1, 1, 1, stdin);

    fwrite(&rCmd,  1, 1, stdout);
    fwrite(&rArg0, 1, 1, stdout);
    fwrite(&rArg1, 1, 1, stdout);

    if(data != NULL)
        *data = (UINT) rCmd;

    if(arg0 != NULL)
        *arg0 = (UINT) rArg0;

    if(arg1 != NULL)
        *arg1 = (UINT) rArg1;

    return STATUS_OK;
} 
#endif
 STATUS getBatteryVoltage(UINT *voltage) {
 	UINT value;
	
 	if(getAdcValue(ADC_BATTERY_VOLTAGE, &value) == STATUS_OK) {
		*voltage = ((value*100)*0.07902) + 0.7;
		return STATUS_OK;
	}
 	return !STATUS_OK;
 }

 int main() {
    Map thisMap;
	STATUS ret;
	UINT cmd, arg0, arg1;
	UINT data;
	UINT sa1 = 90, sa2 = 100;
	char msg[32], text[32];

	/* Initialize hardware */
	ret = initLcd();
	ASSERT(ret == STATUS_OK);
 	ret = initAdc();
	ASSERT(ret == STATUS_OK);
	ret = initMotor();
	ASSERT(ret == STATUS_OK);
	ret = initZigbee();
	ASSERT(ret == STATUS_OK);
	ret = initServo();
	ASSERT(ret == STATUS_OK);
	ret = initBuzzer();
	ASSERT(ret == STATUS_OK);

    //ret = initPower();
    //ASSERT(ret == STATUS_OK);

	/* Initialize software libraries */	

	ret = initFileSystem();
	ASSERT(ret == STATUS_OK);
	ret = initWhiteLineFollower();
	ASSERT(ret == STATUS_OK);
    ret = initBotGuidanceSystem(MAP_FILE, &thisMap);
	ASSERT(ret == STATUS_OK);

    ret = servoSet(SERVO3, 140);
	ASSERT(ret == STATUS_OK);
	ret = servoFree(SERVO3);
	ASSERT(ret == STATUS_OK);

    
	/* Perform task */
	while(1) {
	    lcdCursor(2,1);
		lcdString("Wait");
		receiveCommand(&cmd, &arg0, &arg1);
		buzzerOn();
		_delay_ms(200);
		buzzerOff();

		/* TODO: Command sanity check */
		switch(cmd) {
		 case COMMAND_SUCCESS: 
		 	snprintf(msg, sizeof(msg), "SUCCESS");
		 	break;         
		 case COMMAND_FAIL:   
		 	snprintf(msg, sizeof(msg), "FAIL");
		 	break;         
		 case COMMAND_DATA:   
		 	snprintf(msg, sizeof(msg), "DATA:%x %x", arg0, arg1);
		 	break;         
		 case COMMAND_LEFT:
		 	snprintf(msg, sizeof(msg), "LEFT:%d", arg0);
		 	sa1 -= arg0; 
			if(servoSet(SERVO1, sa1) == STATUS_OK)
			{
				sendCommand(COMMAND_SUCCESS, sa1, 0);
			}
			else{
				sendCommand(COMMAND_FAIL, 0, 0);
			}
			break;
			            
		 case COMMAND_RIGHT:
		 	snprintf(msg, sizeof(msg), "RIGHT:%d", arg0);
		 	sa1 += arg0; 
			if(servoSet(SERVO1, sa1) == STATUS_OK) {
				sendCommand(COMMAND_SUCCESS, sa1, 0);
			}else{
	    		sendCommand(COMMAND_FAIL, 0, 0);
			}			
			break;            

		 case COMMAND_UP:
		 	snprintf(msg, sizeof(msg), "UP:%d", arg0);
		 	sa2 += arg0; 
			if(servoSet(SERVO2, sa2) == STATUS_OK) {
				sendCommand(COMMAND_SUCCESS, 0, 0);
			}else{
				sendCommand(COMMAND_FAIL, 0, 0);
			}			
			break;            

		 case COMMAND_DOWN:
		 	snprintf(msg, sizeof(msg), "DOWN:%d", arg0);
		 	sa2 -= arg0; 
			if(servoSet(SERVO2, sa2) == STATUS_OK) {
				sendCommand(COMMAND_SUCCESS, 0, 0);
			}else{
				sendCommand(COMMAND_FAIL, 0, 0);
			}			
			break;            

		 case COMMAND_CUT:
		 	snprintf(msg, sizeof(msg), "CUT");
		 	servoSet(SERVO3, 0); 
			sa2 = 100; 
			servoSet(SERVO2, sa2);
			servoSet(SERVO3, 140);
			sendCommand(COMMAND_SUCCESS, 0, 0);
			break;             
		 case COMMAND_FORWARD:
		 	snprintf(msg, sizeof(msg), "FORWARD:%d", arg0);
		 	if(gotoForward(&thisMap, arg0) == STATUS_OK) {
				sendCommand(COMMAND_SUCCESS, 0, 0);
			}
			else{
				sendCommand(COMMAND_FAIL, 0, 0);
			}
			break;
		 case COMMAND_HORZ_RESET:
		 	snprintf(msg, sizeof(msg), "HORZ_RESET");
		 	sa1 = 90; 
			if(servoSet(SERVO1, sa1) == STATUS_OK) {
				sendCommand(COMMAND_SUCCESS, 0, 0);
			}else{
				sendCommand(COMMAND_FAIL, 0, 0);
			}			
			break;
		 case COMMAND_VERT_RESET:
		 	snprintf(msg, sizeof(msg), "VERT_RESET");
		 	sa2 = 100; 
			if(servoSet(SERVO2, sa2) == STATUS_OK) {
				sendCommand(COMMAND_SUCCESS, 0, 0);
			}else{
				sendCommand(COMMAND_FAIL, 0, 0);
			}			
			break;
		 case COMMAND_BATTERY_STATUS:
		 	snprintf(msg, sizeof(msg), "BATTETY_STATUS");
		 	if(getBatteryVoltage(&data) == STATUS_OK) {
				sendCommand(COMMAND_SUCCESS, data, 0);
			}
			else{
				sendCommand(COMMAND_FAIL, 0, 0);
			}
			break;
		 case COMMAND_HARVEST_DONE:
		 	snprintf(msg, sizeof(msg), "HARVEST_DONE");
		 	break; 
		 case COMMAND_GOTO:
		 	snprintf(msg, sizeof(msg), "GOTO:%d %d", arg0, arg1);
		 	if(gotoPosition(&thisMap, arg0, arg1) == STATUS_OK) {
				sendCommand(COMMAND_SUCCESS, 0, 0);
			}
			else{
				sendCommand(COMMAND_FAIL, 0, 0);
			}
			break;
		 case COMMAND_FREE_MOTOR:
		 	snprintf(msg, sizeof(msg), "FREE_MOTOR: %d", arg0);
			if(servoFree(arg0) == STATUS_OK) {
				sendCommand(COMMAND_SUCCESS, 0, 0);
			}
			else{
				sendCommand(COMMAND_FAIL, 0, 0);
			}
			break;
		case COMMAND_CUTTER:
			snprintf(msg, sizeof(msg), "CUTTER: %d", arg0);
			if(servoSet(SERVO3, arg0) == STATUS_OK) {
				sendCommand(COMMAND_SUCCESS, 0, 0);
			}
			else{
				sendCommand(COMMAND_FAIL, 0, 0);
			}
			break;

		default:
			snprintf(msg, sizeof(msg), "? %x %x %x", cmd, arg0, arg1);			
			syncCommandStream();
		}
		
		snprintf(text, sizeof(text), "%16s", msg);
		lcdCursor(1,1);
		lcdString(msg);
		
		getBatteryVoltage(&data);
		snprintf(msg, sizeof(msg), "Bat:%d      ", data);
		lcdCursor(2,6);
		lcdString(msg);
	}
    return 0;
 }
