/*******************************************************************************
* FILE NAME: catlin_routines.c
*
* DESCRIPTION:
*  This file contains the catlin declarations.
*
* USAGE:
*	Any real editing should be done in this file!
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
#include "catlin_routines.h"
#include "catlin_control.h"
#include "catlin_aliases.h"
#include "catlin_arm.h"
#include "catlin_auto_tom.h"
#include "catlin_mecanum.h"
#include "catlin_gyro.h"

/***** VARIABLE DECLARATION *******/

//unsigned char posn[2][2][2][2][2][2][2][2];

void Catlin_Initialize(void) {
//	setAngle(2.0);
	holdStill = 0;
	buttonMode = OFF;
//	declareTable();
}

void Catlin_Common_First(void) {
	
	static unsigned char count = 0;
	static unsigned char camera_menu_active = 0;
	static unsigned char tracking_menu_active = 0;
	unsigned char terminal_char;
	unsigned char returned_value;
	
	updatePneumatics();
	BUZZER = LEDOFF;
	
	if(count == 10) {
		autoInitialize();
	}
	else count++;
	
	// send diagnostic information to the terminal, but don't 
	// overwrite the camera or tracking menu if it's active
	if(camera_menu_active == 0 && tracking_menu_active == 0)
	{
		Tracking_Info_Terminal();
	}

	// This function is responsable for camera initialization 
	// and camera serial data interpretation. Once the camera
	// is initialized and starts sending tracking data, this 
	// function will continuously update the global T_Packet_Data 
	// structure with the received tracking information.
	Camera_Handler();

	// This function reads data placed in the T_Packet_Data
	// structure by the Camera_Handler() function and if new
	// tracking data is available, attempts to keep the center
	// of the tracked object in the center of the camera's
	// image using two servos that drive a pan/tilt platform.
	// If the camera doesn't have the object within it's field 
	// of view, this function will execute a search algorithm 
	// in an attempt to find the object.
	if(tracking_menu_active == 0)
	{
		Servo_Track();
	}

	// Turn on the "Switch 3" LED on the operator interface if
	// the camera is pointed at the green light target. The
	// Switch3_LED variable is also used by the Default_Routine()
	// function below, so disable it inside Default_Routine()
	// if you want to keep this functionality. 
	if(Get_Tracking_State() == CAMERA_ON_TARGET)
	{
		Switch3_LED = 1;
	}
	else
	{
		Switch3_LED = 0;
	}

	// this logic guarantees that only one of the menus can be
	// active at any giiven time
	if(camera_menu_active == 1)
	{
		// This function manages the camera menu functionality,
		// which is used to enter camera initialization and
		// color tracking parameters.
		camera_menu_active = Camera_Menu();
	}
	else if(tracking_menu_active == 1)
	{
		// This function manages the tracking menu functionality,
		// which is used to enter parameters that describe how
		// the pan and tilt servos will behave while in searching
		// and tracking modes.
		tracking_menu_active = Tracking_Menu();
	}
	else
	{
		// has the user sent any data via the terminal?
		terminal_char = Read_Terminal_Serial_Port();
		// check to see if any "hotkeys" have been pressed
		if(terminal_char == CM_SETUP_KEY)
		{
			camera_menu_active = 1;
		}
		else if(terminal_char == TM_SETUP_KEY)
		{
			tracking_menu_active = 1;
		}
	}

	// This funtion is used by the functions Camera_Menu() and
	// Tracking_Menu() to manage the writing of initialization
	// parameters to your robot controller's non-volatile
	// Electrically Erasable Programmable Read-Only Memory
	// (EEPROM)
	EEPROM_Write_Handler();
}

void Catlin_Autonomous(void) {
	Catlin_Common_First();
	run_auto();
}

void Catlin_Main_Routine(void) {
	int newSpin;
	char sign;
	Catlin_Common_First();
	
	//Catlin_Arm calls
	if(GYROIGNORESW > 200) updateArm(); // e.g. if you have the switch flipped on
	else ARMMOTOR = NEUTRAL;	
	Catlin_Operator_Control();
	
	if(!TRIGGER){
		drive_code_mecanum(255-XAXIS,scale(YAXIS,.5),scale(255-SPIN,.5));
	}else{
		drive_code_mecanum(255-XAXIS,YAXIS,255-SPIN);
	}

	RRMOTOR = 255 - RRMOTOR;	// Apparently these guys need switching
	RFMOTOR = 255 - RFMOTOR;
	ARMMOTOR = 255 - ARMMOTOR;
	//User_Byte1 = clickRotationsArm / 255;
	//User_Byte2 = clickRotationsArm % 255;
	//User_Byte3=clickRotationsArm;
	
	//printf("gyro: %10d   angle: %10d  adjustment: %10d\n\r", gyro, ((int)(angle )),(int)(adjustment*100));
	
/*	User_Byte1 = TRACKING_MIN_PAN;
	User_Byte2 = TRACKING_MAX_PAN;
	User_Byte3 = TRACKING_CENTER_PAN;
	User_Byte4 = TRACKING_MIN_TILT;
	User_Byte5 = TRACKING_MAX_TILT;
	User_Byte6 = TRACKING_CENTER_TILT;*/
	
	if(GYRORESETB) {
		restrictCameraPan(127, 40);
		restrictCameraTilt(127, 40);
		setCameraPanStep(4);
		setCameraTiltStep(4);
	}

	tomKiller();
}

