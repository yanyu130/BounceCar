#include "PID.h"
#include "def.h"
#include "UartCtrl.h"
#include "stdlib.h"

PID_Typedef pitch_angle_PID;	//pitch角度环的PID
PID_Typedef pitch_rate_PID;		//pitch角速率环的PID

PID_Typedef roll_angle_PID;   //roll角度环的PID
PID_Typedef roll_rate_PID;    //roll角速率环的PID

PID_Typedef yaw_angle_PID;    //yaw角度环的PID 
PID_Typedef yaw_rate_PID;     //yaw角速率环的PID

void GetRollAnglePID(float* pid)
{
	pid[0] = roll_angle_PID.P;
	pid[1] = roll_angle_PID.I;
	pid[2] = roll_angle_PID.D;
}

void GetYawAnglePID(float* pid)
{
	pid[0] = yaw_angle_PID.P;
	pid[1] = yaw_angle_PID.I;
	pid[2] = yaw_angle_PID.D;
}

void GetPitchAnglePID(float* pid)
{
	pid[0] = pitch_angle_PID.P;
	pid[1] = pitch_angle_PID.I;
	pid[2] = pitch_angle_PID.D;
}

void GetRollRatePID(float* pid)
{
	pid[0] = roll_rate_PID.P;
	pid[1] = roll_rate_PID.I;
	pid[2] = roll_rate_PID.D;
}

void GetYawRatePID(float* pid)
{
	pid[0] = yaw_rate_PID.P;
	pid[1] = yaw_rate_PID.I;
	pid[2] = yaw_rate_PID.D;
}

void GetPitchRatePID(float* pid)
{
	pid[0] = pitch_rate_PID.P;
	pid[1] = pitch_rate_PID.I;
	pid[2] = pitch_rate_PID.D;
}
	
//-----------位置式PID-----------
void PID_Postion_Cal(PID_Typedef * PID,float target,float measure,int32_t dertT)
{
 float termI=0;
 float dt= dertT/1000.0;
	//-----------位置式PID-----------
	//误差=期望值-测量值
	PID->Error=target-measure;
	
	PID->Deriv= (PID->Error-PID->PreError)/dt;
	
	PID->Output=(PID->P * PID->Error) + (PID->I * PID->Integ) + (PID->D * PID->Deriv);    //PID:比例环节+积分环节+微分环节
	
	PID->PreError=PID->Error;
	//仅用于角度环和角速度环的

	//if(FLY_ENABLE && offLandFlag)
	{
			//if(fabs(PID->Output) < Thro )		              //比油门还大时不积分
			{
				termI=(PID->Integ) + (PID->Error) * dt;     //积分环节
				if(termI > - PID->iLimit && termI < PID->iLimit && PID->Output > - PID->iLimit && PID->Output < PID->iLimit)       //在-30~30时才进行积分环节
					PID->Integ=termI;
			}
	}
//	else
//			PID->Integ= 0;
}

float GetPIDfloat()
{
	char value_s[4];
	
	value_s[0] = GetUartChar();
	value_s[1] = GetUartChar();
	value_s[2] = GetUartChar();
	value_s[3] = GetUartChar();
	
	return atof(value_s);
}

void pidSetPID(PID_Typedef* pid, float kp, float ki, float kd)
{
  pid->P = kp;
  pid->I = ki;
  pid->D = kd;
}

void controllerSetRollPID(float kp, float ki, float kd)
{
	if(kp>=0)
		pidSetPID(&roll_angle_PID, kp, roll_angle_PID.I, roll_angle_PID.D);
	if(ki>=0)
		pidSetPID(&roll_angle_PID, roll_angle_PID.P, ki, roll_angle_PID.D);
	if(kd>=0)
		pidSetPID(&roll_angle_PID, roll_angle_PID.P, roll_angle_PID.I, kd);
}
void controllerSetPitchPID(float kp, float ki, float kd)
{
	if(kp>=0)
		pidSetPID(&pitch_angle_PID, kp, pitch_angle_PID.I, pitch_angle_PID.D);
	if(ki>=0)
		pidSetPID(&pitch_angle_PID, pitch_angle_PID.P, ki, pitch_angle_PID.D);
	if(kd>=0)
		pidSetPID(&pitch_angle_PID, pitch_angle_PID.P, pitch_angle_PID.I, kd);
}
void controllerSetYawPID(float kp, float ki, float kd)
{
	if(kp>=0)
		pidSetPID(&yaw_angle_PID, kp, yaw_angle_PID.I, yaw_angle_PID.D);
	if(ki>=0)
		pidSetPID(&yaw_angle_PID, yaw_angle_PID.P, ki, yaw_angle_PID.D);
	if(kd>=0)
		pidSetPID(&yaw_angle_PID, yaw_angle_PID.P, yaw_angle_PID.I, kd);
}

