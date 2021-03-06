/*******************************************************************************
*
*	TITLE:		terminal.c 
*
*	VERSION:	0.1 (Beta)                           
*
*	DATE:		25-Nov-2005
*
*	AUTHOR:		R. Kevin Watson
*				kevinw@jpl.nasa.gov
*
*	COMMENTS:	You are free to use this source code for any non-commercial
*				use. Please do not make copies of this source code, modified
*				or un-modified, publicly available on the internet or elsewhere
*				without permission. Thanks.
*
*				Copyright �2005-2007 R. Kevin Watson. All rights are reserved.
*
********************************************************************************
*
*	CHANGE LOG:
*
*	DATE         REV  DESCRIPTION
*	-----------  ---  ----------------------------------------------------------
*	25-Nov-2005  0.1  RKW - Original code.
*	19-Feb-2006  0.2  RKW - Added "no camera data" diagnostic information.
*
*******************************************************************************/
#include <stdio.h>
#include "ifi_aliases.h"
#include "ifi_default.h"
#include "camera.h"
#include "tracking.h"
#include "catlin_aliases.h"

/*******************************************************************************
*
*	FUNCTION:		Tracking_Info_Terminal()
*
*	PURPOSE:		This function is designed to send tracking information
*					to a terminal attached to your robot controller's 
*					programming port.
*
*	CALLED FROM:	user_routines.c/Process_Data_From_Master_uP()
*
*	PARAMETERS:		none
*
*	RETURNS:		nothing
*
*	COMMENTS:		This code assumes that the terminal serial port is 
*					properly set in camera.h
*
*******************************************************************************/
void Tracking_Info_Terminal(void)
{
	static unsigned char i = 0;
	static unsigned char j = 0;
	static unsigned int old_camera_t_packets = 0;

	// has a new camera tracking packet arrived since we last checked?
	if(camera_t_packets != old_camera_t_packets)
	{
		// increment the t-packet counter
		i++;

		// reset the "no camera data" loop counter
		j = 0;

		// only show data on every twenty t-packets
		if(i >= 20)
		{
			// reset the local t-packet counter
			i = 0;

			// does the camera have a tracking solution?
			if(Get_Tracking_State() == SEARCHING)
			{
				printf("Searching...\r\n");
			}
			else
			{
				printf("\r\n");

				// pan angle = ((current pan PWM) - (pan center PWM)) * degrees/pan PWM step
				printf(" Pan Servo = %d\r\n", PAN_SERVO);

				// tilt angle = ((current tilt PWM) - (tilt center PWM)) * degrees/tilt PWM step
				printf("Tilt Servo = %d\r\n", TILT_SERVO);

				printf(" Pan Error (Pixels)  = %d\r\n", (int)T_Packet_Data.mx - Tracking_Config_Data.Pan_Target_Pixel);
				printf("Tilt Error (Pixels)  = %d\r\n", (int)T_Packet_Data.my - Tracking_Config_Data.Tilt_Target_Pixel);
				printf(" Blob Size (Pixels)  = %u\r\n", (unsigned int)T_Packet_Data.pixels);
				printf("Confidence (Pixels)  = %u\r\n", (unsigned int)T_Packet_Data.confidence);
			}
		}
	}
	else
	{
		j++;

		// display a "no camera data" warning if we haven't received
		// a t-packet in 26 loops, which is about 1 second
		if(j >= 26)
		{
			// reset the "no camera data" loop counter
			j = 0;

			printf("No camera data...\r\n");
		}
	}
}
