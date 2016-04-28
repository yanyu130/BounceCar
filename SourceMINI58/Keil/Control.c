#include "Control.h"
#include "def.h"
#include "Timer_Ctrl.h"
#include "RC.h"
#include "motor.h"
#include "IMU.h"
#include "UartCtrl.h"
#include "stdlib.h"


float fStabilityPError;
float fStabilityDError;

float BasicSpeed=45, Roll=0, Pitch=0, Yaw=0;
int16_t Motor[4]={0};   //定义电机PWM数组，分别对应M1-M4

int8_t DeadZone = 0;

int8_t mode = FORWARD;

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

  errorAngle = setAnglePoint-inputAngle;
  errorAngleSpeed = setAngleSpeedPoint - inputAngleSpeed;

  // Kd is implemented in two parts
  //    The biggest one using only the input (sensor) part not the SetPoint input-input(t-2)
  //    And the second using the setpoint to make it a bit more agressive   setPoint-setPoint(t-1)
  fStabilityPError = Kp*errorAngle;
  fStabilityDError = Kd*errorAngleSpeed;
  output = Kp*errorAngle + Kd*errorAngleSpeed;//(Kd*(setPoint - setPointOld) - Kd*(input - PID_errorOld2))/DT;       // + error - PID_error_Old2
  //PID_errorOld2 = PID_errorOld;
  //PID_errorOld = input;  // error for Kd is only the input component
  //setPointOld = setPoint;
  //output = constrain(output,-255,255);
  return(output);
}

void CtrlAttiAng(void)
{
		static uint32_t tPrev=0;
		float angTarget[3]={0};
		float dt=0,t=0;
		t = getSystemTime();
		dt=(tPrev>0)?(t-tPrev):0;
		tPrev=t;
		
		//angTarget[ROLL]=(float)(RC_DATA.ROOL);
		angTarget[PITCH] = 90;
		
 
		PID_Postion_Cal(&pitch_angle_PID,angTarget[PITCH],imu.pitch,dt);
		//PID_Postion_Cal(&roll_angle_PID,angTarget[ROLL],imu.roll,dt);	 
		//Pitch = pitch_angle_PID.Output;
}

void CtrlAttiAng2(void)
{
		
 
		Pitch = -stabilityPDControl(imu.pitch, 70, -imu.gyro[ROLL],0,
										pitch_angle_PID.P,pitch_rate_PID.P);
	
		//Pitch = pitch_angle_PID.Output;
		//float stabilityPDControl(float inputAngle, float setAnglePoint, 
			//float inputAngleSpeed, float setAngleSpeedPoint, float Kp, float Kd)

}

void CtrlAttiRate(void)
{
	float yawRateTarget=0;
	static uint32_t tPrev=0; 

	float dt=0,t=0;
	t=getSystemTime();
	dt=(tPrev>0)?(t-tPrev):0;
	tPrev=t;
		
		yawRateTarget=0;

		//PID_Postion_Cal(&pitch_rate_PID,pitch_angle_PID.Output,-imu.gyro[PITCH]*180.0f/M_PI_F,dt);	
		PID_Postion_Cal(&pitch_rate_PID,pitch_angle_PID.Output,-imu.gyro[PITCH]*180.0f/M_PI_F,dt);	
		//PID_Postion_Cal(&roll_rate_PID,roll_angle_PID.Output,imu.gyro[ROLL]*180.0f/M_PI_F,dt);//gyroxGloble
		PID_Postion_Cal(&yaw_rate_PID,yawRateTarget,-imu.gyro[YAW]*180.0f/M_PI_F,dt);//DMP_DATA.GYROz

		Pitch = pitch_rate_PID.Output;
    //Roll  = roll_rate_PID.Output;
    Yaw   = yaw_rate_PID.Output;
}

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

void CtrlMotor(void)
{
	int leftSpeed,rightSpeed;
	
	//将输出值融合到四个电机 
//		Motor[0] = (int16_t)(BasicSpeed - Yaw );     
//		Motor[2] = (int16_t)(BasicSpeed + Yaw ); 

//		Motor[1] = (int16_t)(BasicSpeed + Yaw );   	
//		Motor[3] = (int16_t)(BasicSpeed - Yaw ); 

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
	
		if(leftSpeed > 0)
		{
			MotorPwmOutput(leftSpeed,0,rightSpeed,0);		//forward
		}
		else
		{
			MotorPwmOutput(0,-leftSpeed,0,-rightSpeed);		//backward
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

void SetThro()
{
	//char type = GetUartChar();
	uint8_t value;
	//if(type=='t') 
		{
			value = GetIntValue();
			BasicSpeed = value;
	}
}
