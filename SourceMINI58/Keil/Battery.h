#ifndef BATTERY_H
#define BATTERY_H
#include "stdint.h"
#include "def.h"

#define AD_REFERENCE 3.3	//参考电压
#define LOW_BATTERY 1.6	//低电压

void BatteryCheckInit(void);
//检查电池电压
void BatteryCheck(void);
//获取电池电压
uint32_t GetBatteryAD(void);
//判断是否低电压
bool isLowBattery(void);
 
#endif
