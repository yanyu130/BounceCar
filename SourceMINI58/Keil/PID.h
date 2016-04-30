#ifndef PID_H
#define PID_H
#include "stdint.h"

// PID结构
typedef struct
{
    float P;
    float I;
    float D;
    float Desired;
    float Error;
    float PreError;
    float PrePreError;
    float Increment;
    float Integ;
		float iLimit;
    float Deriv;
    float Output;
 
}PID_Typedef;



extern PID_Typedef pitch_angle_PID;	  //pitch角度环PID
extern PID_Typedef pitch_rate_PID;		//pitch角速率环PID

extern PID_Typedef speed_angle_PID;    //speed角度环PID
extern PID_Typedef roll_rate_PID;     //roll角速率环PID

extern PID_Typedef yaw_angle_PID;     //yaw角度环PID
extern PID_Typedef yaw_rate_PID;      //yaw角速率环PID  

//-----------位置式PID-----------
void PID_Postion_Cal(PID_Typedef * PID,float target,float measure,int32_t dertT);
void GetRollAnglePID(float* pid);
void GetPitchAnglePID(float* pid);
void GetYawAnglePID(float* pid);

void GetPitchRatePID(float* pid);
void GetRollRatePID(float* pid);
void GetYawRatePID(float* pid);
void SetPID(void);
#endif
