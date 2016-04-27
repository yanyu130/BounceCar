#ifndef CONTROL_H
#define CONTROL_H

#include "ConfigTable.h"
#include "stdint.h"
#include "PID.h"

extern float Thro, Roll, Pitch, Yaw;
enum{PITCH,ROLL,YAW,THROTTLE};

extern int8_t mode;
enum{FORWARD,BACKWORD,CLOCK_WISE,ANTICLOCK_WISE};

void CtrlAttiAng(void);
void CtrlAttiRate(void); 
void CtrlMotor(void);
void SetThro(void);
void CtrlAttiYaw(void);

#endif
