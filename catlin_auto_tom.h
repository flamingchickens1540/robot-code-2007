/*******************************************************************************
* FILE NAME: catlin_auto.h
*
* DESCRIPTION:
*  This file contains the catlin autonomous declarations.
*
*******************************************************************************/

#ifndef __catlin_auto_tom_h_
#define __catlin_auto_tom_h_

#define	TRACKING_MIN_PAN			Tracking_Config_Data.Pan_Min_PWM
#define	TRACKING_MAX_PAN			Tracking_Config_Data.Pan_Max_PWM
#define	TRACKING_CENTER_PAN			Tracking_Config_Data.Pan_Center_PWM
#define	TRACKING_MIN_TILT			Tracking_Config_Data.Tilt_Min_PWM
#define	TRACKING_MAX_TILT			Tracking_Config_Data.Tilt_Max_PWM
#define	TRACKING_CENTER_TILT		Tracking_Config_Data.Tilt_Center_PWM
#define	TRACKING_STEP_TILT		Tracking_Config_Data.Tilt_Search_Step_Size
#define	TRACKING_STEP_PAN			Tracking_Config_Data.Pan_Search_Step_Size

void run_auto(void);
void driveStraight(int power);
void driveSide(int power);
void rotate(int power);
void autoDrive(void);

void autoInitialize(void);
int preventWrapAround(int x);
void autoArm(void);
void deployKeeper(void);
int getDesiredArmHeight(void);
void setAutoMode(void);
void rotateRightConstant(void);
void rotateLeftConstant(void);
void rotateRightSlowConstant(void);
void rotateLeftSlowConstant(void);
int max(int a, int b);

void restrictCameraPan(int center,int buffer);
void restrictCameraTilt(int center, int buffer);
void setCameraPanStep(unsigned char step);
void setCameraTiltStep(unsigned char step);
void resetCameraPan(void);
void resetCameraTilt(void);
void resetCameraView(void);


#endif
