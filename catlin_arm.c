/*******************************************************************************
* FILE NAME: catlin_arm.c
*
* DESCRIPTION:
*  This file contains the catlin arm declarations.
*
* USAGE:
*	Any real editing should be done in this file!
*
*******************************************************************************/

#include "catlin_arm.h"
#include "catlin_aliases.h"
#include "catlin_control.h"
#include "interrupts.h"
#include "ifi_aliases.h"
#include "ifi_default.h"
#include "ifi_utilities.h"

int armGoal;
int buttonMode;
char holdStill;
void setArmGoal(int newGoal){
	armGoal=newGoal;
}

/*void updateClaw(void){
	if(CLAWSW == SWITCHON) CLAW=CLAWOPEN;
	else if(CLAWSW == SWITCHOFF) CLAW=CLAWCLOSED;
	
//	if(WRISTSW == SWITCHON) WRIST=WRISTOPEN;
//	else if(WRISTSW == SWITCHOFF) WRIST=WRISTCLOSED;
}*/

void checkBottom(void){	//depressed switch returns 0; otherwise 1
	
	if(ARMBOTTOMLIM == SWITCHON){
		holdStill = YES;
		clickRotationsArm = 0;
		if(ARMMOTOR < 140){
			ARMMOTOR = 127;
			armGoal = 0;
		}
	}
	
	if(clickRotationsArm <= 80 && ARMMOTOR < 110) {
		ARMMOTOR = 110;
	}
	if( (clickRotationsArm <= 320) &&
		(clickRotationsArm >= 50) &&
		(WRISTUP) &&
		(ARMMOTOR < 120)) { // WRIST down
		BUZZER = LEDON;
	
	}
	
}

void updateArm(void){
	
   /* To make this code exceedingly clear to anyone who should read it:
	* The logic behind this is that the lower the goal is, the higher up
	* the lift it is. Likewise, the higher above 127 the
	* motor value is, the faster up it goes, and vice versa. */
	
	if(!MANUALUP && !MANUALDOWN) {
	
		if(clickRotationsArm>(armGoal+ARMSPEEDBUFFER)) ARMMOTOR=100; //far above the goal; move down fast
		else if(clickRotationsArm>(armGoal+ARMSTOPBUFFER)) ARMMOTOR=110; //just above the goal; move down slowly
		else if(clickRotationsArm>(armGoal-ARMSTOPBUFFER)) ARMMOTOR=133; //within stop buffer zone; stop here
		else if(clickRotationsArm<(armGoal-ARMSTOPBUFFER)) ARMMOTOR=170; //just below goal; move up slowly
		else if(clickRotationsArm<(armGoal-ARMSPEEDBUFFER)) ARMMOTOR=230; //far below the goal; move up fast
	
	}
	if(buttonMode == ON)ARMMOTOR = 133;
	
	checkBottom(); //should the bottom limit get tripped, everything grinds to a halt
}
