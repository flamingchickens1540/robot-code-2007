

/*******************************************************************************
* FILE NAME: catlin_auto.c
*
* DESCRIPTION:
*  This file contains the catlin autonomous declarations.
*
* USAGE:
*	Any real editing should be done in this file!
*
*******************************************************************************/

#include "catlin_aliases.h"
#include "catlin_routines.h"
#include "catlin_auto_tom.h"
#include "catlin_mecanum.h"
#include "ifi_aliases.h"
#include "camera.h"
#include "terminal.h"
#include "tracking.h"
#include "ifi_default.h"
#include "catlin_arm.h"
#include "interrupts.h"
#include "catlin_control.h"

#define PAN_ERROR_THRESHOLD			30
#define PAN_ERROR_THRESHOLD_FINE	 1
#define PAN_ROTATE_THRESHOLD		40
#define ROT_SPEED 					70
#define ROT_SPEED_FINE 				20
#define PAN_FACING_FORWARD			4
#define PAN_ERROR					((int)T_Packet_Data.mx - Tracking_Config_Data.Pan_Target_Pixel)
#define DRIVE_SPEED					50
#define DRIVE_SPEED_SLOW			30
#define WIGGLE_SPEED				20
#define	TILT_MAX					169
#define	BLOB_SIZE					T_Packet_Data.pixels
/*#define	HAPPY_BLOB_SIZE				22*/
#define TILT_ERROR_PIXELS			(T_Packet_Data.my - Tracking_Config_Data.Tilt_Target_Pixel)
#define ARM_ZERO_SPEED				100

#define GET_FLOATY	0
#define ZERO_ARM	1
#define RAISE_ARM	2
#define ARM_IDLE	3
#define DROP_FLOATY	4
#define DONE_AUTO	5

unsigned char Pan_Min_Default;
unsigned char Pan_Max_Default;
unsigned char Pan_Center_Default;
unsigned char Tilt_Min_Default;
unsigned char Tilt_Max_Default;
unsigned char Tilt_Center_Default;

unsigned char Tilt_Step_Default;
unsigned char Pan_Step_Default;

int armState = GET_FLOATY;
int count = 0;
unsigned char goalLevel;

void run_auto(void){
	setAutoMode();
	if(ENABLE_AUTO_SWITCH) {
		if(DELAY_AUTO_SWITCH) {
			if(count >= 80) {
				autoDrive();
				autoArm();
			}
		}
		else {
			autoDrive();
			autoArm();
		}
	}
	count++;
	
	/*User_Byte1 = TRACKING_MIN_PAN;
	User_Byte2 = TRACKING_MAX_PAN;
	User_Byte3 = TRACKING_CENTER_PAN;
	User_Byte4 = TRACKING_MIN_TILT;
	User_Byte5 = TRACKING_MAX_TILT;
	User_Byte6 = TRACKING_CENTER_TILT;*/
}

