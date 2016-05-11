/******************************************************************************
 * @file     main.c
 * @version  
 * $Revision: 
 * $Date: 
 * @brief    
 *
 * @note
 * Copyright (C) 
*****************************************************************************/
#include <stdio.h>


#include "def.h"
#include "ConfigTable.h"
#include "led.h"
#include "Timer_Ctrl.h"
#include "Motor.h"
#include "Battery.h"
#include "UartCtrl.h"
#include "FlashCtrl.h"
#include "RC.h"
#include "Control.h"
#include "I2CDev.h"
#include "MPU6050.h"
#include "IMUSO3.h"
#include "IMU.h"
#include "DMP.h"

#include "FailSafe.h"
#include "Report.h"
#include "SleepCtrl.h"
#include "Audio.h"
#include "jump.h"

void MotorTest(void);



void setupSystemClock(void)
{
		SYS_UnlockReg();
	
		/* Enable HIRC clock */
		CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);

		/* Waiting for HIRC clock ready */
		CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

		/* Switch HCLK clock source to HIRC */
		CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV_HCLK(1));
	
		/* To update the variable SystemCoreClock */
    SystemCoreClockUpdate();
	
		SYS_LockReg();
}



void setup()
{
//	uint8_t i=0;
	bool bMPU6050Status = TRUE;
	
	//初始化系统时钟
	setupSystemClock();
	//初始化System_tick
	setup_system_tick(SYSTEM_TICK_FREQ);
	
	//初始化串口
	setupUART();
	UART_NVIC_INIT();

	//语音
	Audio_Init();

	
	//初始化LED
	LED_Init();
	LED_OFF();
	//初始化IIC
	I2C_Init();
	DelayMsec(300);		//延迟下，再去读传感器，不延迟下，读取传感器会失败
	
	//初始化SENSOR
	#ifdef IMU_SW											//软件姿态解算
		bMPU6050Status = MPU6050_initialize();
		//陀螺仪初始化失败，灯循环闪烁
		if(bMPU6050Status != TRUE)
		{
			while(1) 
			{
				printf("MPU6050 initial faile\n");
				DelayMsec(100);
				LED_ON();
				DelayMsec(100);
				LED_OFF();
			}
		}
	#else
		MPU6050_initialize();
		DelayMsec(1000);			//必须加延迟，否则读取陀螺仪数据出错
		MPU6050_DMP_Initialize();     //初始化DMP引擎
	#endif
	
	//初始化FLASH
	FlashInit();
	LoadParamsFromFlash();
	
	//初始化低电压检测
	//BatteryCheckInit();
	
	//初始化遥控
	RC_init();
	
	//测试用，延迟启动时间
//	for(i=0;i<6;i++)
//	{
//		LED_ON();
//		DelayMsec(1000);
//		LED_OFF();
//	}

	//初始化电机
	Motor_Init();
	//Motor_Start();

	
	//初始化自稳定
	IMU_Init();			// sample rate and cutoff freq.  sample rate is too low now due to using dmp.
	
	//printf("\n\nCPU @ %dHz\n", SystemCoreClock);

}

void loop()
{
//	static bool falg = false;
		static uint32_t nextTick = 0;
		while(getSystemTime()<nextTick){}
		nextTick = getSystemTime()+TICK_FRAME_PERIOD;	//循环间隔FRAME
	
		//处理蓝牙命令
		CommandProcess();
			
		//更新遥控状态
		RC_Update();
	
		if(GetFrameCount()%10 == 0)
		{
			//读取姿态传感器数据
			#ifdef IMU_SW												//软件姿态解算
				IMUSO3Thread();
			#else
				DMP_Routing();	//DMP 线程  所有的数据都在这里更新
			#endif
		}

		if(GetFrameCount()%20 == 0)
		{
			//遥控命令监测
			RC_CommandDetect();

			if(CarMode == HAND_STAND)
			{
				//平衡站立
				CtrlAttiAng();
				//控制电机
				CtrlMotor();
				
				DoActionUsingTime2();
			}
			else
			{
				DoActionUsingTime();
			}

		}

		if(GetFrameCount()%1000 == 0)
		{
			//电池电池检测
			//BatteryCheck();
			
			//遥控通信丢失处理
			
			
			//更新LED灯状态
		
			UpdateLED();
			//AudioPlay(2);
			
		}
		
		//故障保护
		if(GetFrameCount()%100 == 0)
		{
			//站立时，翻覆处理
			 FailSafeCrash();
			
			//与遥控连接断开
			FailSafeLostRC();
			
			//电池低电压处理
			//printf("Convert result is %d\n", GetBatteryAD());
			
		}
		
		//打印调试信息
		if(GetFrameCount()%100 == 0)
		{
			ReportMessage();
//			if(Comm_Data && Comm_Data[4]>0)
//			{
//				printf("Comm_Data%d,%d,%d,%d,%d\n",Comm_Data[0],Comm_Data[1],Comm_Data[2],Comm_Data[3],Comm_Data[4]);
//			}
		}
		IncFrameCount(1);
}


int main()
{
	//开机默认进入休眠
	InitSleepIO();
	IntoSleep();	
	
  setup();
	while(TRUE)
	{
		if(OPERTION_MODE == OPERATION)	//工作模式下
		{
			//查询
			//1.是否允许切换模式 2.按键是否按下 3.按下按键距今的时间
			if(PermitTonggleOperation && PressIsOn 
				&& (getSystemTime() - LastPressTime > WAKE_TIME))
			{
				OPERTION_MODE = SLEEP;
				
				AudioPlay(AUDIO_GOODBYE);
				//DelayMsec(2000);	//延迟时间，播放音乐
				
				IntoSleep();
				//printf("SLEEP");
				PermitTonggleOperation = false;
				
			}
			else
			{
				loop();
			}
		}
		else if(OPERTION_MODE == SLEEP)	//休眠模式下
		{
			//1.是否允许切换模式 2.按键是否按下 3.按下按键距今的时间
			if(PermitTonggleOperation &&  PressIsOn 
				&& (getSystemTime() - LastPressTime > WAKE_TIME))
			{
				OPERTION_MODE = OPERATION;
				//printf("OPERATION");
				PermitTonggleOperation = false;
				
				AudioPlay(AUDIO_WELCOME);
				//DelayMsec(2000);	//延迟时间，播放音乐
			}
			else if(PressIsOn == false)	//按键弹起时,按下的时间太短，再次进入休眠
			{
				//printf("SLEEP");
				IntoSleep();
				
			}
		}
	}
}

void MotorTest(void)
{
	static bool falg = false;
	//		if(GetFrameCount() > 6000 && GetFrameCount() < 8000)
//		{
//			Motor_Start();
//			MotorPwmOutput(20,20,20,20);
//		}
//		else if(GetFrameCount() >= 8000  && GetFrameCount() < 10000)
//		{
//			MotorPwmOutput(40,40,40,40);
//		}
//		else if(GetFrameCount() >= 10000   && GetFrameCount() < 12000)
//		{
//			MotorPwmOutput(60,60,60,60);
//		}
//		else if(GetFrameCount() >= 12000  && GetFrameCount() < 14000)
//		{
//			MotorPwmOutput(80,80,80,80);
//		}
//		else if(GetFrameCount() >= 14000 && GetFrameCount() < 16000)
//		{
//			MotorPwmOutput(100,100,100,100);
//		}
//		else 
		if(GetFrameCount() >= 8000 && !falg)
		{
			falg = true;
			MotorPwmOutput(0,0,0,0);
			Motor_Stop();
		}
}
	


/*** (C) COPYRIGHT ***/
