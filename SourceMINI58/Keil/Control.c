#include "Control.h"
#include "def.h"
#include "Timer_Ctrl.h"
#include "RC.h"
#include "motor.h"
#include "IMU.h"
#include "UartCtrl.h"
#include "stdlib.h"

int16_t DEFAULT_ANGLE = 65;

float fStabilityPError;
float fStabilityDError;

float g_fSpeedControlIntegral = 0;

//float g_Speed = 0;

float BasicSpeed=45, Roll=0, Pitch=0, Yaw=0;
int16_t Motor[4]={0};   //定义电机PWM数组，分别对应M1-M4

int8_t DeadZone = 0;
int8_t TurnRound = 0;

int8_t Action = FORWARD;
int8_t CarMode = NORMAL;

int16_t TargetAngle = 79;// = DEFAULT_ANGLE;

ACTION currentAction;

//IMU
typedef struct float_xyz
{
    float X;
    float Y;
    float Z;
    
}S_FLOAT_XYZ;

S_FLOAT_XYZ DIF_ACC;		//实际去期望相差的加速度


//控制平衡站立PID算法
float stabilityPDControl(float inputAngle, float setAnglePoint, float inputAngleSpeed, float setAngleSpeedPoint, float Kp, float Kd)
{
  float errorAngle;
  float errorAngleSpeed;
  
  float output;

  errorAngle = inputAngle - setAnglePoint;
  errorAngleSpeed = inputAngleSpeed - setAngleSpeedPoint;

  fStabilityPError = Kp*errorAngle;
  fStabilityDError = Kd*errorAngleSpeed;
  output = Kp*errorAngle + Kd*errorAngleSpeed;//(Kd*(setPoint - setPointOld) - Kd*(input - PID_errorOld2))/DT;       // + error - PID_error_Old2

  return(output);
}

void CtrlAttiAng(void)
{
		Pitch = stabilityPDControl(imu.pitch, TargetAngle, -imu.gyro[PITCH],0,
										pitch_angle_PID.P,pitch_rate_PID.P);
}

//速度控制函数
//float SpeedPIControl(int8_t Angle, int8_t nTargetAngle)
//{
//  float fDelta;
//  float fP, fI;
//  float output;
//  
//  
//  fDelta = nTargetAngle - Angle;    //nTargetSpeed是目标速度
//  fP = fDelta * speed_angle_PID.P;  
//  fI = fDelta * speed_angle_PID.I;
//  
//  g_fSpeedControlIntegral += fI;
//	if(g_fSpeedControlIntegral > speed_angle_PID.iLimit) 
//	{
//		g_fSpeedControlIntegral = speed_angle_PID.iLimit;
//	}
//	if(g_fSpeedControlIntegral < -speed_angle_PID.iLimit) 
//	{
//		g_fSpeedControlIntegral = -speed_angle_PID.iLimit;
//	}
//  
//  output =  g_fSpeedControlIntegral + fP;
//  

//  return output;
//}

//void CtrlAttiSpeed(void)
//{
//	g_Speed = SpeedPIControl(imu.pitch, TargetAngle);
//}



//void CtrlAttiRate(void)
//{
//	float yawRateTarget=0;
//	static uint32_t tPrev=0; 

//	float dt=0,t=0;
//	t=getSystemTime();
//	dt=(tPrev>0)?(t-tPrev):0;
//	tPrev=t;
//		
//		yawRateTarget=0;

//		//PID_Postion_Cal(&pitch_rate_PID,pitch_angle_PID.Output,-imu.gyro[PITCH]*180.0f/M_PI_F,dt);	
//		PID_Postion_Cal(&pitch_rate_PID,pitch_angle_PID.Output,-imu.gyro[PITCH]*180.0f/M_PI_F,dt);	
//		//PID_Postion_Cal(&roll_rate_PID,roll_angle_PID.Output,imu.gyro[ROLL]*180.0f/M_PI_F,dt);//gyroxGloble
//		PID_Postion_Cal(&yaw_rate_PID,yawRateTarget,-imu.gyro[YAW]*180.0f/M_PI_F,dt);//DMP_DATA.GYROz

//		Pitch = pitch_rate_PID.Output;
//    //Roll  = roll_rate_PID.Output;
//    Yaw   = yaw_rate_PID.Output;
//}

//void CtrlAttiYaw(void)
//{
//	float yawRateTarget=0;
//	static uint32_t tPrev=0; 

//	float dt=0,t=0;
//	t=getSystemTime();
//	dt=(tPrev>0)?(t-tPrev):0;
//	tPrev=t;
//		
//		yawRateTarget = 0;

//		//PID_Postion_Cal(&pitch_rate_PID,pitch_angle_PID.Output,-imu.gyro[PITCH]*180.0f/M_PI_F,dt);	
//		//PID_Postion_Cal(&roll_rate_PID,roll_angle_PID.Output,imu.gyro[ROLL]*180.0f/M_PI_F,dt);//gyroxGloble
//		PID_Postion_Cal(&yaw_rate_PID,yawRateTarget,-imu.gyro[YAW]*180.0f/M_PI_F,dt);//DMP_DATA.GYROz

