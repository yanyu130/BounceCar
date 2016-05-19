#include "Control.h"
#include "def.h"
#include "Timer_Ctrl.h"
#include "RC.h"
#include "motor.h"
#include "IMU.h"
#include "UartCtrl.h"
#include "stdlib.h"

PID_Typedef pitch_angle_PID;	//pitch角度环的PID
PID_Typedef pitch_rate_PID;		//pitch角速率环的PID

PID_Typedef roll_angle_PID;   //speed角度环的PID
PID_Typedef roll_rate_PID;    //roll角速率环的PID

PID_Typedef yaw_angle_PID;    //yaw角度环的PID 
PID_Typedef yaw_rate_PID;     //yaw角速率环的PID

float DEFAULT_PITCH_ANGLE   = 63.5;
float TargetPitchAngle = 63.5;// = DEFAULT_ANGLE;
float TargetYawAngle = 0;

float fStabilityPError;
float fStabilityDError;

float g_fSpeedControlIntegral = 0;
float g_fSpeedControl = 0;

float g_fCarSpeed = 0;

int8_t BasicSpeed=0;

float Roll=0, Pitch=0, Yaw=0;
int16_t Motor[4]={0};   //定义电机PWM数组，分别对应M1-M4

int8_t DeadZone = 0;
int8_t TurnRound = 0;

uint8_t Action = ACTION_FORWARD;
int8_t CarMode = NORMAL;

uint8_t handup_ready = false;

ACTION currentAction;

//IMU
typedef struct float_xyz
{
    float X;
    float Y;
    float Z;
    
}S_FLOAT_XYZ;

S_FLOAT_XYZ DIF_ACC;		//实际去期望相差的加速度

float limitValue(float value,float min, float max)
{
	float output = value;
	if(value > max)	output = max;
	else if(value <min)	output = min;
	return output;
}

//控制平衡站立PD算法
float stabilityPDControl(float inputAngle, float setAnglePoint, float inputAngleSpeed, float setAngleSpeedPoint, PID_Typedef * PID)
{
  float errorAngle;
  float errorAngleSpeed;
  
  float output;

  errorAngle = inputAngle - setAnglePoint;
  errorAngleSpeed = inputAngleSpeed - setAngleSpeedPoint;

  fStabilityPError = PID->P * errorAngle;
  fStabilityDError = PID->D * errorAngleSpeed;
	
  output = PID->P*errorAngle + PID->D*errorAngleSpeed;

  return(output);
}
//控制平衡站立PID算法
float PIDControl(float ActualAngle, float TargetAngle, PID_Typedef * PID)
{
  float error;
  float output;

  error = TargetAngle - ActualAngle;
	PID->Integ += error;
  output = PID->P*error + PID->I*PID->Integ + PID->D*(error-PID->PreError);
	PID->PreError = error;
  return(output);
}

void CtrlAttiAng(void)
{ 
		//Pitch = stabilityPDControl(imu.pitch, TargetAngle, -imu.gyro[PITCH],0,
		//								&pitch_angle_PID);
	Pitch = -PIDControl(imu.pitch, TargetPitchAngle,&pitch_angle_PID);
		
//	static uint32_t tPrev=0; 

//	float dt=0,t=0;
//	t=getSystemTime();
//	dt=(tPrev>0)?(t-tPrev):0;
//	tPrev=t;

//	PID_Postion_Cal(&pitch_angle_PID,TargetAngle,imu.pitch,dt);	
//	Pitch = pitch_angle_PID.Output;
}

void CtrlStraightLineAng(float TargetAngle)
{ 
	Yaw = -PIDControl(imu.yaw, TargetAngle,&yaw_angle_PID);
	printf("TargetAngle = %d\n",(int)TargetAngle);
}


//速度控制函数
//float SpeedPIControl(float Angle, float nTargetAngle, PID_Typedef * PID)
//{
//  float fDelta;
//  float fP, fI;
//  float output;
//  
//  
//  fDelta = Angle - nTargetAngle;    //nTargetSpeed是目标速度
//  fP = fDelta * PID->P;  
//  fI = fDelta * PID->I;
//	
//	PID->Deriv = fDelta - PID->PreError;
//	PID->PreError = fDelta;
//  
//  g_fSpeedControlIntegral += fI;
//	//printf("SpeedPIControl = %f\n",g_fSpeedControlIntegral);
//	if(g_fSpeedControlIntegral > 60) 
//	{
//		g_fSpeedControlIntegral = 60;
//	}
//	if(g_fSpeedControlIntegral < -60) 
//	{
//		g_fSpeedControlIntegral = -60;
//	}
//	//printf("SpeedPIControl = %f\n",g_fSpeedControlIntegral);
//  //output =  g_fSpeedControlIntegral;
//  output =  g_fSpeedControlIntegral + fP + PID->D * PID->Deriv;
//  return output;
//}