///*
void autoDrive(void)
{
	int time;
	int actualPanError = (PAN_SERVO-PAN_ERROR/2.0)+15;
	static int driveState = 0;
	static int secondClock = 0;
	static int oldBlobSize = -1;
	static int blobSize;
	static int searchingTimer = 0;
	static int doneCounter = 0;
	static int nextForward = 20;
	static int testCounter = 0;
	static int outCounter = 0;
	static int saved;
	static int rotateCounter = 0;
	User_Byte3 = driveState;
	switch(driveState) {
		case 0://drive forward
			if(count <= 10){
				//nothing
			}else if(count <= 40){
				driveStraight(127+DRIVE_SPEED);
			}else {
				driveState++;				
				secondClock = count;
				TRACKING_CENTER_TILT = 100;
			}
		break;
		
		case 1://we are driving forward for one second stopping getting camera information
			   //then continuing on
			time = count-secondClock;
			if((time%40) < 15 && (time%40) != 14){//stop
				driveStraight(127);
			}else if((time%40) == 14){
				if(Get_Tracking_State() != SEARCHING){					
					TRACKING_CENTER_TILT = TILT_SERVO;
					TRACKING_CENTER_PAN = PAN_SERVO;
					searchingTimer = 0;
					blobSize = BLOB_SIZE;
					User_Byte1 = blobSize;
					User_Byte2 = oldBlobSize;
					if(blobSize < oldBlobSize && oldBlobSize != -1){		
						driveState++;
						doneCounter = 0;
//						User_Byte5 = 1;
//						restrictCameraPan(0,PAN_SERVO+3);
					}else{
						oldBlobSize = blobSize;
					}
				}else{
					count--;//lets try this again					
				}
			}else{//drive foward
				driveStraight(127+DRIVE_SPEED_SLOW);
			}
		break;
		
		case 2://rotate to face the light
			if(Get_Tracking_State() != SEARCHING){		
				TRACKING_CENTER_TILT = TILT_SERVO;	
				TRACKING_CENTER_PAN = PAN_SERVO;	
				User_Byte4 = 1;
				if(PAN_ERROR > 30 && PAN_SERVO == 0){	
				///driveStraight(127);	 	
				 //	if(rotateCounter > 0) rotateCounter--;
				 //	if(rotateCounter <= 0) {
					 	rotateLeftSlowConstant();
				//	 	rotateCounter = -20;
				//	 }
				}else if(PAN_ERROR < 10 && PAN_SERVO == 0) {
					/*driveStraight(127);
					if(rotateCounter < 0) rotateCounter++;
				 	if(rotateCounter >= 0) {*/
					 	rotateRightSlowConstant();
					 	//rotateCounter = 20;
					 //}
				}else if(PAN_SERVO == 0){//were done
					doneCounter++;
					driveStraight(127);
					if(doneCounter >= 20){
						restrictCameraPan(0,20);
						restrictCameraTilt(TILT_MAX,TILT_MAX-TILT_SERVO+5);
						outCounter = 0;						
						driveState = 3;
						secondClock = count;
						doneCounter = 0;
					}
					
				}else if(abs(actualPanError) < PAN_ERROR_THRESHOLD){//slightly off
					
					//rotate(127 + ROT_SPEED_FINE * ((actualPanError < 0) ? -1 : 1));//slow rotate
					if(actualPanError < 0){
						rotateLeftSlowConstant();
					}else{
						rotateRightSlowConstant();
					}
				}else{//way off
					
					if(actualPanError < 0){
						rotateLeftConstant();
					}else{
						rotateRightConstant();
					}
				}
			}else{
				User_Byte4 = 2;
				RFMOTOR = scale(RFMOTOR,0.99);				
				LFMOTOR = scale(LFMOTOR,0.99);				
				RRMOTOR = scale(RRMOTOR,0.99);				
				LRMOTOR = scale(LRMOTOR,0.99);
			}
		break;
		
		case 3://drive towards light
			time = count-secondClock;
			if(Get_Tracking_State() != SEARCHING && PAN_SERVO == 0 && PAN_ERROR > 40){
				outCounter++;
				if(outCounter == 10){
					driveState = 4;
					doneCounter = 0;
				}				
			}
			if(Get_Tracking_State() != SEARCHING && PAN_ERROR < 20){
				outCounter++;
				if(outCounter == 10){
					driveState = 4;
					doneCounter = 0;
				}	
			}
			if(time%60 < nextForward){//drive foward
				driveStraight(127+DRIVE_SPEED_SLOW);
			}else if(time%60 < 54){//stop
				driveStraight(127);
			}else if(time%60 >= 51){//get reading
				if(Get_Tracking_State() != SEARCHING){
					if(TILT_SERVO == TILT_MAX && TILT_ERROR_PIXELS > 15){
						doneCounter++;
						User_Byte5 = doneCounter;
						count--;
						if(doneCounter > 10) {
							driveState = 5;
							//driveState = 6;
							doneCounter = 0;
						}
						
					}else{
						nextForward = min(nextForward+10,max(10,min(50,(TILT_MAX-TILT_SERVO)*4)));
					}
				}else{
					count--;
				}
			}
			User_Byte6 = nextForward;
		break;
		
		case 4://rotate to face the light slowly
			if(Get_Tracking_State() != SEARCHING){
				if(PAN_ERROR > 30 && PAN_SERVO == 0) rotateLeftSlowConstant();
				else if(PAN_ERROR < 10 && PAN_SERVO == 0) rotateRightSlowConstant();
				else if(PAN_SERVO == 0 &&  PAN_ERROR > 10){//were done
					doneCounter++;
					if(doneCounter == 10){
						driveState = 3;
						doneCounter = 0;
						outCounter = 0;
					}
					driveStraight(127);
				}else driveState = 2;
			}else{
				User_Byte4 = 2;
				RFMOTOR = scale(RFMOTOR,0.99);				
				LFMOTOR = scale(LFMOTOR,0.99);				
				RRMOTOR = scale(RRMOTOR,0.99);				
				LRMOTOR = scale(LRMOTOR,0.99);
			}
		break;
		
		case 5: //deploy the ringer
			armState = DROP_FLOATY;
		break;
		
		case 6:
			driveStraight(127);
		break;
		
		case 7://rotate until you can see the light
			rotateRightConstant();
			if(Get_Tracking_State() != SEARCHING){
				driveState = 2;
			}
		break;
		
		case 8:
			driveStraight(127);
		break;
	}	
}

