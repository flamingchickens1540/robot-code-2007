

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

/*#define PAN_ERROR_THRESHOLD			30
#define PAN_ERROR_THRESHOLD_FINE	 4
#define PAN_ROTATE_THRESHOLD		40
#define sgn(x)						((x)<0 ? -1 : ((x)>0 ? 1 : 0))
#define ROT_SPEED 					70
#define ROT_SPEED_FINE 				20
#define PAN_FACING_FORWARD			57 // 30
#define PAN_ERROR					((int)T_Packet_Data.mx - Tracking_Config_Data.Pan_Target_Pixel)
#define DRIVE_SPEED					25
#define DRIVE_SPEED_SLOW			20
#define WIGGLE_SPEED				20
#define	TILT_MAX					185
#define	BLOB_SIZE					T_Packet_Data.pixels
#define	HAPPY_BLOB_SIZE				22
#define TILT_ERROR_PIXELS			(T_Packet_Data.my - Tracking_Config_Data.Tilt_Target_Pixel)
#define ARM_ZERO_SPEED				92

#define GET_FLOATY	0
#define ZERO_ARM	1
#define RAISE_ARM	2
#define ARM_IDLE	3
#define DROP_FLOATY	4
#define DONE_AUTO	5*/


//char atGoal = 0;
int armState = GET_FLOATY;
int count = 0;
unsigned char goalLevel;

void run_auto(void){
	setAutoMode();
	if(ENABLE_AUTO_SWITCH) {
		autoDrive();
		autoArm();
	}
	if(DELAY_AUTO_SWITCH) {
		rotateRightConstant();
	}

	count++;
}

///*
void autoDrive(void)
{
	int actualPanError = (PAN_SERVO - PAN_FACING_FORWARD) + .5 * PAN_ERROR;// + 7);
	static int driveState = 0;
	User_Byte2 = actualPanError+127;
	switch(driveState) {
		case 0: // Initial Drive Forward
			if(count <= 20) {
				// wait...
			}
			else if(count <= 90) {
				driveStraight(127+DRIVE_SPEED);
			}
			else driveState++;
			User_Byte6 = 0;
		break;
		case 1: // Grab the camera, and drive towards it from a straight-on-ish view
			if(Get_Tracking_State() == SEARCHING) driveStraight(127); // Safety Catch-all
			else if(BLOB_SIZE < HAPPY_BLOB_SIZE) {
				// Then we're depressed
				driveStraight(127 + DRIVE_SPEED);
				User_Byte6 = 2;
			}
			else if(abs(actualPanError) > PAN_ERROR_THRESHOLD) {
				//rotate(127 + ROT_SPEED * ((actualPanError < 0) ? -1 : 1));
				if(actualPanError < 0) {
					User_Byte1 = 1;
					rotateLeftConstant();
				}
				else{
					User_Byte1 = 2;
					 rotateRightConstant();
				}
				User_Byte6 = 4;
			}
			else if(abs(actualPanError) > PAN_ERROR_THRESHOLD_FINE) {
				rotate(127 + ROT_SPEED_FINE * ((actualPanError < 0) ? -1 : 1));
				//if(actualPanError < 0) rotateLeftConstant();
				//else rotateRightConstant();
//				User_Byte4 = 127 + ROT_SPEED_FINE * ((actualPanError < 0) ? -1 : 1);
				User_Byte6 = 3;
			}
			else if(TILT_SERVO >= TILT_MAX){//-(TILT_ERROR_PIXELS*(0.8))) {
				driveStraight(127);
				driveState++;
			}
			else {
				//Something's wrong
			}
		break;
		case 2: // backup some so that we get out of the way
			armState = DROP_FLOATY;
		break;
	}
}

//*/

void autoArm()
{
	switch(armState)
	{
		case GET_FLOATY:
			WRISTUP = WRISTOPEN;
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
//			User_Byte1 =1;
			break;
		case RAISE_ARM:
//			setArmGoal(getDesiredArmHeight() + 55);
			if(goalLevel == 1) setArmGoal(LOWDROPHEIGHT+55);
			if(goalLevel == 2) setArmGoal(MEDDROPHEIGHT+55);
			if(goalLevel == 3) setArmGoal(HIGHDROPHEIGHT+55);
			armState=ARM_IDLE;
			break;
		case ARM_IDLE:
			
			break;
		case DROP_FLOATY:
			deployKeeper();			
		//	User_Byte1+=20;
			break;		
	}
	
	if(armState > ZERO_ARM)
		updateArm();
	
	ARMMOTOR = 255 - ARMMOTOR;
}

void deployKeeper(void){
	static int deployCount = 0;
	static int countStart;
	
	if(deployCount == 0) setArmGoal(armGoal-85);
	if(deployCount == 20) countStart = count;
	if(deployCount >= 20){ // Wiggle wiggle wiggle!
		if((count - countStart) <= 15) {
			rotate(127 + WIGGLE_SPEED+5);
		}
		else if((count - countStart) <= 30) {
			rotate(127 - WIGGLE_SPEED);
		}
		
		if((count - countStart) >= 30) {
			countStart = count;
		}
	}
	if(deployCount == 80) {
		WRISTUP = 0; // Drop the wrist
		WRISTDOWN = !WRISTUP;
	}
	if(deployCount == 100) {
		CLAW = CLAWOPEN; // Open the jaw
	}
	if(deployCount >= 120 && deployCount < 160) {
		driveStraight(127 - DRIVE_SPEED_SLOW);
	}
	if(deployCount >= 160){
		driveStraight(127);
	}
	updateArm();
	deployCount++;
}

void driveStraight(int power){
//	drive_code_mecanum(127,power,127);
//	User_Byte5 = 1;
	pwm02 = pwm04 = power + 12;
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
	RFMOTOR = 147;
	LFMOTOR = 147;
	RRMOTOR = 147;
	LRMOTOR = 147;
}
void rotateLeftConstant(void){
	RFMOTOR = 107;
	LFMOTOR = 107;
	RRMOTOR = 107;
	LRMOTOR = 107;
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
