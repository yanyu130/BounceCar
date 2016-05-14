#include "RC.h"
#include "def.h"

#include "stdio.h"
#include "control.h"
#include "motor.h"
#include "jump.h"
#include "imu.h"

RC_GETDATA RC_DATA;

uint8_t command1_reset = true;
uint8_t command2_reset = true;

uint8_t getRC_Status()
{
		return Comm_CurrentStatus;
}

//初始化遥控
void RC_init(void)
{
	Comm_Init();
}

//更新遥控状态
void RC_Update(void)
{
	Comm_Process();
}

//遥控命令监测
void RC_CommandDetect(void)
{
	uint8_t command1,command2;
	//处理遥控数据
	if(Comm_Flag&Comm_NewData_Mask)
	{
		command1 = 0x00;
		Comm_Flag &= ~ Comm_NewData_Mask;
		//printf("Comm_Data %02x,%02x,%02x,%02x,%02x\n",Comm_Data[0],Comm_Data[1],
		//	Comm_Data[2],Comm_Data[3],Comm_Data[4]);
		command1 = Comm_Data[4];
		command2 = Comm_Data[3];
		
		if(command1 == CLOCK_WISE_BIG_2S)
		{
			SetActionUsingTime(ANTICLOCK_WISE_BIG,2000,60);
		}
		else if(command1 & ANTICLOCK_WISE_LITTLE)
		{
			RC_DATA.COMMAND = ANTICLOCK_WISE_LITTLE;
			SetActionUsingTime(ANTICLOCK_WISE_BIG,100,30);
		}
		else if(command1 & CLOCK_WISE_LITTLE)
		{
			RC_DATA.COMMAND = CLOCK_WISE_LITTLE;
			SetActionUsingTime(CLOCK_WISE_LITTLE,100,30);
		}
		else if(command1 & ANTICLOCK_WISE_BIG)
		{
			RC_DATA.COMMAND = ANTICLOCK_WISE_BIG;
			SetActionUsingTime(ANTICLOCK_WISE_BIG,200,60);
		}
		else if(command1 & CLOCK_WISE_BIG)
		{
			RC_DATA.COMMAND = CLOCK_WISE_BIG;
			SetActionUsingTime(CLOCK_WISE_BIG,200,60);
		}
		else if(command1 & MODEL_JUMP_MODE1)
		{
			Jump(Mode1);
		}
		else if(command1 & ROLL_180)
		{
			RC_DATA.COMMAND = ROLL_180;
			//退出倒立模式
			if(CarMode == HAND_STAND) CarMode = NORMAL;
			
			if(command1_reset)
			{
				command1_reset = false;
				if(imu.pitch < 30)
				{
					SetActionUsingAngle(ROLL_180,200,100);
				}
				else
				{
					SetActionUsingAngle(ROLL_180,-30,100);
				}
			}
			
		}
		else if(command1 & MODEL_JUMP_MODE2)
		{
			Jump(Mode2);
		}
		if(command1 == 0)
		{
			command1_reset = true;
		}
		
		if(command2 & MODEL_HAND_UP)
		{
			if(command2_reset)
			{
				if(CarMode == HAND_STAND)
				{
					CarMode = NORMAL;
					printf("NORMAL\n");
				}
				else
				{
					CarMode = HAND_STAND;
					printf("HAND_STAND\n");
				}
				command2_reset = false;
			}
		}
		
		if(command2 == 0)
		{
			command2_reset = true;
		}
	}
}
