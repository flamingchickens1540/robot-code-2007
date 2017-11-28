/*******************************************************************************
* FILE NAME: catlin_auto.h
*
* DESCRIPTION:
*  This file contains the catlin autonomous declarations.
*
*******************************************************************************/

#ifndef __catlin_auto_h_
#define __catlin_auto_h_



void run_auto(void);
void driveStraight(int power);
void driveSide(int power);
void rotate(int power);
void goTowardsLight(void);

int preventWrapAround(int x);
void autoArm(void);
void deployKeeper(void);
int getDesiredArmHeight(void);
void setAutoMode(void);
void rotateRightConstant(void);
void rotateLeftConstant(void);

#endif
