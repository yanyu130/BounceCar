#ifndef DMP_H
#define DMP_H

#include "stdint.h"

#define   ONE_G  9.80665f //related to acc calculate 

//#define DMP_CALC_PRD 7	//ms
#define DMP_ACC_SCALE 8192.0f	//4G , 31276/4=8192
//#define DMP_GYRO_SCALE 16.4f	//2000deg/s , 31276/2000=16.4f 

struct DMP_FIFO_map{
int16_t qw;		 // DMP输出的四元数值
int16_t null0;
int16_t qx;
int16_t null1;
int16_t qy;
int16_t null2;
int16_t qz;
int16_t null3;
int16_t GYROx;	// 陀螺仪 X轴 角速度 ADC值
int16_t null4;
int16_t GYROy;  // 陀螺仪 Y轴 角速度 ADC值
int16_t null5;
int16_t GYROz;	// 陀螺仪 Z轴 角速度 ADC值
int16_t null6;
int16_t ACCx;   // 加速度计 X轴 ADC值
int16_t null7;
int16_t ACCy;	  // 加速度计 Y轴 ADC值
int16_t null8;
int16_t ACCz;	  // 加速度计 Z轴 ADC值
int16_t null9;
int16_t null10;

//以下数据由 DMP_Routing 更新。
float  dmp_pitch;  //DMP算出来的俯仰角	单位：度
float  dmp_roll;    //DMP滚转角		   单位：度
float  dmp_yaw;		//DMP 航向角，由于没有磁力计参与，航向角会飘  单位：度
float  dmp_gyrox;	// 陀螺仪 X轴 角速度   单位：度每秒
float  dmp_gyroy;   // 陀螺仪 Y轴 角速度   单位：度每秒
float  dmp_gyroz;   // 陀螺仪 Z轴 角速度   单位：度每秒
float  dmp_accx;	// 加速度计 X轴   单位：g  [9.8 m/S^2]
float  dmp_accy;	// 加速度计 Y轴   单位：g  [9.8 m/S^2]
float  dmp_accz;	// 加速度计 Z轴   单位：g  [9.8 m/S^2]
};

//初始化DMP引擎
uint8_t MPU6050_DMP_Initialize(void);

void DMP_Routing(void);
 
#endif
