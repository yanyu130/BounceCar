#include "RC.h"
#include "def.h"
#include "Comm.h"
#include "stdio.h"
#include "control.h"
#include "motor.h"

RC_GETDATA RC_DATA;

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
		//printf("Comm_Data%02x,%02x,%02x,%02x,%02x\n",Comm_Data[0],Comm_Data[1],
		//	Comm_Data[2],Comm_Data[3],Comm_Data[4]);
		command = Comm_Data[4];
		
		if(command == 0x01)
		//if(command == RC_MODEL_NORMAL)
		{
			printf("command%02x\n",command);
			//RC_DATA.COMMAND &= ~RC_MODEL_HAND_UP;
			//RC_DATA.COMMAND |= RC_MODEL_NORMAL;
			Motor_Start();
			CarMode = HAND_STAND;
		}
		else if(command == 0x02)
		{
			printf("command%02x\n",command);
			//RC_DATA.COMMAND &= ~MODEL_NORMAL;
			//RC_DATA.COMMAND |= MODEL_HAND_UP;
			CarMode = NORMAL;
		//	Motor_Stop();
		}
		else if(command & ANTICLOCK_WISE_LITTLE)
		{
			printf("RC_ANTICLOCK_WISE_LITTLE%02x\n",ANTICLOCK_WISE_LITTLE);
			RC_DATA.COMMAND = ANTICLOCK_WISE_LITTLE;
			SetActionUsingTime(ANTICLOCK_WISE_BIG,200,30);
		}
		else if(command & CLOCK_WISE_LITTLE)
		{
			printf("RC_CLOCK_WISE_LITTLE%02x\n",CLOCK_WISE_LITTLE);
			RC_DATA.COMMAND = CLOCK_WISE_LITTLE;
			SetActionUsingTime(CLOCK_WISE_LITTLE,200,30);
		}
		else if(command & ANTICLOCK_WISE_BIG)
		{
			printf("RC_ANTICLOCK_WISE_BIG%02x\n",ANTICLOCK_WISE_BIG);
			RC_DATA.COMMAND = ANTICLOCK_WISE_BIG;
			SetActionUsingTime(ANTICLOCK_WISE_BIG,200,60);
		}
		else if(command & CLOCK_WISE_BIG)
		{
			printf("RC_CLOCK_WISE_BIG%02x\n",CLOCK_WISE_BIG);
			RC_DATA.COMMAND = CLOCK_WISE_BIG;
			SetActionUsingTime(CLOCK_WISE_BIG,200,60);
			
		}
	}
}
