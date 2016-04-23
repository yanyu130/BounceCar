#ifndef CONTROL_H
#define CONTROL_H

#include "ConfigTable.h"
#include "stdint.h"
#include "PID.h"

extern float Thro, Roll, Pitch, Yaw;
enum{PITCH,ROLL,YAW,THROTTLE};

void CtrlAttiAng(void);
void CtrlAttiRate(void); 
void CtrlMotor(void);
void SetThro(void);

#endif