//*/

void autoArm()
{
	switch(armState)
	{
		case GET_FLOATY:
			WRISTUP = !WRISTOPEN;
			WRISTDOWN = !WRISTUP;
			CLAW=CLAWCLOSED;
			armState=ZERO_ARM;
			break;
		case ZERO_ARM:
			ARMMOTOR=ARM_ZERO_SPEED;
			if(ARMBOTTOMLIM==SWITCHON)
			{
				ARMMOTOR=NEUTRAL;
				checkBottom();
				armState=RAISE_ARM;
			}
			break;
		case RAISE_ARM:
			//setArmGoal(getDesiredArmHeight() + 55);
			if(goalLevel == 1) setArmGoal(LOWDROPHEIGHT+55);
			if(goalLevel == 2) setArmGoal(MEDDROPHEIGHT+55);
			if(goalLevel == 3) setArmGoal(HIGHDROPHEIGHT+55);
			armState=ARM_IDLE;
			break;
		case ARM_IDLE:			
			break;
		case DROP_FLOATY:
			deployKeeper();	
			break;		
	}
	
	if(armState > ZERO_ARM)
		updateArm();
	
	ARMMOTOR = 255 - ARMMOTOR;
}

void deployKeeper(void){
	static int deployCount = 0;
	static int countStart;
	
	if(deployCount == 0) setArmGoal(armGoal-115);
	if(deployCount == 20) countStart = count;
	/*if(deployCount >= 20){ // Wiggle wiggle wiggle!
		if((count - countStart) <= 15) {
			//rotate(127 + WIGGLE_SPEED+5);
			rotateLeftSlowConstant();
		}
		else if((count - countStart) <= 30) {
			//rotate(127 - WIGGLE_SPEED);
			rotateRightSlowConstant();
		}
		
		if((count - countStart) >= 30) {
			countStart = count;
		}
	}*/
	if(deployCount == (80-70)) {
		WRISTUP = WRISTOPEN; // Drop the wrist
		WRISTDOWN = !WRISTUP;
	}
	if(deployCount == (100-70)) {
		CLAW = CLAWOPEN; // Open the jaw
	}
	if(deployCount >= 120-70 && deployCount < 160-60) {
		driveStraight(127 - DRIVE_SPEED_SLOW);
	}
	if(deployCount >= 160-70){
		driveStraight(127);
	}
	updateArm();
	deployCount++;
}

void driveStraight(int power){
//	drive_code_mecanum(127,power,127);
//	User_Byte5 = 1;
	pwm02 = pwm04 = power + 9;
	pwm01 = pwm03 = 255 - power;
}
void driveSide(int power){
	drive_code_mecanum(power,127,127);
	//User_Byte5 = 2;
}
void rotate(int power){
	drive_code_mecanum(127,127,power);
	//User_Byte5 = 3;
	RRMOTOR = 255 - RRMOTOR;	// Apparently these guys need switching
	RFMOTOR = 255 - RFMOTOR;
}
void rotateRightConstant(void){
	RFMOTOR = 168;
	LFMOTOR = 168;
	RRMOTOR = 168;
	LRMOTOR = 168;
}
void rotateLeftConstant(void){
	RFMOTOR = 104;
	LFMOTOR = 104;
	RRMOTOR = 104;
	LRMOTOR = 104;
}
void rotateRightSlowConstant(void){
	RFMOTOR = 153;
	LFMOTOR = 153;
	RRMOTOR = 153;
	LRMOTOR = 153;
}
void rotateLeftSlowConstant(void){
	RFMOTOR = 111;
	LFMOTOR = 111;
	RRMOTOR = 111;
	LRMOTOR = 111;
}
void setAutoMode(void) {
	if(!HIGH_AUTO_SWITCH) goalLevel = 3;
	else if(!LOW_AUTO_SWITCH) goalLevel = 1;
	else goalLevel = 2;
}

int preventWrapAround(int x)
{
	if(x<0) return 0;
	if(x>255) return 255;
	return x;
}
int max(int a, int b){
	if(a > b) return a;
	return b;
}

void restrictCameraPan(int center, int buffer) {
	TRACKING_CENTER_PAN = center;
	if((center - buffer) > Pan_Min_Default) TRACKING_MIN_PAN = center - buffer;
	else TRACKING_MIN_PAN = Pan_Min_Default;
	
	if((center + buffer) < Pan_Max_Default) TRACKING_MAX_PAN = center + buffer;
	else TRACKING_MAX_PAN = Pan_Max_Default;
	
	Set_Pan_Servo_Position(center);
}

