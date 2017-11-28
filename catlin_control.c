/*******************************************************************************
* FILE NAME: catlin_control.c
*
* DESCRIPTION:
*  This file contains the catlin control functions.
*
* USAGE:
*	Any editing in how you control the robot should be done in this file!
*
*******************************************************************************/

//Includes to non catlin code
#include "camera.h"
#include "camera_menu.h"
#include "eeprom.h"
#include "ifi_aliases.h"
#include "ifi_default.h"
#include "ifi_utilities.h"
#include "interrupts.h"
#include "pwm.h"
#include "serial_ports.h"
#include <stdio.h>
#include "terminal.h"
#include "tracking.h"
#include "tracking_menu.h"
#include "user_routines.h"

//Includes to catlin code
#include "catlin_control.h"
#include "catlin_routines.h"
#include "catlin_aliases.h"
#include "catlin_arm.h"
#include "catlin_auto.h"
#include "catlin_mecanum.h"
#include "catlin_gyro.h"

char HIGHPRESET, MEDPRESET, LOWPRESET, GNDPRESET, MANUALUP, MANUALDOWN;

void Catlin_Operator_Control(void) {
	Catlin_OI_Analog_To_Digital();

	Catlin_Claw_Control();
	
	Catlin_Arm_Control();
}

void Catlin_Claw_Control(void) {
	CLAW = !CLAWSW;
	WRISTUP = !WRISTSW;
	WRISTDOWN = !WRISTUP;
}

void Catlin_Arm_Control(void) {
	static int count = 0;
	static int armRotationsOld = -1;
	static int armRotationsOlder = -1;
	static char holdStill = 0;
	if(MANUALDOWN) {
		if(!holdStill) ARMMOTOR = 82;
		else ARMMOTOR = 127;
		
		setArmGoal(clickRotationsArm);
		checkBottom();
		buttonMode = ON;
	} else if(MANUALUP) {
		holdStill = NO;
		ARMMOTOR = ARMCRUISESPEED;
		setArmGoal(clickRotationsArm);
		buttonMode = ON;
		count++;
		if(inBufferZone(armRotationsOld, clickRotationsArm, 10) && inBufferZone(clickRotationsArm, armRotationsOlder, 10) && ARMBOTTOMLIM == SWITCHOFF) {
			ARMMOTOR = 140;
			BUZZER = LEDON;
		}
		if(count >= 10) {
			armRotationsOlder = armRotationsOld;
			armRotationsOld = clickRotationsArm;
			count = 0;
		}
	} else if(LIFTDROPTOGGLE) {
		if(GNDPRESET){
			 setArmGoal(GNDLIFTHEIGHT);
			 buttonMode = OFF;
		}
		else if(LOWPRESET){
			 setArmGoal(LOWLIFTHEIGHT);
			 buttonMode = OFF;
		}else if(MEDPRESET){
			 setArmGoal(MEDLIFTHEIGHT);
			 buttonMode = OFF;
		}
		else if(HIGHPRESET){
			 setArmGoal(HIGHLIFTHEIGHT);
			 buttonMode = OFF;
		}
	} else {
		if(GNDPRESET){
			 setArmGoal(GNDDROPHEIGHT);
			 buttonMode = OFF;
		}
		else if(LOWPRESET){
			 setArmGoal(LOWDROPHEIGHT);
			 buttonMode = OFF;
		}else if(MEDPRESET){
			 setArmGoal(MEDDROPHEIGHT);
			 buttonMode = OFF;
		}
		else if(HIGHPRESET){
			 setArmGoal(HIGHDROPHEIGHT);
			 buttonMode = OFF;
		}
	}
}

void Catlin_OI_Analog_To_Digital(void) {
	HIGHPRESET = MEDPRESET = LOWPRESET = GNDPRESET = MANUALUP = MANUALDOWN = 0;
	
	if(inBufferZone(ANALOGHEIGHT, HIGHXVAL, ATDBUFFER)) HIGHPRESET = 1;
	else if(inBufferZone(ANALOGHEIGHT, MEDXVAL, ATDBUFFER)) MEDPRESET = 1;
	else if(inBufferZone(ANALOGHEIGHT, LOWXVAL, ATDBUFFER)) LOWPRESET = 1;
	else if(inBufferZone(ANALOGHEIGHT, GNDXVAL, ATDBUFFER)) GNDPRESET = 1;
	
	//User_Byte1 = MANUALLIFT;
//	User_Byte2 =MANDOWNXVAL;
//	User_Byte3 =ATDBUFFER;
//	User_Byte4 = inBufferZone(MANUALLIFT, MANDOWNXVAL, ATDBUFFER);
	
	if(inBufferZone(MANUALLIFT, MANDOWNXVAL, ATDBUFFER)) MANUALDOWN = 1;
	else if(inBufferZone(MANUALLIFT, MANUPXVAL, ATDBUFFER)) MANUALUP = 1;
}

char inBufferZone(int val, int goal, char buffer) {
	if((val <= (goal + buffer)) &&
	   (val >= (goal - buffer))) return 1;
	else return 0;
}
