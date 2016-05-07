#ifndef SLEEPCTRL_H
#define SLEEPCTRL_H
#include "def.h"

#define WAKE_TIME 2000	//休眠唤醒时间

enum{OPERATION,SLEEP};	//工作模式
extern uint8_t OPERTION_MODE;

extern uint32_t LastPressTime;	//按下按键的时间
extern bool PressIsOn;	//按键是否按下
extern bool PermitTonggleOperation;		//是否允许切换工作模式

void InitSleepIO(void);

void IntoSleep(void);

void PrepareForSleep();

#endif
