/*******************************************************************************
* FILE NAME: catlin_mecanum.c
*
* DESCRIPTION:
*  This file contains the catlin declarations for mecanum drive code.
*
* USAGE:
*	Any real editing should be done in this file!
*
*******************************************************************************/

#include "catlin_mecanum.h"
#include "catlin_aliases.h"
#include "ifi_aliases.h"
#include "ifi_default.h"
#include "ifi_utilities.h"
#include "math.h"

//used to weight different wheels if they perform differently in real life

//Holds the temporary motor values for moving in linear directions
int RFMOTORTS;
int LFMOTORTS;
int RRMOTORTS;
int LRMOTORTS;

int RFMOTORTF;
int LFMOTORTF;
int RRMOTORTF;
int LRMOTORTF;

//Holds the temporary motor values for rotating
int RFMOTORTROT;
int LFMOTORTROT;
int RRMOTORTROT;
int LRMOTORTROT;

void drive_code_mecanum(int XAX,int YAX,int SPI) {
	int spinI = SPI;
	int xaxisI = XAX;
	int yaxisI = YAX;
	double rotateWeight;
	double sideWeight;
	double forwardWeight;
	xaxisI-=127;
	spinI -=127;
	yaxisI-=127;
	rotateWeight = ((double)abs(spinI)/127.0);
	sideWeight = ((double)abs(xaxisI)/127.0);	
	forwardWeight = ((double)abs(yaxisI)/127.0); 
	//Catlin_Linear_Drive(XAX,YAX);
	Catlin_Rotate_Drive(SPI); 
	Catlin_Side_Drive(XAX);
	Catlin_Forward_Drive(YAX);

	if(rotateWeight+sideWeight+forwardWeight > 0.1 && (abs(xaxisI) > 10 || abs(yaxisI) > 10 || abs(spinI) > 10)){
		RFMOTOR = (int)(scale(RFMOTORTF,forwardWeight)+scale(RFMOTORTS,sideWeight)+scale(RFMOTORTROT,rotateWeight)-381.0)/(rotateWeight+sideWeight+forwardWeight)+127;
		LFMOTOR = (int)(scale(LFMOTORTF,forwardWeight)+scale(LFMOTORTS,sideWeight)+scale(LFMOTORTROT,rotateWeight)-381.0)/(rotateWeight+sideWeight+forwardWeight)+127;
		RRMOTOR = (int)(scale(RRMOTORTF,forwardWeight)+scale(RRMOTORTS,sideWeight)+scale(RRMOTORTROT,rotateWeight)-381.0)/(rotateWeight+sideWeight+forwardWeight)+127;
		LRMOTOR = (int)(scale(LRMOTORTF,forwardWeight)+scale(LRMOTORTS,sideWeight)+scale(LRMOTORTROT,rotateWeight)-381.0)/(rotateWeight+sideWeight+forwardWeight)+127;
	}else{
		RFMOTOR = 127;
		LFMOTOR = 127;
		RRMOTOR = 127;
		LRMOTOR = 127;
	}
	Catlin_Weights(XAX,YAX,SPI);
}
void Catlin_Weights(int XAX,int YAX,int SPI){
	char inMiddle = abs(((int)XAX)-127) <= 10 && abs(((int)SPI)-127) <= 10;

	if(YAX > 175 && inMiddle){
		//left side faster
		LFMOTOR = inBounds(scale(LFMOTOR,1.1));
		LRMOTOR = inBounds(scale(LRMOTOR,1.1));	
	}else if(YAX < 87 && inMiddle){
		//right side faster
		RFMOTOR = inBounds(scale(RFMOTOR,1.1));
		RRMOTOR = inBounds(scale(RRMOTOR,1.1));
	}else if(YAX > 137 && inMiddle){
		//left side a lot faster
		LFMOTOR = inBounds(scale(LFMOTOR,1.45));
		LRMOTOR = inBounds(scale(LRMOTOR,1.45));	
	}else if(YAX < 117 && inMiddle){
		//right side a lot faster
		RFMOTOR = inBounds(scale(RFMOTOR,1.35));
		RRMOTOR = inBounds(scale(RRMOTOR,1.35));	
	}
	inMiddle = abs(((int)XAX)-127) <= 10 && abs(((int)YAX)-127) <= 10;
	if(SPI < 170 && SPI > 137&&inMiddle){
		//left side a lot faster
		LFMOTOR = inBounds(scale(LFMOTOR,1.35));
		LRMOTOR = inBounds(scale(LRMOTOR,1.35));
	}else if(SPIN> 84 && SPI < 117&&inMiddle){		
		//right side a lot faster
		RFMOTOR = inBounds(scale(RFMOTOR,1.3));
		RRMOTOR = inBounds(scale(RRMOTOR,1.3));	
	}
//	User_Byte1 = 2;
	inMiddle = (abs(((int)SPI)-127) <= 10) && (abs(((int)YAX)-127) <= 10);
	if(XAX < 117 && inMiddle && XAX >54){
		LRMOTOR = inBounds(scale(LRMOTOR,1.2));
		RRMOTOR = inBounds(scale(RRMOTOR,1.2));
		LFMOTOR = inBounds(scale(LFMOTOR,0.8));
		RFMOTOR = inBounds(scale(RFMOTOR,0.8));
	//	User_Byte1 = 3;
	}else if(XAX > 137 && XAX < 200 && inMiddle){
		LFMOTOR = inBounds(scale(LFMOTOR,1.2));
		RFMOTOR = inBounds(scale(RFMOTOR,1.35));
		LRMOTOR = inBounds(scale(LRMOTOR,1.1));
		RRMOTOR = inBounds(scale(RRMOTOR,1.1));
	//	User_Byte1 = 4;
	}
//	User_Byte2 = XAX;
//	User_Byte3 = inMiddle;
}
int inBounds(int a){
	if(a < 0) return 0;
	if(a > 255) return 255;
	return a;
}
int min(int a, int b){
	if(a<b) return a;
	else return b;
}
float scale(int a, float c){
	return (a-127)*c+127.0;
}
int abs(int a){
	if(a < 0) return -1*a;
	else return a;
}
int max4(int a,int b,int c,int d){
	return (max2(a,b),max2(c,d));
}
int max2(int a, int b){
	if(a > b) return a;
	else return b;
}
void Catlin_Rotate_Drive(int SPI){
	if(abs(SPI-127)>10){
		RFMOTORTROT = 255-SPI;
		LFMOTORTROT = SPI;
		RRMOTORTROT = 255-SPI;
		LRMOTORTROT = SPI;
	}else{
		RFMOTORTROT = 127;
		LFMOTORTROT = 127;
		RRMOTORTROT = 127;
		LRMOTORTROT = 127;
	}
}
void Catlin_Forward_Drive(int YAX){
	if(abs(YAX-127)>10){
		RFMOTORTF = YAX;
		LFMOTORTF = YAX;
		RRMOTORTF = YAX;
		LRMOTORTF = YAX;
	}else{
		RFMOTORTF = 127;
		LFMOTORTF = 127;
		RRMOTORTF = 127;
		LRMOTORTF = 127;
	}
}
void Catlin_Side_Drive(int XAX){
	if(abs(XAX-127)>10){
		RFMOTORTS = 255-XAX;
		LFMOTORTS = XAX;
		RRMOTORTS = XAX;
		LRMOTORTS = 255-XAX;
	}else{
		RFMOTORTS = 127;
		LFMOTORTS = 127;
		RRMOTORTS = 127;
		LRMOTORTS = 127;
	}
}
/*void Catlin_Linear_Drive(int XAX,int YAX){
//based on x and y axis it sets the global variables
	//for each motor with a TEMP at the end
	int RF,RB,LF,LB;
	float maxLengthInDir;
	float actualLength;
	float scale;
	int yaxisI = (int)YAX;
	int xaxisI = (int)XAX;
	int tempY = yaxisI-127;
	int tempX = xaxisI-127;
	actualLength = sqrt(tempX*tempX+tempY*tempY);
	if(abs(tempY) > abs(tempX)){
		tempX = tempX*127/tempY;
		tempY = 127;
		maxLengthInDir = sqrt(tempY*tempY+tempX*tempX);
	}else if(abs(tempY) < abs(tempX)){
		tempY = tempY*127/tempX;
		tempX = 127;
		maxLengthInDir = sqrt(tempY*tempY+tempX*tempX);
	}else{
		maxLengthInDir = sqrt(127*127+127*127);
	}
	
	RF = (yaxisI+255-xaxisI)/2-127;
	RB = (yaxisI+xaxisI)/2-127;
	LF = (yaxisI+xaxisI)/2-127;
	LB = (yaxisI+255-xaxisI)/2-127;
	
	scale = (127.0f/max4(abs(RF),abs(RB),abs(LF),abs(LB)))*(actualLength/maxLengthInDir);
	
	RF = RF*scale;
	RB = RB*scale;
	LF = LF*scale;
	LB = LB*scale;	
	RF += 127;
	RB += 127;
	LF += 127;
	LB += 127;
	RFMOTORTLIN = RF;
	LFMOTORTLIN = LF;
	RRMOTORTLIN = RB;
	LRMOTORTLIN = LB;
//	User_Byte1 = RFMOTORTLIN;
//	User_Byte2 = RRMOTORTLIN;
//	User_Byte3 = LFMOTORTLIN;
//	User_Byte4 = LRMOTORTLIN;
}*/

