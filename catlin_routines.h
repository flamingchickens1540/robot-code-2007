/*******************************************************************************
* FILE NAME: catlin_routines.h
*
* DESCRIPTION: 
*  This file contains declarations of functions used in Catlin's portion of
*  software.
*
*******************************************************************************/

#ifndef __catlin_routines_h_
#define __catlin_routines_h_


/* Function Declarations */
void Catlin_Initialize(void);
void Catlin_Autonomous(void);
void Catlin_Main_Routine(void);
void Catlin_Common_First(void);
void declareTable(void);

void updatePneumatics(void);
void tomKiller(void);

#endif
