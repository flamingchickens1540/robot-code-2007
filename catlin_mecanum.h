/*******************************************************************************
* FILE NAME: catlin_mecanum.h
*
* DESCRIPTION: 
*  This file contains declarations of functions used in Catlin's portion of
*  software for the mecanum drive code.
*
*******************************************************************************/

#ifndef __catlin_mecanum_h_
#define __catlin_mecanum_h_

void drive_code_mecanum(int XAX,int YAX,int SPI);
void run_mecanum_weights(void);
void Catlin_Common_First(void);
void Catlin_Forward_Drive(int YAX);
void Catlin_Side_Drive(int XAX);
void Catlin_Motor_Weights();
void Catlin_Rotate_Drive(int SPI);
void Catlin_Linear_Drive(int XAX,int YAX);
int abs(int a);
int max4(int a,int b,int c,int d);
int max2(int a,int b);
int min2(int a , int b);
double scale(int a, float c);
int min(int a, int b);
int inBounds(int a);
void Catlin_Weights(int XAX,int YAX,int SPI);
#endif
