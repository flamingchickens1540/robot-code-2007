/*******************************************************************************
* FILE NAME: catlin_gyro.c
*
* DESCRIPTION:
*  This file contains the catlin gyro declarations.
*
* USAGE:
*	Any real editing should be done in this file!
*
*******************************************************************************/

#include "catlin_gyro.h"
#include "catlin_aliases.h"
#include "interrupts.h"
#include "ifi_aliases.h"
#include "ifi_default.h"
#include "ifi_utilities.h"
#include "math.h"

double angleGoal;

unsigned char getSpinRotation(void) {
	
	if(angle > (angleGoal + .05)) {
	//	User_Byte1 = 2;
		if(angle > (angleGoal + 1.5)) {
			return 74;
		}
		else if(angle > (angleGoal+0.2)){
			return 95;
		}else{
			return 104;
		}
	}
	else if(angle < (angleGoal - .05)) {
	//	User_Byte1 = 1;
		if(angle < (angleGoal - 1.5)) {
			return 180;
		}
		else if(angle < (angleGoal-0.2)){
			return 159;
		}else{
			return 150;
		}
	}
	else {
		return NEUTRAL; // Woo hoo!
	//	User_Byte1 = 3;
	}	
	
}

void setAngle(double newAngle) {
	angleGoal = pi*newAngle;
}

void resetGyro(void) {
	angle = 0;
}

void updateGyro() {
	if(angle > (2*pi)) angle = angle - 2*pi;
	else if(angle < 0) angle = 2*pi + angle;
}

void Catlin_Gyro_Bias(void) {
	char pwr;
	int xaxis, yaxis;
	double theta;
	
	if(XAXIS == 255 && YAXIS == 255) pwr = 127;
	else {
		xaxis = XAXIS - 127;
		yaxis = YAXIS - 127;
		pwr = (char) (sqrt(xaxis*xaxis + yaxis*yaxis));
		theta = (angle);
		
		yaxis = (char) pwr*cos(theta);
		xaxis = (char) pwr*sin(theta);
		
		YAXIS = yaxis + 127;
		XAXIS = xaxis + 127;
	}
}
