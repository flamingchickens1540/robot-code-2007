/*******************************************************************************
* FILE NAME: catlin_control.h
*
* DESCRIPTION: 
*  This file contains declarations of functions used in Catlin's portion of
*  software for the arm.
*
*******************************************************************************/

#ifndef __catlin_control_h_
#define __catlin_control_h_

extern char MANUALUP;
extern char MANUALDOWN;

void Catlin_Operator_Control(void);
void Catlin_Arm_Control(void);
void Catlin_Claw_Control(void);
void Catlin_OI_Analog_To_Digital(void);

char inBufferZone(int val, int goal, char buffer);
#endif
