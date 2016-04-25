#include "IMU.h"
#include "def.h"
//#include "mini51series.h"
#include "MPU6050.h"
#include "filter.h"
#include "Timer_Ctrl.h"


#define IMU_SAMPLE_RATE 			100.0f//	1000.0f/(float)DMP_CALC_PRD //
#define IMU_FILTER_CUTOFF_FREQ	30.0f

#define SENSOR_MAX_G 8.0f		//constant g		// tobe fixed to 8g. but IMU need to  correct at the same time
#define SENSOR_MAX_W 2000.0f	//deg/s
#define ACC_SCALE  (SENSOR_MAX_G/32768.0f)
#define GYRO_SCALE  (SENSOR_MAX_W/32768.0f)

imu_t imu={0};
uint8_t imuCaliFlag=1;

void IMU_Init(void)
{
#ifdef IMU_SW		//软件解算前需先校准陀螺
		imu.ready=0;
#else
			imu.ready=1;
#endif
		imu.caliPass=1;
		//filter rate
		LPF2pSetCutoffFreq_1(IMU_SAMPLE_RATE,IMU_FILTER_CUTOFF_FREQ);		//30Hz
		LPF2pSetCutoffFreq_2(IMU_SAMPLE_RATE,IMU_FILTER_CUTOFF_FREQ);
		LPF2pSetCutoffFreq_3(IMU_SAMPLE_RATE,IMU_FILTER_CUTOFF_FREQ);
		LPF2pSetCutoffFreq_4(IMU_SAMPLE_RATE,IMU_FILTER_CUTOFF_FREQ);
		LPF2pSetCutoffFreq_5(IMU_SAMPLE_RATE,IMU_FILTER_CUTOFF_FREQ);
		LPF2pSetCutoffFreq_6(IMU_SAMPLE_RATE,IMU_FILTER_CUTOFF_FREQ);
}

void ReadIMUSensorHandle(void)
{
	uint8_t i;
		//read raw
		
		MPU6050_getAcceleration(&imu.accADC[1], &imu.accADC[0], &imu.accADC[2]);
		MPU6050_getRotation(&imu.gyroADC[1], &imu.gyroADC[0], &imu.gyroADC[2]);
	
		//turn to physical
		for(i=0;i<3;i++)
		{
				imu.accRaw[i]= (float)imu.accADC[i] *ACC_SCALE * CONSTANTS_ONE_G ;
				imu.gyroRaw[i]=(float)imu.gyroADC[i] * GYRO_SCALE * M_PI_F /180.f;		//deg/s
		}

		imu.accb[0]=LPF2pApply_1(imu.accRaw[0]-imu.accOffset[0]);
		imu.accb[1]=LPF2pApply_2(imu.accRaw[1]-imu.accOffset[1]);
		imu.accb[2]=LPF2pApply_3(imu.accRaw[2]-imu.accOffset[2]);
		#ifdef IMU_SW
		imu.gyro[0]=LPF2pApply_4(imu.gyroRaw[0]);
		imu.gyro[1]=LPF2pApply_5(imu.gyroRaw[1]);
		imu.gyro[2]=LPF2pApply_6(imu.gyroRaw[2]); 
		#else
		imu.gyro[0]=LPF2pApply_4(imu.gyroRaw[0]-imu.gyroOffset[0]);
		imu.gyro[1]=LPF2pApply_5(imu.gyroRaw[1]-imu.gyroOffset[1]);
		imu.gyro[2]=LPF2pApply_6(imu.gyroRaw[2]-imu.gyroOffset[2]); 
		#endif
		//low pass filter.  inertial or digital . tobe tested
		/*
		for(i=0;i<2;i++)	//tobe fixed to digital filter
		{
				accFilted[i] = accFilted[i] * (1.0f - (1.0f / ACC_XY_LPF_FACTOR)) + accRaw[i] * (1.0f /ACC_XY_LPF_FACTOR); 
				
				gyroFilted[i] = gyroFilted[i] * (1.0f - (1.0f / GYRO_XY_LPF_FACTOR)) + gyroRaw[i] * (1.0f/GYRO_XY_LPF_FACTOR);
		}
		accFilted[2]=LPF2pApply_3(accRaw[2]);			// need to set cutoff freq and sample rate before 
		gyroFilted[2] = gyroFilted[2] * (1.0f - (1.0f / GYRO_Z_LPF_FACTOR)) + gyroRaw[2] * (1.0f/GYRO_Z_LPF_FACTOR);
		*/
		
} 

//should place to a level surface and keep it stop for 1~2 second
//return 1 when finish
uint8_t IMU_Calibrate(void)
{
	//3s 
	static float accSum[3]={0,0,0};
	static float gyroSum[3]={0,0,0};
	static uint16_t cnt=0;
	static uint16_t tPrev=0,startTime=0;
	static uint8_t calibrating=0;
	uint8_t ret=0;
	uint8_t i=0;
	uint16_t dt=0,now=0,caliTime=0;;


	now=getSystemTime();


#if (1)
		dt=now-tPrev;	

	if(calibrating==0)
	{
			calibrating=1;
			for(i=0;i<3;i++)
			{
					accSum[i]=0;
					gyroSum[i]=0;
					cnt=0;
					imu.ready=0;
			}
			
	}
	if(dt>=10)		//10ms 
	{
			if(cnt<300)
			{
				for(i=0;i<3;i++)
				{
					accSum[i]+=imu.accRaw[i];		
					gyroSum[i]+=imu.gyroRaw[i];
				}
				cnt++;
				tPrev=now;
			}
			else
			{
					for(i=0;i<3;i++)
					{
						imu.accOffset[i]=accSum[i]/(float)cnt;
						imu.gyroOffset[i]=gyroSum[i]/(float)cnt;
					} 
					
					imu.accOffset[2]=imu.accOffset[2] - CONSTANTS_ONE_G;
					
					calibrating=0;
					#ifndef IMU_SW
					imu.ready=1;
					#endif
					ret=1;
					//tobe added: write to eeprom !!
			}
	}
#else
	
	
	if(startTime==0)
	{
		startTime=now;
		imu.ready=0;
	}

	if(now>startTime)
		caliTime=now-startTime;
	else
		caliTime=65536-startTime + now;
	

		for(i=0;i<3;i++)
		{
			accSum[i]+=imu.accRaw[i];			//tobe 
		}
		cnt++;
		if(caliTime > ACC_CALC_TIME)
		{
				for(i=0;i<3;i++)
				{
					imu.accOffset[i]=accSum[i]/cnt;
					accSum[i]=0;
				} 
				imu.accOffset[2]=imu.accOffset[2] - CONSTANTS_ONE_G;
				
				startTime=0;
				cnt=0;
				ret=1;
		}
	
#endif
	
	return ret;
	
}
