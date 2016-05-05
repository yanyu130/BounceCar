#include "FailSafe.h"
#include "IMU.h"
#include "math.h"
#include "motor.h"
#include "control.h"
#include "RC.h"


#define FAIL_SAFE_ANGLE_LOW -20
#define FAIL_SAFE_ANGLE_HIGH 180
void FailSafeCrash(void)
{
		if(CarMode == HAND_STAND) 
		{
			if(imu.pitch >= FAIL_SAFE_ANGLE_HIGH || imu.pitch <= FAIL_SAFE_ANGLE_LOW )
			{
				CarMode = NORMAL;
			} 
		}
}

void FailSafeLostRC(void)
{
	if(getRC_Status() == STATUS_LOST)
	{
		Motor_Stop();
	}
}
