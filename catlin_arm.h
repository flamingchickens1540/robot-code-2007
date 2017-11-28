/*******************************************************************************
* FILE NAME: catlin_arm.h
*
* DESCRIPTION: 
*  This file contains declarations of functions used in Catlin's portion of
*  software for the arm.
*
*******************************************************************************/

#ifndef __catlin_arm_h_
#define __catlin_arm_h_

void setArmGoal(int newGoal);
void updateArm(void);
//void updateClaw(void);
void checkBottom(void);

extern int armGoal;
extern int buttonMode;
extern char holdStill;

#endif
