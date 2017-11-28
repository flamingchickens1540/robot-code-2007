/*******************************************************************************
* FILE NAME: catlin_gyro.h
*
* DESCRIPTION: 
*  This file contains declarations of functions used in Catlin's portion of
*  software for the arm.
*
*******************************************************************************/

#ifndef __catlin_gyro_h_
#define __catlin_gyro_h_

extern double angleGoal;

unsigned char getSpinRotation(void);
void setAngle(double newAngle);
void resetGyro(void);
void Catlin_Gyro_Bias(void);
void updateGyro();
