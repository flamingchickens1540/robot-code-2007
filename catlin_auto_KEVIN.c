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
#include "ifi_aliases.h"
#include "catlin_aliases.h"
#include "catlin_auto.h"
#include "catlin_mecanum.h"
#include "catlin_gyro.h"
#include "ifi_default.h"
#include "tracking.h"
#include "math.h"
#include "interrupts.h"
#include "tracking.h"

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
#include "catlin_auto.h"
#include "catlin_mecanum.h"
#include "ifi_aliases.h"
#include "camera.h"
#include "terminal.h"
#include "tracking.h"
#include "ifi_default.h"
#include "catlin_arm.h"

#define UPPER_TILT_THRESHOLD ???
#define PAN_ERROR_THRESHOLD 60
#define sgn(x) ((x)<0 ? -1 : ((x)>0 ? 1 : 0))
#define ROT_SPEED 30
#define PAN_FACING_FORWARD 138
#define PAN_ERROR ((int)T_Packet_Data.mx - Tracking_Config_Data.Pan_Target_Pixel)
#define DRIVE_SPEED 35
#define MIN_SPEED 5
#define ANGLE_CUTOFF	200
#define TILT_MAX 200
#define BLOB_SIZE	T_Packet_Data.pixels
#define	HAPPY_BLOB_SIZE	14


void handleArm(void);

void goTowardsLight(void)
{
	
	
	int actualPanError=abs(PAN_SERVO-PAN_FACING_FORWARD);
	float speed;
	static int count = 0;
	User_Byte1=0;
	Catlin_Common_First();
	
	if(Get_Tracking_State()==SEARCHING)
	{
		driveStraight(127);
		User_Byte1=1;
	}
	else if((count <= 80) || (BLOB_SIZE < HAPPY_BLOB_SIZE)) {
		// Then we're depressed...
		driveStraight(150);
		count++;
	}
	else if(TILT_ANGLE > ANGLE_CUTOFF) {
		driveStraight(127);
	}
	else if(abs(actualPanError)>PAN_ERROR_THRESHOLD) //There's too much error so we should rotate
	{
		if(PAN_SERVO>PAN_FACING_FORWARD)
			rotate(127+ROT_SPEED);
		else
			rotate(127-ROT_SPEED);
		//speed=0.075*((float)(PAN_SERVO-PAN_FACING_FORWARD));
		//User_Byte2=(unsigned char)speed;
		//rotate(127+((int)speed));
		User_Byte1=2;
	}
	else //There is little error so we should drive straight ahead
	{
		driveStraight(127+DRIVE_SPEED);
		User_Byte1=3;
	}
}

void run_auto(void){
	goTowardsLight();
	handleArm();
	
	
	RRMOTOR = 255 - RRMOTOR;	// Apparently these guys need switching
	RFMOTOR = 255 - RFMOTOR;
	ARMMOTOR = 255 - ARMMOTOR;
}

#define GET_FLOATY 0
#define ZERO_ARM 1
#define RAISE_ARM 2
#define ARM_IDLE 3
#define DROP_FLOATY 4
#define DONE_AUTO 5

#define ARM_ZERO_SPEED 92

void handleArm(void)
{
	static int state=GET_FLOATY;
	static int timer=0;
	
	switch(state)
	{
		case GET_FLOATY:
			WRISTUP = WRISTOPEN;
			WRISTDOWN = !WRISTUP;
			CLAW=CLAWCLOSED;
			state=ZERO_ARM;
			break;
		case ZERO_ARM:
			ARMMOTOR=ARM_ZERO_SPEED;
			if(ARMBOTTOMLIM==SWITCHON)
			{
				ARMMOTOR=NEUTRAL;
				checkBottom();
				state=RAISE_ARM;
			}
			break;
		case RAISE_ARM:
			setArmGoal(MEDDROPHEIGHT);
			state=ARM_IDLE;
			break;
		case ARM_IDLE:
			//Transition code here
			if(timer>380)
				state=DROP_FLOATY;
			break;
		case DROP_FLOATY:
			CLAW=CLAWOPEN;
			WRISTUP=WRISTCLOSED;
			WRISTDOWN = !WRISTUP;
			state=DONE_AUTO;
			break;
		case DONE_AUTO: // 5
			break;
	}
	
	if(state > ZERO_ARM)
		updateArm();
	
	User_Byte5 = ARMBOTTOMLIM;
	User_Byte6 = state;

	timer++;
}

void driveStraight(int power){
	drive_code_mecanum(127,power,127);
}
void driveSide(int power){
	drive_code_mecanum(power,127,127);
}
void rotate(int power){
	drive_code_mecanum(127,127,power);
}