//速度控制
float SpeedPIControl(float inputAngle, float nTargetAngle, PID_Typedef * PID,float dt)
{
  float fDelta;
  float fP, fI;
  float output;
  
  g_fCarSpeed += (inputAngle - nTargetAngle) / dt;
	g_fCarSpeed = limitValue(g_fCarSpeed,-100,100);
  	printf("g_fCarSpeed = %f\n",g_fCarSpeed);
  //g_fCarSpeed *= CAR_SPEED_CONSTANT;  //折算成有现实意义的速度
  
  fDelta = (g_fCarSpeed - 0);    //nTargetSpeed是目标速度
  fP = fDelta * PID->P;  
  fI = fDelta * PID->I;
  
  g_fSpeedControlIntegral += fI;

  g_fSpeedControlIntegral = limitValue(g_fSpeedControlIntegral,-255,255);
	
	fP = limitValue(fP,-255,255);
	
  output =  g_fSpeedControlIntegral + fP;
	
	output = limitValue(output,-255,255);	
  
  return output;
}

void CtrlAttiSpeed(void)
{
	float output;
	static uint32_t tPrev=0; 

	float dt=0,t=0;
	t=getSystemTime();
	dt=(tPrev>0)?(t-tPrev):0;
	tPrev=t;
	
	if(handup_ready)
	{
		output = SpeedPIControl(imu.pitch, TargetPitchAngle, &pitch_rate_PID,dt);
		Pitch += output;
		printf("SpeedPIControl = %f\n",output);
	}
	else
	{
		g_fSpeedControlIntegral = 0;
		g_fSpeedControl = 0;
		g_fCarSpeed = 0;
	}
	
}



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
		}
		else if(leftSpeed <= 0 && rightSpeed <= 0)
		{
			TurnBackward(-leftSpeed,-rightSpeed);		//backward
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
	float value;
	
	value = GetPIDfloat();
	DEFAULT_PITCH_ANGLE = TargetPitchAngle = value;
	
}

//void SetHandstandMode()
//{
//		TargetAngle = DEFAULT_ANGLE;
//}

//void SetNormalMode()
//{
//		TargetAngle = -37;
//}

void SetActionUsingTime(uint8_t action,int32_t time,int8_t speed)
{
	currentAction.actionType = action;
	currentAction.stopTime = getSystemTime() + time;
	currentAction.speed = speed;
}

void SetActionUsingAngle(uint8_t action,float angle,int8_t speed)
{
	currentAction.actionType = action;
	currentAction.speed = speed;
	currentAction.targetAngle = angle;
//	if(currentAction.actionType == ACTION_FORWARD || 
//			currentAction.actionType == ACTION_BACKWARD)
//	{
//		TargetYawAngle = angle;
//	}
		
}

