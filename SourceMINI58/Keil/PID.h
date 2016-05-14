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





//-----------位置式PID-----------
void PID_Postion_Cal(PID_Typedef * PID,float target,float measure,int32_t dertT);
void GetRollAnglePID(float* pid);
void GetPitchAnglePID(float* pid);
void GetYawAnglePID(float* pid);

void GetPitchRatePID(float* pid);
void GetRollRatePID(float* pid);
void GetYawRatePID(float* pid);
void SetPID(void);
float GetPIDfloat(void);
#endif
