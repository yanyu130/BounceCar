#include "Report.h"
#include "def.h"
#include "UartCtrl.h"
#include "ConfigTable.h"
#include "stdio.h"
#include "PID.h"
#include "Motor.h"
#include "imu.h"

char report_mode = REPORT_MOTOR_POWER;
float PID[9];

void report_angle_pid(void)
{
	
	GetPitchAnglePID(&PID[0]);
	GetRollAnglePID(&PID[3]);
	GetYawAnglePID(&PID[6]);

	{
		printf("@AnglePID:%1.1f,%1.1f,%1.1f;  %1.1f,%1.1f,%1.1f; %1.1f,%1.1f,%1.1f\n",
			PID[0],PID[1],PID[2],PID[3],PID[4],PID[5],PID[6],PID[7],PID[8]);
	}
}

void report_rate_pid(void)
{
	GetPitchRatePID(&PID[0]);
	GetRollRatePID(&PID[3]);
	GetYawRatePID(&PID[6]);

	{
		printf("@RatePID:%1.4f,%1.4f,%1.4f;  %1.4f,%1.4f,%1.4f; %1.4f,%1.4f,%1.4f\n",
			PID[0],PID[1],PID[2],PID[3],PID[4],PID[5],PID[6],PID[7],PID[8]);
	}
}

void report_moter_power(void)
{
	printf("%d,%d,%d,%d\n",motor1PWM, motor2PWM, motor3PWM, motor4PWM);
}

void report_euler(void)
{
	printf("%d,%d,%d\n", (int)imu.pitch, (int)imu.roll, (int)imu.yaw);
}

void report_sensor_raw(void)
{
	printf("%d,%d,%d,%d,%d,%d\n", (int)imu.accRaw[0], (int)imu.accRaw[1], (int)imu.accRaw[2],
		(int)imu.gyroRaw[0], (int)imu.gyroRaw[1], (int)imu.gyroRaw[2]);
}

void ReportMessage(void)
{
	if(report_mode == REPORT_ANGLE_PID) 
	{
		report_angle_pid();
	}
	else if(report_mode == REPORT_SENSORS_RAW) 
	{
		report_sensor_raw();
	}
	else if(report_mode == REPORT_RATE_PID) 
	{
		report_rate_pid();
	}
	else if(report_mode == REPORT_MOTOR_POWER) 
	{
		report_moter_power();
	}
	else if(report_mode == REPORT_AHRS_EULER) 
	{
		report_euler();
	}
	
}

void CheckVersion()
{
	printf("table.version = %d\n",(int)table.version);
}


