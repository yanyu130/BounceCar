#ifndef CONTROL_H
#define CONTROL_H

#include "ConfigTable.h"
#include "stdint.h"
#include "PID.h"

#define ACTION_NONE 0x00
#define ACTION_FORWARD 0x03
#define ACTION_BACKWARD 0x05

//command1
#define ACTION_CLOCK_WISE_LITTLE 0x40
#define ACTION_ANTICLOCK_WISE_BIG 0x02
#define ACTION_MODEL_JUMP_MODE1 0x04
#define ACTION_CLOCK_WISE_BIG 0x08

#define ACTION_MODEL_JUMP_MODE2 0x10
#define ACTION_CLOCK_WISE_BIG_2S 0x20			//大幅度顺时针
#define ACTION_ANTICLOCK_WISE_LITTLE 0x01	//小幅度逆时针
#define ACTION_ROLL_180 							0x80	//翻转180度

//command2
#define MODEL_HAND_UP 	0x01

//占空比
#define BASIC_SPEED0	0
#define BASIC_SPEED1	15
#define BASIC_SPEED2	20
#define BASIC_SPEED3	25
#define BASIC_SPEED4	30

typedef struct _action
{
	uint8_t actionType;
	uint8_t speed;
	uint32_t stopTime;
	float targetAngle;
}ACTION;

extern uint8_t handup_ready;	//准备好站立

extern int8_t DeadZone;

extern float Thro, Roll, Pitch, Yaw;
//extern float g_fSpeedControlIntegral;
enum{ROLL,PITCH,YAW,THROTTLE};

extern int8_t CarMode;
enum{HAND_STAND,NORMAL};

extern uint8_t Action;
//enum{STOP,FORWARD,BACKWARD,CLOCK_WISE_LITTLE,
//		ANTICLOCK_WISE_LITTLE,CLOCK_WISE_BIG,ANTICLOCK_WISE_BIG};

extern PID_Typedef pitch_angle_PID;	  //pitch角度环PID
extern PID_Typedef pitch_rate_PID;		//pitch角速率环PID

extern PID_Typedef speed_angle_PID;    //speed角度环PID
extern PID_Typedef roll_rate_PID;     //roll角速率环PID

extern PID_Typedef yaw_angle_PID;     //yaw角度环PID
extern PID_Typedef yaw_rate_PID;      //yaw角速率环PID  

void ActionHandle(uint8_t action,int8_t speed);

void CtrlAttiAng(void);
void CtrlAttiRate(void); 
void CtrlMotor(void);
void SetTargetAngle(void);
//void SetHandstandMode(void);
//void SetNormalMode(void);
void SetActionUsingTime(uint8_t action,int32_t time,int8_t speed);
void SetActionUsingAngle(uint8_t action,float angle,int8_t speed);
//void DoActionUsingTime(void);	//time(ms)
void DoActionLoop(uint8_t CarMode);
void ActionHandle2(uint8_t action, int8_t speed);
void MotorPower(int16_t leftSpeed, int16_t rightSpeed);
void SetBasicSpeed(int8_t Speed);
void CtrlAttiSpeed(void);
float limitValue(float value,float min, float max);
#endif