//		//Pitch = pitch_rate_PID.Output;
//    //Roll  = roll_rate_PID.Output;
//    Yaw   = yaw_rate_PID.Output;
//}

void TurnForward(int16_t leftPWM, int16_t rightPWM)
{
	MotorPwmOutput(0,leftPWM,0,rightPWM);
}


void TurnBackward(int16_t leftPWM, int16_t rightPWM)
{
	MotorPwmOutput(leftPWM,0,rightPWM,0);
}

void MotorPower(int16_t leftSpeed, int16_t rightSpeed)
{
		if(leftSpeed > 0 && rightSpeed > 0)
		{
			TurnForward(leftSpeed,rightSpeed);		//forward
			//MotorPwmOutput(0,leftPWM,0,rightPWM);
		}
		else if(leftSpeed <= 0 && rightSpeed <= 0)
		{
			TurnBackward(-leftSpeed,-rightSpeed);		//backward
			//MotorPwmOutput(leftPWM,0,rightPWM,0);
		}
		else if(leftSpeed > 0 && rightSpeed <= 0)
		{
			MotorPwmOutput(0,leftSpeed,-rightSpeed,0);		
		}
		else if(leftSpeed <= 0 && rightSpeed > 0)
		{
			MotorPwmOutput(-leftSpeed,0,0,rightSpeed);		
		}
}

void CtrlMotor(void)
{
	int16_t leftSpeed,rightSpeed;

	if(CarMode == HAND_STAND)
	{
		//Pitch = Pitch - g_Speed;
		if(Pitch > 0)
		{
			leftSpeed = Pitch + DeadZone;
			rightSpeed = Pitch + DeadZone;
		}
		else
		{
			leftSpeed = Pitch - DeadZone;
			rightSpeed = Pitch - DeadZone; 
		}
		//转弯控制
		leftSpeed -=   TurnRound;
		rightSpeed +=   TurnRound;
		
		MotorPower(leftSpeed, rightSpeed);
	}
	
}



int GetIntValue()
{
	char value_s[3];
	
	value_s[0] = GetUartChar();
	value_s[1] = GetUartChar();
	value_s[2] = GetUartChar();
	//value_s[3] = GetUartChar();
	
	return atoi(value_s);
}

void SetTargetAngle()
{
	uint8_t value;
	
	value = GetIntValue();
	DEFAULT_ANGLE = value;
	
}

void SetHandstandMode()
{
		TargetAngle = DEFAULT_ANGLE;
}

void SetNormalMode()
{
		TargetAngle = -37;
}

void SetActionUsingTime(int8_t action,int32_t time,int8_t speed)
{
	currentAction.actionType = action;
	currentAction.stopTime = getSystemTime() + time;
	currentAction.speed = speed;
}

void DoActionUsingTime()	//time(ms)
{
	if(getSystemTime() <= currentAction.stopTime)
	{
		ActionHandle(currentAction.actionType, currentAction.speed);
	}
	else
	{
			MotorPwmOutput(0,0,0,0);
	}
}

void DoActionUsingTime2()	//time(ms)
{
	if(getSystemTime() <= currentAction.stopTime)
	{
		ActionHandle2(currentAction.actionType, currentAction.speed);
	}
	else
	{
			TurnRound = 0;
			TargetAngle = DEFAULT_ANGLE;
	}
}


void ActionHandle2(int8_t action,int8_t speed)
{
	switch(action)
	{
//		case STOP:
//		{
//			MotorPwmOutput(0,0,0,0);
//		}
		case FORWARD:
		{
			MotorPower(speed,speed);
		}
		break;
		case BACKWARD:
		{
			MotorPower(-speed,-speed);
		}
		break;
		case CLOCK_WISE_LITTLE:	//顺时针
		{
			TurnRound = 40;
			//MotorPower(speed,-speed);
		}
		break;
		case ANTICLOCK_WISE_LITTLE:	//逆时针
		{
			TurnRound = -40;
			//MotorPower(-speed,speed);
		}
		case CLOCK_WISE_BIG:	//顺时针
		{
			TurnRound = 60;
			//MotorPower(speed,-speed);
		}
		break;
		case ANTICLOCK_WISE_BIG:	//逆时针
		{
			TurnRound = -60;
			//MotorPower(-speed,speed);
		}
		break;
	}
}

void ActionHandle(int8_t action,int8_t speed)
{
	switch(action)
	{
//		case STOP:
//		{
//			MotorPwmOutput(0,0,0,0);
//		}
		case FORWARD:
		{
			MotorPower(speed,speed);
		}
		break;
		case BACKWARD:
		{
			MotorPower(-speed,-speed);
		}
		break;
		case CLOCK_WISE_LITTLE:	//顺时针
		{
			MotorPower(speed,-speed);
		}
		break;
		case ANTICLOCK_WISE_LITTLE:	//逆时针
		{
			MotorPower(-speed,speed);
		}
		case CLOCK_WISE_BIG:	//顺时针
		{
			MotorPower(speed,-speed);
		}
		break;
		case ANTICLOCK_WISE_BIG:	//逆时针
		{
			MotorPower(-speed,speed);
		}
		break;
	}
}
