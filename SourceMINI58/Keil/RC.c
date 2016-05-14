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
	uint8_t command1 = 0, command2 = 0, command3 = 0;
	
	//处理遥控数据
	if(Comm_Flag&Comm_NewData_Mask)
	{
		command1 = 0x00;
		Comm_Flag &= ~ Comm_NewData_Mask;
		printf("Comm_Data %02x,%02x,%02x,%02x,%02x\n",Comm_Data[0],Comm_Data[1],
			Comm_Data[2],Comm_Data[3],Comm_Data[4]);
		command1 = Comm_Data[4];
		command2 = Comm_Data[3];
		command3 = Comm_Data[1];
		
		if(command1 == ACTION_CLOCK_WISE_BIG_2S)
		{
			SetActionUsingTime(ACTION_ANTICLOCK_WISE_BIG,2000,60);
		}
		else if(command1 & ACTION_ANTICLOCK_WISE_LITTLE)
		{
			RC_DATA.COMMAND = ACTION_ANTICLOCK_WISE_LITTLE;
			SetActionUsingTime(ACTION_ANTICLOCK_WISE_BIG,100,30);
		}
		else if(command1 & ACTION_CLOCK_WISE_LITTLE)
		{
			RC_DATA.COMMAND = ACTION_CLOCK_WISE_LITTLE;
			SetActionUsingTime(ACTION_CLOCK_WISE_LITTLE,100,30);
		}
		else if(command1 & ACTION_ANTICLOCK_WISE_BIG)
		{
			RC_DATA.COMMAND = ACTION_ANTICLOCK_WISE_BIG;
			SetActionUsingTime(ACTION_ANTICLOCK_WISE_BIG,200,60);
		}
		else if(command1 & ACTION_CLOCK_WISE_BIG)
		{
			RC_DATA.COMMAND = ACTION_CLOCK_WISE_BIG;
			SetActionUsingTime(ACTION_CLOCK_WISE_BIG,200,60);
		}
		else if(command1 & ACTION_MODEL_JUMP_MODE1)
		{
			Jump(Mode1);
		}
		else if(command1 & ACTION_ROLL_180)
		{
			RC_DATA.COMMAND = ACTION_ROLL_180;
			//退出倒立模式
			if(CarMode == HAND_STAND) CarMode = NORMAL;
			
			if(command1_reset)
			{
				command1_reset = false;
				if(imu.pitch < 30)
				{
					SetActionUsingAngle(ACTION_ROLL_180,200,100);
				}
				else
				{
					SetActionUsingAngle(ACTION_ROLL_180,-30,100);
				}
			}
			
		}
		else if(command1 & ACTION_MODEL_JUMP_MODE2)
		{
			Jump(Mode2);
		}
		if(command1 == 0)
		{
			command1_reset = true;
		}
		
		if(command2 == MODEL_HAND_UP)
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
		
		if(command3 == 0x00)
		{
			SetBasicSpeed(BASIC_SPEED0);
		}
		else if(command3 == 0x01)
		{
			SetBasicSpeed(-BASIC_SPEED1);
		}
		else if(command3 == 0x02)
		{
			SetBasicSpeed(-BASIC_SPEED2);
		}
		else if(command3 == 0x03)
		{
			SetBasicSpeed(-BASIC_SPEED3);
		}
		else if(command3 == 0x04)
		{
			SetBasicSpeed(-BASIC_SPEED4);
		}
		else if(command3 == 0x10)
		{
			SetBasicSpeed(BASIC_SPEED1);
		}
		else if(command3 == 0x20)
		{
			SetBasicSpeed(BASIC_SPEED2);
		}
		else if(command3 == 0x30)
		{
			SetBasicSpeed(BASIC_SPEED3);
		}
		else if(command3 == 0x40)
		{
			SetBasicSpeed(BASIC_SPEED4);
		}
	}
}
