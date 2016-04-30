#ifndef CONTROL_H
#define CONTROL_H

#include "ConfigTable.h"
#include "stdint.h"
#include "PID.h"

extern int8_t DeadZone;

extern float Thro, Roll, Pitch, Yaw;
//extern float g_fSpeedControlIntegral;
enum{ROLL,PITCH,YAW,THROTTLE};

extern int8_t CarMode;
enum{HAND_STAND,NORMAL};

extern int8_t Action;
enum{FORWARD,BACKWORD,CLOCK_WISE,ANTICLOCK_WISE};

void ActionHandle(int8_t action,int8_t speed);

void CtrlAttiAng(void);
void CtrlAttiRate(void); 
void CtrlMotor(void);
void SetTargetAngle(void);
void SetHandstandMode(void);
void SetNormalMode(void);

#endif