void DoActionLoop(uint8_t CarMode)	//单位：ms
{
	int8_t leftSpeed = 0, rightSpeed = 0;
	//有动作时
	if(getSystemTime() <= currentAction.stopTime)	//时间结束时
	{
		if(CarMode == HAND_STAND)	//倒立模式下
		{
			ActionHandle2(currentAction.actionType, currentAction.speed);
		}
		else
		{
			ActionHandle(currentAction.actionType, currentAction.speed);
		}
	}
	else if(currentAction.actionType ==  ACTION_ROLL_180)	//翻滚180度
	{
		//车的俯仰角度大于90时
		if(currentAction.targetAngle > 90)
		{
			if(imu.pitch < currentAction.targetAngle-30)	//未达到目标角度时，-30是惯性处理
			{
				ActionHandle(currentAction.actionType, currentAction.speed);
				//printf("pitch = %d\n",(int)imu.pitch);
			}
			else
			{
				currentAction.actionType = ACTION_NONE;
				//printf("finish\n");
			}
		}
		else
		{
			if(imu.pitch > currentAction.targetAngle+30)	//未达到目标角度时，+30是惯性处理
			{
				ActionHandle(currentAction.actionType, -currentAction.speed);
				//printf("pitch = %d\n",(int)imu.pitch);
			}
			else
			{
				currentAction.actionType = ACTION_NONE;
				//printf("finish\n");
			}
		}
		
	}
	else if(currentAction.actionType ==  ACTION_FORWARD)	//直线走
	{	
		if(currentAction.speed != BASIC_SPEED0)
		{
			CtrlStraightLineAng(currentAction.targetAngle);
			leftSpeed = currentAction.speed - Yaw;
			rightSpeed = currentAction.speed + Yaw;
			//printf("YAW = %d\n",(int)Yaw);
			MotorPower(leftSpeed, rightSpeed);
		}
	}
	else if(currentAction.actionType ==  ACTION_BACKWARD)	//直线走
	{	
		if(currentAction.speed != BASIC_SPEED0)
		{
			CtrlStraightLineAng(currentAction.targetAngle);
			leftSpeed = currentAction.speed - Yaw;
			rightSpeed = currentAction.speed + Yaw;
			//printf("YAW = %d\n",(int)Yaw);
			MotorPower(leftSpeed, rightSpeed);
			
		}
	}
	else	//无动作时
	{	
		if(CarMode == HAND_STAND)
		{
			TurnRound = 0;
			TargetPitchAngle = DEFAULT_PITCH_ANGLE;
			
			//小车的俯仰角进入目标区间，直立算法做积分
			if((imu.pitch >= (TargetPitchAngle-10)) && (imu.pitch <= (TargetPitchAngle+10)) )
			{
				handup_ready = true;
				//printf("handup_ready = true \n");
			}
			else if((imu.pitch >= 180) || (imu.pitch <= 0))
			{
				handup_ready = false;
				//printf("handup_ready = false \n");
			}
		}
		else
		{
			handup_ready = false;
			
			MotorPower(BasicSpeed, BasicSpeed);
			
			
			//MotorPwmOutput(0,0,0,0);
			//currentAction.actionType = ACTION_NONE;
		}
	}
}



void ActionHandle2(uint8_t action,int8_t speed)
{
	switch(action)
	{
//		case STOP:
//		{
//			MotorPwmOutput(0,0,0,0);
//		}
		case ACTION_FORWARD:
		{
			MotorPower(speed,speed);
		}
		break;
		case ACTION_BACKWARD:
		{
			MotorPower(-speed,-speed);
		}
		break;
		case ACTION_CLOCK_WISE_LITTLE:	//顺时针
		{
			TurnRound = 40;
			//MotorPower(speed,-speed);
		}
		break;
		case ACTION_ANTICLOCK_WISE_LITTLE:	//逆时针
		{
			TurnRound = -40;
			//MotorPower(-speed,speed);
		}
		case ACTION_CLOCK_WISE_BIG:	//顺时针
		{
			TurnRound = 60;
			//MotorPower(speed,-speed);
		}
		break;
		case ACTION_ANTICLOCK_WISE_BIG:	//逆时针
		{
			TurnRound = -60;
			//MotorPower(-speed,speed);
		}
		break;
	}
}

void ActionHandle(uint8_t action,int8_t speed)
{
	switch(action)
	{
//		case STOP:
//		{
//			MotorPwmOutput(0,0,0,0);
//		}
		case ACTION_FORWARD:
		{
			MotorPower(speed,speed);
		}
		break;
		case ACTION_BACKWARD:
		{
			MotorPower(-speed,-speed);
		}
		break;
		case ACTION_CLOCK_WISE_LITTLE:	//顺时针
		{
			MotorPower(speed,-speed);
		}
		break;
		case ACTION_ANTICLOCK_WISE_LITTLE:	//逆时针
		{
			MotorPower(-speed,speed);
		}
		case ACTION_CLOCK_WISE_BIG:	//顺时针
		{
			MotorPower(speed,-speed);
		}
		break;
		case ACTION_ANTICLOCK_WISE_BIG:	//逆时针
		{
			MotorPower(-speed,speed);
		}
		break;
		case ACTION_ROLL_180:	//翻滚180
		{
			MotorPower(-speed,-speed);
		}
		break;
	}
}

void SetBasicSpeed(int8_t speed)
{
	BasicSpeed = speed;
}