void updatePneumatics(void) {
	COMPRESSOR = !PRESSUREVALVE;
}

void tomKiller(void) {
	if(p2_sw_aux2) RFMOTOR = 127;
	if(p2_sw_trig) LFMOTOR = 127;
	if(p2_sw_aux1) RRMOTOR = 127;
	if(p2_sw_top)  LRMOTOR = 127;
}

void declareTable(void) {
	/*
	posn[1][0][0][0][0][0][0][0] = 0;
	posn[1][1][0][0][0][0][0][0] = 1;
	posn[1][1][0][0][0][0][0][1] = 2;
	posn[1][1][0][0][0][1][0][1] = 3;
	posn[1][1][0][0][0][1][1][1] = 4;
	posn[0][1][0][0][0][1][1][1] = 5;
	posn[0][1][1][0][0][1][1][1] = 6;
	posn[1][1][1][0][0][1][1][1] = 7;
	posn[1][1][1][1][0][1][1][1] = 8;
	posn[1][0][1][1][0][1][1][1] = 9;
	posn[1][0][1][1][0][1][1][0] = 10;
	posn[1][0][1][1][0][0][1][0] = 11;
	posn[1][0][1][1][0][0][0][0] = 12;
	posn[1][1][1][1][0][0][0][0] = 13;
	posn[1][1][0][1][0][0][0][0] = 14;
	posn[0][1][0][1][0][0][0][0] = 15;
	posn[0][1][0][0][0][0][0][0] = 16;
	posn[0][1][1][0][0][0][0][0] = 17;
	posn[1][1][1][0][0][0][0][0] = 18;
	posn[1][1][1][0][0][0][1][0] = 19;
	posn[1][1][1][0][0][0][1][1] = 20;
	posn[1][0][1][0][0][0][1][1] = 21;
	posn[1][0][1][1][0][0][1][1] = 22;
	posn[1][1][1][1][0][0][1][1] = 23;
	posn[1][1][1][1][1][0][1][1] = 24;
	posn[1][1][0][1][1][0][1][1] = 25;
	posn[0][1][0][1][1][0][1][1] = 26;
	posn[0][1][0][1][1][0][0][1] = 27;
	posn[0][1][0][1][1][0][0][0] = 28;
	posn[0][1][1][1][1][0][0][0] = 29;
	posn[0][1][1][0][1][0][0][0] = 30;
	posn[0][0][1][0][1][0][0][0] = 31;
	posn[0][0][1][0][0][0][0][0] = 32;
	posn[0][0][1][1][0][0][0][0] = 33;
	posn[0][1][1][1][0][0][0][0] = 34;
	posn[0][1][1][1][0][0][0][1] = 35;
	posn[1][1][1][1][0][0][0][1] = 36;
	posn[1][1][0][1][0][0][0][1] = 37;
	posn[1][1][0][1][1][0][0][1] = 38;
	posn[1][1][1][1][1][0][0][1] = 39;
	posn[1][1][1][1][1][1][0][1] = 40;
	posn[1][1][1][0][1][1][0][1] = 41;
	posn[1][0][1][0][1][1][0][1] = 42;
	posn[1][0][1][0][1][1][0][0] = 43;
	posn[0][0][1][0][1][1][0][0] = 44;
	posn[0][0][1][1][1][1][0][0] = 45;
	posn[0][0][1][1][0][1][0][0] = 46;
	posn[0][0][0][1][0][1][0][0] = 47;
	posn[0][0][0][1][0][0][0][0] = 48;
	posn[0][0][0][1][1][0][0][0] = 49;
	posn[0][0][1][1][1][0][0][0] = 50;
	posn[1][0][1][1][1][0][0][0] = 51;
	posn[1][1][1][1][1][0][0][0] = 52;
	posn[1][1][1][0][1][0][0][0] = 53;
	posn[1][1][1][0][1][1][0][0] = 54;
	posn[1][1][1][1][1][1][0][0] = 55;
	posn[1][1][1][1][1][1][1][0] = 56;
	posn[1][1][1][1][0][1][1][0] = 57;
	posn[1][1][0][1][0][1][1][0] = 58;
	posn[0][1][0][1][0][1][1][0] = 59;
	posn[0][0][0][1][0][1][1][0] = 60;
	posn[0][0][0][1][1][1][1][0] = 61;
	posn[0][0][0][1][1][0][1][0] = 62;
	posn[0][0][0][0][1][0][1][0] = 63;
	posn[0][0][0][0][1][0][0][0] = 64;
	posn[0][0][0][0][1][1][0][0] = 65;
	posn[0][0][0][1][1][1][0][0] = 66;
	posn[0][1][0][1][1][1][0][0] = 67;
	posn[0][1][1][1][1][1][0][0] = 68;
	posn[0][1][1][1][0][1][0][0] = 69;
	posn[0][1][1][1][0][1][1][0] = 70;
	posn[0][1][1][1][1][1][1][0] = 71;
	posn[0][1][1][1][1][1][1][1] = 72;
	posn[0][1][1][1][1][0][1][1] = 73;
	posn[0][1][1][0][1][0][1][1] = 74;
	posn[0][0][1][0][1][0][1][1] = 75;
	posn[0][0][0][0][1][0][1][1] = 76;
	posn[0][0][0][0][1][1][1][1] = 77;
	posn[0][0][0][0][1][1][0][1] = 78;
	posn[0][0][0][0][0][1][0][1] = 79;
	posn[0][0][0][0][0][1][0][0] = 80;
	posn[0][0][0][0][0][1][1][0] = 81;
	posn[0][0][0][0][1][1][1][0] = 82;
	posn[0][0][1][0][1][1][1][0] = 83;
	posn[0][0][1][1][1][1][1][0] = 84;
	posn[0][0][1][1][1][0][1][0] = 85;
	posn[0][0][1][1][1][0][1][1] = 86;
	posn[0][0][1][1][1][1][1][1] = 87;
	posn[1][0][1][1][1][1][1][1] = 88;
	posn[1][0][1][1][1][1][0][1] = 89;
	posn[1][0][1][1][0][1][0][1] = 90;
	posn[1][0][0][1][0][1][0][1] = 91;
	posn[1][0][0][0][0][1][0][1] = 92;
	posn[1][0][0][0][0][1][1][1] = 93;
	posn[1][0][0][0][0][1][1][0] = 94;
	posn[1][0][0][0][0][0][1][0] = 95;
	posn[0][0][0][0][0][0][1][0] = 96;
	posn[0][0][0][0][0][0][1][1] = 97;
	posn[0][0][0][0][0][1][1][1] = 98;
	posn[0][0][0][1][0][1][1][1] = 99;
	posn[0][0][0][1][1][1][1][1] = 100;
	posn[0][0][0][1][1][1][0][1] = 101;
	posn[1][0][0][1][1][1][0][1] = 102;
	posn[1][0][0][1][1][1][1][1] = 103;
	posn[1][1][0][1][1][1][1][1] = 104;
	posn[1][1][0][1][1][1][1][0] = 105;
	posn[1][1][0][1][1][0][1][0] = 106;
	posn[1][1][0][0][1][0][1][0] = 107;
	posn[1][1][0][0][0][0][1][0] = 108;
	posn[1][1][0][0][0][0][1][1] = 109;
	posn[0][1][0][0][0][0][1][1] = 110;
	posn[0][1][0][0][0][0][0][1] = 111;
	posn[0][0][0][0][0][0][0][1] = 112;
	posn[1][0][0][0][0][0][0][1] = 113;
	posn[1][0][0][0][0][0][1][1] = 114;
	posn[1][0][0][0][1][0][1][1] = 115;
	posn[1][0][0][0][1][1][1][1] = 116;
	posn[1][0][0][0][1][1][1][0] = 117;
	posn[1][1][0][0][1][1][1][0] = 118;
	posn[1][1][0][0][1][1][1][1] = 119;
	posn[1][1][1][0][1][1][1][1] = 120;
	posn[0][1][1][0][1][1][1][1] = 121;
	posn[0][1][1][0][1][1][0][1] = 122;
	posn[0][1][1][0][0][1][0][1] = 123;
	posn[0][1][1][0][0][0][0][1] = 124;
	posn[1][1][1][0][0][0][0][1] = 125;
	posn[1][0][1][0][0][0][0][1] = 126;
	posn[1][0][1][0][0][0][0][0] = 127;
	*/
}
