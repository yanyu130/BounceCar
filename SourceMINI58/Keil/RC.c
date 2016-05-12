#include "RC.h"
#include "def.h"

#include "stdio.h"
#include "control.h"
#include "motor.h"
#include "jump.h"

RC_GETDATA RC_DATA;

uint8_t command_reset = true;

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
	uint8_t command;
	//处理遥控数据
	if(Comm_Flag&Comm_NewData_Mask)
	{
		command = 0x00;
		Comm_Flag &= ~ Comm_NewData_Mask;
		//printf("Comm_Data %02x,%02x,%02x,%02x,%02x\n",Comm_Data[0],Comm_Data[1],
		//	Comm_Data[2],Comm_Data[3],Comm_Data[4]);
		command = Comm_Data[4];
		
		if(command == MODEL_HAND_UP)
		{
			if(command_reset)
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
				command_reset = false;
			}
		}
		else if(command == CLOCK_WISE_BIG_2S)
		{
			//CarMode = NORMAL;
			SetActionUsingTime(ANTICLOCK_WISE_BIG,2000,60);
		}
		else if(command & ANTICLOCK_WISE_LITTLE)
		{
			//printf("RC_ANTICLOCK_WISE_LITTLE%02x\n",ANTICLOCK_WISE_LITTLE);
			RC_DATA.COMMAND = ANTICLOCK_WISE_LITTLE;
			SetActionUsingTime(ANTICLOCK_WISE_BIG,100,30);
			//command_reset = false;
		}
		else if(command & CLOCK_WISE_LITTLE)
		{
			//printf("RC_CLOCK_WISE_LITTLE%02x\n",CLOCK_WISE_LITTLE);
			RC_DATA.COMMAND = CLOCK_WISE_LITTLE;
			SetActionUsingTime(CLOCK_WISE_LITTLE,100,30);
			//command_reset = false;
		}
		else if(command & ANTICLOCK_WISE_BIG)
		{
			//printf("RC_ANTICLOCK_WISE_BIG%02x\n",ANTICLOCK_WISE_BIG);
			RC_DATA.COMMAND = ANTICLOCK_WISE_BIG;
			SetActionUsingTime(ANTICLOCK_WISE_BIG,200,60);
			//command_reset = false;
		}
		else if(command & CLOCK_WISE_BIG)
		{
			//printf("RC_CLOCK_WISE_BIG%02x\n",CLOCK_WISE_BIG);
			RC_DATA.COMMAND = CLOCK_WISE_BIG;
			SetActionUsingTime(CLOCK_WISE_BIG,200,60);
			//command_reset = false;
			
		}
		else if(command & MODEL_JUMP_MODE1)
		{
			Jump(Mode1);
			//command_reset = false;
		}	
		else if(command & MODEL_JUMP_MODE2)
		{
			Jump(Mode2);
			//command_reset = false;
		}
		if(command == 0)
		{
			command_reset = true;
		}
	}
}
