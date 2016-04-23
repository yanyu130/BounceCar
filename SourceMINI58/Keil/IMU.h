#ifndef IMU_H
#define IMU_H

#include "stdint.h"

#define M_PI_F 3.1415926

//校准时间
#define ACC_CALC_TIME  3000//ms
#define GYRO_CALC_TIME   3000	//ms

#define CONSTANTS_ONE_G					9.80665f		/* m/s^2		*/


typedef struct IMU_tt
{
uint8_t caliPass;
uint8_t ready;
int16_t accADC[3];
int16_t gyroADC[3];
int16_t magADC[3];
float 	accRaw[3];		//m/s^2
float 	gyroRaw[3];		//rad/s 
float 	magRaw[3];		//
float   accOffset[3];		//m/s^2
float   gyroOffset[3]; 
float   accb[3];		//filted, in body frame
float   accg[3];
float   gyro[3];
float   DCMgb[3][3];
float   q[4];
float   roll;				//deg
float   pitch;
float 	yaw;
float   rollRad;				//rad
float   pitchRad;
float 	yawRad;
}imu_t;

extern imu_t imu;
extern uint8_t imuCaliFlag;

void IMU_Init(void);
void ReadIMUSensorHandle(void);
uint8_t IMU_Calibrate(void);

 
#endif
