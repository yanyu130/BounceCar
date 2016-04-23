#include "FailSafe.h"
#include "IMU.h"
#include "math.h"
#include "motor.h"

#define FAIL_SAFE_ANGLE 90

void FailSafeCrash(void)
{
			 
			if(fabs(imu.pitch) >= FAIL_SAFE_ANGLE || fabs(imu.roll) >= FAIL_SAFE_ANGLE )
			{
			//	RC_DATA.THROTTLE=LAND_THRO;
				Motor_Stop();
				//MotorPwmOutput(0,0,0,0);
				//FLY_ENABLE=0;
			} 
}
