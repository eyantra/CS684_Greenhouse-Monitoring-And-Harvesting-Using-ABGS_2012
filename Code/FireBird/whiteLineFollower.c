 #include <prjParams.h>
 #include <prjCommon.h>
 #include <stdio.h>
 #include <hal/adc.h>
 #include <hal/lcd.h>
 #include <hal/motor.h>
 #include <hal/buzzer.h>
 #include <whiteLineFollower.h>
 #include <assert.h>
 
 static volatile ULINT lPosCount, rPosCount;
 
  void lPosHandler() {
	lPosCount ++;
 }

 void rPosHandler() {
  	rPosCount ++;
 }

 ULINT getPosCountForDistance(ULINT distInMm) {
 	ASSERT(distInMm < (0xFFFFFFFFul / POS_COUNT_PER_METRE));
	return ((ULINT)POS_COUNT_PER_METRE * distInMm) / 1000;
 }

 STATUS readWhiteLineSensors(UINT *pValue) {
	UINT wl1, wl2, wl3;
	STATUS ret;

	ASSERT(pValue != NULL);

	ret = getAdcValue(ADC_WHITE_LINE1, &wl1);   /* Left */
	ASSERT(ret == STATUS_OK); 
	ret = getAdcValue(ADC_WHITE_LINE2, &wl2);	/* Middle */
	ASSERT(ret == STATUS_OK);
	ret = getAdcValue(ADC_WHITE_LINE3, &wl3);	/* Right */
	ASSERT(ret == STATUS_OK); 

	*pValue = COLOR(wl1) << 2 | COLOR(wl2) << 1 | COLOR(wl3);

	return STATUS_OK;
 }

 STATUS whiteLineVelocityAdjust(BOOL *checkpointHit) {
 	UINT idx;
	UINT lBaseVel, rBaseVel;
	BYTE lVel, rVel;
	const UINT velReduction = 0;	/* Used for braking, currently not supported */
	STATUS ret;

	ASSERT(checkpointHit != NULL);

	ret = readWhiteLineSensors(&idx);
	ASSERT(ret == STATUS_OK);

	lBaseVel = BOT_VELOCITY;
	rBaseVel = BOT_VELOCITY + VEL_ADJUST;
	ret = motorVelocityGet(&lVel, &rVel);
	ASSERT(ret == STATUS_OK);

	*checkpointHit = FALSE;

	switch(idx) {
		case 0: *checkpointHit = TRUE;
				break;/* Checkpoint */ 
		case 1: lVel = MAX(BOT_VELOCITY - VEL_DEVIATION, lVel - NOTCH - velReduction);
				rVel = MIN(BOT_VELOCITY + VEL_DEVIATION, rVel + NOTCH - velReduction);
				break;
		case 2: break; /* impossible - ignore */		
		case 3: lVel = MAX(BOT_VELOCITY - VEL_DEVIATION, lVel - 2*NOTCH - velReduction);
				rVel = MIN(BOT_VELOCITY + VEL_DEVIATION, rVel + 2*NOTCH - velReduction);
				break;
		case 4: lVel = MIN(BOT_VELOCITY + VEL_DEVIATION, lVel + NOTCH - velReduction);
				rVel = MAX(BOT_VELOCITY - VEL_DEVIATION, rVel - NOTCH - velReduction);
				break;
		case 5: lVel = lBaseVel;
				rVel = rBaseVel;
				break;
		case 6: lVel = MIN(BOT_VELOCITY + VEL_DEVIATION, lVel + 2*NOTCH - velReduction);
				rVel = MAX(BOT_VELOCITY - VEL_DEVIATION, rVel - 2*NOTCH - velReduction);
				break;
		case 7: lVel = lBaseVel;
				rVel = rBaseVel;
				break;
		default: ASSERT(0);
	}

	motorVelocitySet(lVel, rVel);

 	return STATUS_OK;
 }

 STATUS moveForwardFollwingLineByDistance(ULINT distInMm, BOOL stop) {
	ULINT posCount;
	BOOL checkPoint;
	UINT lVel, rVel;	
	STATUS ret;

	motorLeftPositionEncoderInterruptConfig(INTR_OFF); /* Mask interrupt */
	motorRightPositionEncoderInterruptConfig(INTR_OFF); /* Mask interrupt */

 	/* Configure distance */
 	posCount = getPosCountForDistance(distInMm);
	lPosCount = rPosCount = 0;	/* Reset position counters */
	motorLeftPositionEncoderInterruptConfig(INTR_ON); /* Unmask interrupt */

	/* Start the Bot */
	lVel = BOT_VELOCITY;
	rVel = BOT_VELOCITY + VEL_ADJUST;

	motorVelocitySet(lVel, rVel);
	motorDirectionSet(FORWARD);
			
	while(lPosCount < posCount) {
		ret = whiteLineVelocityAdjust(&checkPoint);
		ASSERT(ret == STATUS_OK);
		
		/* Delay */
//		_delay_ms(DELAY_COUNT);
	}

	motorLeftPositionEncoderInterruptConfig(INTR_OFF); /* Mask interrupt */
	motorRightPositionEncoderInterruptConfig(INTR_OFF); /* Mask interrupt */

    if(stop == TRUE){
	    motorDirectionSet(STOP);
	    _delay_ms(MOTOR_SAFETY_DELAY);
    }

	return STATUS_OK;
 }


 STATUS rotateBot(MotorDirection dir, UINT angle) {
	RotationAutomataState state;
	UINT val, rVel, lVel, turn;
	STATUS ret;

	ASSERT(angle == 90 || angle == 180 || angle == 270);
	ASSERT(dir == LEFT || dir == RIGHT);

	/* Start the Bot */
	lVel = BOT_ROTATION_SPEED;
	rVel = BOT_ROTATION_SPEED + ROT_VEL_ADJUST;

	motorVelocitySet(lVel, rVel);
	motorDirectionSet(dir);
	
	for(turn = 0; turn < angle/90; turn ++) {
		/* Bot may be standing on background */
		ret = readWhiteLineSensors(&val);
		ASSERT(ret == STATUS_OK);
		if(val == 7) {_delay_ms(800);}

		state = RAS_START;
		while(state != RAS_STOP) {
			ret = readWhiteLineSensors(&val);
			ASSERT(ret == STATUS_OK);
		
			switch(state) {
			case RAS_START: if(val == 7) {state = RAS_SEARCH;}
				break;
			case RAS_SEARCH: if(val == 5) {state = RAS_LOCKED;}
				break;
			case RAS_LOCKED: if(val != 5) {state = RAS_STOP;}
				break;
			case RAS_STOP:
				break;
			default: ASSERT(0);
			}

			/* Delay */
			//_delay_ms(DELAY_COUNT);
		}
	}

	motorDirectionSet(STOP);
	_delay_ms(MOTOR_SAFETY_DELAY);

	return STATUS_OK;
 }


 STATUS initWhiteLineFollower(void) {
	STATUS statusCode;

	statusCode = motorLeftPositionEncoderInit(lPosHandler);
	if(statusCode != STATUS_OK)
		return statusCode;

	//motorRightPositionEncoderInit(rPosHandler);

 	return STATUS_OK;
 }

STATUS moveForwardFollwingLineByCheckpoint(UINT chkpts, BOOL stop) {
	BOOL checkPoint, ignoreCheckPoint;
	UINT lVel, rVel;	
	STATUS ret;
	
	/* Start the Bot */
	lVel = BOT_VELOCITY;
	rVel = BOT_VELOCITY + VEL_ADJUST;
	ignoreCheckPoint = FALSE;

	motorVelocitySet(lVel, rVel);
	motorDirectionSet(FORWARD);
		
	while(chkpts > 0) {
		ret = whiteLineVelocityAdjust(&checkPoint);
		ASSERT(ret == STATUS_OK);

		if(checkPoint == TRUE) {
			if(ignoreCheckPoint == FALSE) {
				ignoreCheckPoint = TRUE;
				chkpts --;	
			}
		}
		else {
			ignoreCheckPoint = FALSE;
		}
		
		/* Delay */
		//_delay_ms(DELAY_COUNT);
	}

    
    moveForwardFollwingLineByDistance(75, stop);
	
	return STATUS_OK;
 }