void controllerSetRollRatePID(float kp, float ki, float kd)
{
	if(kp>=0)
		pidSetPID(&roll_rate_PID, kp, roll_rate_PID.I, roll_rate_PID.D);
	if(ki>=0)
		pidSetPID(&roll_rate_PID, roll_rate_PID.P, ki, roll_rate_PID.D);
	if(kd>=0)
		pidSetPID(&roll_rate_PID, roll_rate_PID.P, roll_rate_PID.I, kd);
}
void controllerSetPitchRatePID(float kp, float ki, float kd)
{
	if(kp>=0)
		pidSetPID(&pitch_rate_PID, kp, pitch_rate_PID.I, pitch_rate_PID.D);
	if(ki>=0)
		pidSetPID(&pitch_rate_PID, pitch_rate_PID.P, ki, pitch_rate_PID.D);
	if(kd>=0)
		pidSetPID(&pitch_rate_PID, pitch_rate_PID.P, pitch_rate_PID.I, kd);
}
void controllerSetYawRatePID(float kp, float ki, float kd)
{
	if(kp>=0)
		pidSetPID(&yaw_rate_PID, kp, yaw_rate_PID.I, yaw_rate_PID.D);
	if(ki>=0)
		pidSetPID(&yaw_rate_PID, yaw_rate_PID.P, ki, yaw_rate_PID.D);
	if(kd>=0)
		pidSetPID(&yaw_rate_PID, yaw_rate_PID.P, yaw_rate_PID.I, kd);
}

void SetPID()
{
	char type = GetUartChar();
	//int value;
	char rpy, pid;
	float valuef;
	if(type=='p') { //PID
		rpy = GetUartChar();
		if(rpy=='r') { //Roll
			pid = GetUartChar();
			valuef = GetPIDfloat();
			if(pid=='p')
				controllerSetRollPID(valuef, -1, -1);
			else if(pid=='i') 
				controllerSetRollPID(-1, valuef, -1);
			else if(pid=='d') 
				controllerSetRollPID(-1, -1, valuef);
		}
		else if(rpy=='p') { //Pitch
			pid = GetUartChar();
			valuef = GetPIDfloat();
			if(pid=='p') 
				controllerSetPitchPID(valuef, -1, -1);
			else if(pid=='i') 
				controllerSetPitchPID(-1, valuef, -1);
			else if(pid=='d') 
				controllerSetPitchPID(-1, -1, valuef);
		}
		else if(rpy=='y') { //Yaw
			pid = GetUartChar();
			valuef = GetPIDfloat();
			if(pid=='p') 
				controllerSetYawPID(valuef, -1, -1);
			else if(pid=='i') 
				controllerSetYawPID(-1, valuef, -1);
			else if(pid=='d') 
				controllerSetYawPID(-1, -1, valuef);
		}
	}
	else if(type=='r') { //Rate PID
		rpy = GetUartChar();
		if(rpy=='r') { //Roll
			pid = GetUartChar();
			valuef = GetPIDfloat();
			if(pid=='p') 
				controllerSetRollRatePID((float)valuef, -1, -1);
			else if(pid=='i') 
				controllerSetRollRatePID(-1, (float)valuef, -1);
			else if(pid=='d') 
				controllerSetRollRatePID(-1, -1, (float)valuef);
		}
		else if(rpy=='p') { //Pitch
			pid = GetUartChar();
			valuef = GetPIDfloat();
			if(pid=='p') 
				controllerSetPitchRatePID((float)valuef, -1, -1);
			else if(pid=='i') 
				controllerSetPitchRatePID(-1, (float)valuef, -1);
			else if(pid=='d') 
				controllerSetPitchRatePID(-1, -1, (float)valuef);
		}
		else if(rpy=='y') { //Yaw
			pid = GetUartChar();
			valuef = GetPIDfloat();
			if(pid=='p') 
				controllerSetYawRatePID((float)valuef, -1, -1);
			else if(pid=='i') 
				controllerSetYawRatePID(-1, (float)valuef, -1);
			else if(pid=='d') 
				controllerSetYawRatePID(-1, -1, (float)valuef);
		}
	}
//	else if(type=='s') { //'s'tore flash PID
//		UpdateFlashPID(false);
//	}
//	else if(type=='l') { //'l'oad flash PID
//		LoadFlashPID();
//	}
}