void restrictCameraTilt(int center, int buffer) {
	TRACKING_CENTER_TILT = center;
	if((center - buffer) > Tilt_Min_Default) TRACKING_MIN_TILT = center - buffer;
	else TRACKING_MIN_TILT = Tilt_Min_Default;
	
	if((center + buffer) < Tilt_Max_Default) TRACKING_MAX_TILT = center + buffer;
	else TRACKING_MAX_TILT = Tilt_Max_Default;
	
	Set_Tilt_Servo_Position(center);
}

void resetCameraPan(void) {
	TRACKING_MIN_PAN		= Pan_Min_Default;
	TRACKING_MAX_PAN		= Pan_Max_Default;
	TRACKING_CENTER_PAN		= Pan_Center_Default;
	TRACKING_STEP_PAN		= Pan_Step_Default;
	
	Set_Pan_Servo_Position(TRACKING_CENTER_PAN);
}
void resetCameraTilt(void) {
	TRACKING_MIN_TILT		= Tilt_Min_Default;
	TRACKING_MAX_TILT		= Tilt_Max_Default;
	TRACKING_CENTER_TILT	= Tilt_Center_Default;
	TRACKING_STEP_TILT		= Pan_Step_Default;
	
	Set_Tilt_Servo_Position(TRACKING_CENTER_TILT);
}
void resetCameraView(void) {
	resetCameraPan();
	resetCameraTilt();
}

void setCameraPanStep(unsigned char step) {
	if((TRACKING_MAX_PAN - TRACKING_MIN_PAN) > step) TRACKING_STEP_PAN = step;
	else TRACKING_STEP_PAN = (TRACKING_MAX_PAN - TRACKING_MIN_PAN);
}

void setCameraTiltStep(unsigned char step) {
	if((TRACKING_MAX_TILT - TRACKING_MIN_TILT) > step) TRACKING_STEP_TILT = step;
	else TRACKING_STEP_TILT = (TRACKING_MAX_TILT - TRACKING_MIN_TILT);
}

void autoInitialize(void) {
	Pan_Min_Default			= TRACKING_MIN_PAN;
	Pan_Max_Default			= TRACKING_MAX_PAN;
	Pan_Center_Default		= TRACKING_CENTER_PAN;
	Tilt_Min_Default		= TRACKING_MIN_TILT;
	Tilt_Max_Default		= TRACKING_MAX_TILT;
	Tilt_Center_Default		= TRACKING_CENTER_TILT;
	
	Pan_Step_Default		= TRACKING_STEP_PAN;
	Tilt_Step_Default		= TRACKING_STEP_TILT;
}

/*	
void autoArm(void)
	if(atGoal == 0){
		if(zeroedState == 0) {
			WRISTUP = 1;
			CLAW = 1;
			ARMMOTOR = 92;
			if(ARMBOTTOMLIM == SWITCHON) {
				zeroedState = 1;
				clickRotationsArm = 0;
			}
		}
		else {
			setArmGoal(LOWDROPHEIGHT+55);
			updateArm();
		}
	}else{
		deployKeeper();
	}
}
*/

/*
void goTowardsLight(void)
{
	int actualPanError = (PAN_SERVO - PAN_FACING_FORWARD + 10);
	
//	User_Byte1 = atGoal;
	if(count <= 70) {
		driveStraight(127+DRIVE_SPEED);
		User_Byte6 = 0;
	}
	else if(Get_Tracking_State() == SEARCHING) {
		driveStraight(127);
		User_Byte6 = 1;
	}
	else if(BLOB_SIZE < HAPPY_BLOB_SIZE) {
		// Then we're depressed
		driveStraight(127 + DRIVE_SPEED);
		User_Byte6 = 2;
	}
	else if(actualPanError > PAN_ERROR_THRESHOLD_FINE) {
		rotate(127 + ROT_SPEED_FINE * ((actualPanError < 0) ? -1 : 1));
		User_Byte4 = 127 + ROT_SPEED_FINE * ((actualPanError < 0) ? -1 : 1);
		User_Byte6 = 3;
	}
	else if(actualPanError > PAN_ERROR_THRESHOLD) {
		rotate(127 + ROT_SPEED * ((actualPanError < 0) ? -1 : 1));
		User_Byte6 = 4;
	}
	else if(TILT_SERVO >= TILT_MAX-(TILT_ERROR_PIXELS*(0.8))) {
		driveStraight(127);
//		atGoal = 1;
		armState = DROP_FLOATY;
	}
	else {
		driveStraight(127+DRIVE_SPEED);
		User_Byte6 = 66;
	}
}
//*/
