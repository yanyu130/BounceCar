#ifndef CONTROL_H
#define CONTROL_H

#include "ConfigTable.h"
#include "stdint.h"
#include "PID.h"

#define FORWARD 11
#define BACKWARD 16

#define CLOCK_WISE_LITTLE 0x01
#define ANTICLOCK_WISE_BIG 0x02
#define MODEL_JUMP_MODE1 0x04
#define CLOCK_WISE_BIG 0x08

#define MODEL_JUMP_MODE2 0x10
#define CLOCK_WISE_BIG_2S 0x20
#define MODEL_HAND_UP 0x80
//#define CLOCK_WISE_BIG_2S 0x20
#define ANTICLOCK_WISE_LITTLE 0x40






typedef struct _action
{
	uint8_t actionType;
	uint8_t speed;
	uint32_t stopTime;
}ACTION;


extern int8_t DeadZone;

extern float Thro, Roll, Pitch, Yaw;
//extern float g_fSpeedControlIntegral;
enum{ROLL,PITCH,YAW,THROTTLE};

extern int8_t CarMode;
enum{HAND_STAND,NORMAL};

extern int8_t Action;
//enum{STOP,FORWARD,BACKWARD,CLOCK_WISE_LITTLE,
//		ANTICLOCK_WISE_LITTLE,CLOCK_WISE_BIG,ANTICLOCK_WISE_BIG};

void ActionHandle(int8_t action,int8_t speed);

void CtrlAttiAng(void);
void CtrlAttiRate(void); 
void CtrlMotor(void);
void SetTargetAngle(void);
void SetHandstandMode(void);
void SetNormalMode(void);
void SetActionUsingTime(int8_t action,int32_t time,int8_t speed);
void DoActionUsingTime(void);	//time(ms)
void ActionHandle2(int8_t action,int8_t speed);
void DoActionUsingTime2(void);

#endif
