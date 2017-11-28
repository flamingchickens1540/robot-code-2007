/*******************************************************************************
* FILE NAME: catlin_aliases.h
*
* DESCRIPTION:
*  This file contains the catlin alias declarations.
*
* USAGE:
*	NO EDITING SHOULD HAPPEN TO THIS FILE WITHOUT THE
*	CONSENT OF THE SOFTWARE MANAGER OR MENTOR
*
*******************************************************************************/

#ifndef __catlin_aliases_h_
#define __catlin_aliases_h_

/*/////////////////////////////////////// PWM OUT ///////////////////////////////////*/
/* Motors */
#define	RFMOTOR				pwm01		/* Right Front Motor */
#define	LFMOTOR				pwm02		/* Left Front Motor */
#define	RRMOTOR				pwm03		/* Right Rear Motor */
#define	LRMOTOR				pwm04		/* Left Rear Motor */
#define	ARMMOTOR			pwm05

/* Servos */
#define	PAN_SERVO			pwm10		/* Camera Pan */
#define	TILT_SERVO			pwm11		/* Camera Tilt */

/*//////////////////////////////////////////////////////////*/

/*///////////////////////////////////////// OI //////////////////////////////////////*/
/* Drive Control */
#define XAXIS				p4_x		/* Change this to your planar joystick's x-axis */
#define YAXIS				p4_y		/* Change this to your planar joystick's y-axis */
#define	SPIN				p3_x		/* Change this to your spin joystick's spin axis */

/* Arm Control */
#define	CLAWSW				p1_sw_aux2	/* Change this to your claw toggle switch */
#define	WRISTSW				p1_sw_trig	/* Change this to your wrist toggle switch */
/*//////////////////////////////////////////////////////////*/

/*/////////////////////////////////////// Analog ////////////////////////////////////*/
#define	GYROFAST			rc_ana_in10	/* The Gyroscope with the larger range */
#define	GYROFINE			rc_ana_in11	/* The Gyroscope with the more exact range */
/*//////////////////////////////////////////////////////////*/

/*/////////////////////////////////////// Digital ///////////////////////////////////*/
/*///////////////////////// INPUTS //////////////////////*/
/* Arm */
#define	ARMENC1				rc_dig_in01	/* The Arm's Interupt-based Encoder Input */
#define	ARMENC2				rc_dig_in07	/* The Arm's NON-Interupt-based Encoder Input */

#define	ARMTOPLIM			rc_dig_in05	/* The Arm's top limit switch (non-existant?) */
#define	ARMBOTTOMLIM			rc_dig_in06	/* The Arm's bottom limit switch */

/* Pneumatic */
#define	PRESSUREVALVE			rc_dig_in12	/* The Pressure Valve. When Active, needs more air */

/*///////////////////////// OUTPUTS /////////////////////*/
#define	GRIP				rc_dig_out13	/* Main Gripper on Arm's spike relay */
#define	CLAW				rc_dig_out13	/* Main Gripper on Arm's spike relay */
#define	WRIST				rc_dig_out14	/* Arm's Wrist's spike relay */

#define	COMPRESSOR			rc_dig_out16	/* Compressor's spike relay */

/*//////////////////////////////////////////////////////////*/


/*////////////////////////////////////// CONSTANTS //////////////////////////////////*/

/* Arm Stuff */
#define ARMCRUISESPEED			240
#define ARMSLOWSPEED			200
#define ARMSPEEDBUFFER			2000
#define ARMSTOPBUFFER			200

/* Wrist/Claw Stuff */
#define	CLAWOPEN			1
#define	CLAWCLOSED			0
#define	WRISTOPEN			1
#define	WRISTCLOSED			0

/* These are basics that should always have values */
#ifndef FALSE
#define FALSE				0
#endif
#ifndef TRUE
#define TRUE				1
#endif
#define YES				1
#define NO				0
#define	MAX				255
#define	MIN				0
#define	NEUTRAL				127
#define	FULLFORWARD			255
#define	FULLBACK			0
#define STOPPED				127
#define ON				1
#define OFF				0
#define	LEDON				0
#define LEDOFF				1
#define	SWITCHOFF			1
#define	SWITCHON			0
#define CALLSPSEC			38.17
#define	RIGHT				1
#define	LEFT				-1
#define	OPEN				1
#define	CLOSED				0

#define PAN_MIDDLE		65
#define PAN_ANGLE		((((int)PAN_SERVO - PAN_MIDDLE) * 65)/124)
#define TILT_ANGLE		((((int)TILT_SERVO - 144) * 25)/50)
#define PAN_ERROR		((int)T_Packet_Data.mx - Tracking_Config_Data.Pan_Target_Pixel)
#define TILT_ERROR		((int)T_Packet_Data.my - Tracking_Config_Data.Tilt_Target_Pixel)
#define LOCKONERROR      15

#define	pi					3.141592653589
#define	DPSPERNFAST			0.5859375
#define	DPSPERNFINE			0.146484375
#define	SAMPLE_PERIOD_FAST	0.00625 /* aka 160 */ /* /s.0125 /* aka 80 /s */ /*.025 /* aka 40 per second */
#define	SAMPLE_PERIOD_FINE	0.003125 /* aka 320 /s */ /*.00625 /* aka 160 /s */ /*.0125 /* aka 80 per second */
/*/////////////////////////////*/


/*/////////////////////////////////////// LEGACY ////////////////////////////////////*/
#define STARTPOSN1			rc_dig_in12		/* Autonomous Box, Start Posn Selector, Posn 1 */
#define STARTPOSN3			rc_dig_in13		/* Autonomous Box, Start Posn Selector, Posn 3 */
#define AUTOBIT1			rc_dig_in14		/* Autonomous Box, Autonomous Selector, bit 1 */
#define AUTOBIT2			rc_dig_in15		/* Autonomous Box, Autonomous Selector, bit 2 */
#define	AUTOBIT4			rc_dig_in16		/* Autonomous Box, Autonomous Selector, bit 4 */

/* CONSTANTS REPRESENTING OI DEVICES (LEDs, Buzzers, Joysticks, etc.) */
/*TOGGLES*/
#define	MODETOGGLE			p1_sw_aux2
#define	LAUNCHERTOGGLE			p1_sw_aux1
#define	LOADERTOGGLE			p1_sw_top
#define	LOWGOALTOGGLE			p1_sw_trig
#define	TRIM				p1_x
#define BIAS				p1_x
#define	MANUALELEVATION			p1_y
#define	DRIVEDIRECTION			p3_sw_aux2
#define CAMERARESEET			p3_sw_trig
/*Buttons*/
#define	AUTOROTATE			p2_sw_trig
#define	LEARNHIGH			p2_sw_top
#define	LEARNCORRECT			p2_sw_aux1
#define	LEARNLOW			p2_sw_aux2
#define FIRE1				p3_sw_aux1
#define	FIRE2				FALSE /*p4_sw_trig*/
#define	LOADERBARF			p3_sw_top
#define	CAMERARESET			p3_sw_trig

#define	TURBO				p4_sw_trig

#define	MORELEFT			p1_wheel
#define MORERIGHT			p1_aux
/*LEDs*/
#define	READYTOFIRELED			Pwm1_green
#define	BALLREADYLED			Pwm1_red
#define	CAMERALOCKLED			Pwm2_green
/*Other*/
#define	BUZZER				Pwm2_red
#define	DRIVEOVERRIDE			p4_sw_top



#define	HIGHPRESET			p1_x.bitselect.bit0



#endif
