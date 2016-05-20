#include "jump.h"
#include "Timer_Ctrl.h"
#include "IMU.h"

uint8_t Jump_Flag;
uint8_t Jump_KeyDelay;
uint16_t Jump_Timeout;
uint32_t Jump_TimerTick;

void Jump_Init(void)
{
	GPIO_SetMode(P1, BIT0, GPIO_MODE_INPUT);//״̬����
	P24 = 0;
	P25 = 0;
	GPIO_SetMode(P2, BIT4, GPIO_MODE_OUTPUT);//��ת
	GPIO_SetMode(P2, BIT5, GPIO_MODE_OUTPUT);//��ת
	P24 = 0;
	P25 = 0;
	
	Jump_Flag = 0;
	Jump_KeyDelay = 0;
	Jump_Timeout = 0;
	Jump_TimerTick = getSystemTime();
}
void Jump_Motor_Set(MOTOR_DIR Dir)
{
	switch(Dir)
	{
		case Forward:
		{
			P24 = 0;
			P25 = 0;
			P24 = 1;
			break;
		}
		case Backward:
		{
			P24 = 0;
			P25 = 0;
			P25 = 1;
			break;
		}
		case Stop:
		default:
		{
			P24 = 0;
			P25 = 0;
			break;
		}
	}
}

uint8_t ucJump_GetKey(void)
{
	return ((P10==0)?0:1);
}


void Jump(JUMP_MODE Mode)
{
	switch(Mode)
	{
		case Mode1:
		{
			if((Jump_Flag&FLAG_MODE_MASK) == 0)
			{
					Jump_Flag &= ~FLAG_MODE_MASK;
					Jump_Flag |= FLAG_MODE1_ACTIVE;
					Jump_Timeout = MODE1_FINISH_TIME_LOAD_VAL;
					Jump_Flag |= FLAG_TIMEOUT_EN;
					Jump_Motor_Set(Forward);
			}
			break;
		}
		case Mode2:
		{
			if(((Jump_Flag&FLAG_MODE_MASK) == FLAG_MODE2_ACTIVE) || ((Jump_Flag&FLAG_MODE_MASK) == 0))
			{
				if((Jump_Flag&FLAG_MODE_MASK) == 0)
				{
					Jump_Flag &= ~FLAG_MODE_MASK;
					Jump_Flag |= FLAG_MODE2_ACTIVE;
					Jump_Motor_Set(Forward);
					Jump_Timeout = MODE2_READY_TIME_LOAD_VAL;
					Jump_Flag |= FLAG_TIMEOUT_EN;
				}
				else if(((Jump_Flag&FLAG_MODE_MASK) == FLAG_MODE2_ACTIVE) && (Jump_Flag&FLAG_MODE2_READY_MASK))
				{
					Jump_Motor_Set(Backward);
					Jump_Timeout = MODE2_BACKWARD_TIME_LOAD_VAL;
					Jump_Flag |= FLAG_TIMEOUT_EN;
				}
			}
			break;
		}
	}
}

void Jump_UpdateKeyStatus(void)
{
	if(ucJump_GetKey() != ((Jump_Flag&FLAG_KEY_VAL_MASK)>>3))//���ŵ�ƽ���ڲ������ֵ�����
	{
		if(Jump_Flag&FLAG_KEY_DELAY_EN_MASK)//������ʱ���Ѿ���
		{
			if(Jump_KeyDelay == 0)//����ʱ���ѵ�
			{
				if(ucJump_GetKey() != ((Jump_Flag&FLAG_KEY_VAL_MASK)>>3))//���ŵ�ƽ���ڲ������ֵ�����
				{
					Jump_Flag |= FLAG_NEW_KEY;
					Jump_Flag ^= FLAG_KEY_VAL_MASK;
				}
				Jump_Flag &= ~FLAG_KEY_DELAY_EN_MASK;//�ر�������ʱ��
			}
		}
		else
		{
			Jump_Flag |= FLAG_KEY_DELAY_EN;//��������ʱ��
			Jump_KeyDelay = KEY_DELAY_TIME_LOAD_VAL;
		}
	}
	else
	{
		Jump_Flag &= ~FLAG_KEY_DELAY_EN_MASK;//�ر�������ʱ��
	}
}

void Jump_TimerProcess(void)
{
	uint8_t temp = 0;
	if(Jump_TimerTick != getSystemTime())
	{
		if(Jump_TimerTick < getSystemTime())
		{
			temp = getSystemTime() - Jump_TimerTick;
			Jump_TimerTick = getSystemTime();
		}
		else if(Jump_TimerTick > getSystemTime())
		{
			Jump_TimerTick = ~Jump_TimerTick+1;//Jump_TimerTick = 0xFFFFFFFF - Jump_TimerTick;
			temp = Jump_TimerTick;
			temp += getSystemTime();
			Jump_TimerTick = getSystemTime();
		}

		if(Jump_Flag&FLAG_KEY_DELAY_EN_MASK)
		{
			if(Jump_KeyDelay > temp) Jump_KeyDelay -= temp;
			else Jump_KeyDelay = 0;
		}
		if(Jump_Flag&FLAG_TIMEOUT_EN_MASK)
		{
			if(Jump_Timeout > temp) Jump_Timeout -= temp;
			else Jump_Timeout = 0;
		}
	}
}


void Jump_Sever(void)
{
	Jump_TimerProcess();
	Jump_UpdateKeyStatus();
	if(Jump_Flag&FLAG_NEW_KEY_MASK) printf("New key,val=%d",(Jump_Flag&FLAG_KEY_VAL_MASK)>>3);
//	printf("gyroRawX:%4.2f,gyroRawY:%4.2f,gyroRawZ:%4.2f \n",imu.gyroRaw[0],imu.gyroRaw[1],imu.gyroRaw[2]);
	if(Jump_Flag&FLAG_MODE_MASK)
	{
		if((Jump_Flag&FLAG_MODE_MASK) == FLAG_MODE1_ACTIVE)			//ģʽ1
		{
			if(Jump_Flag&FLAG_NEW_KEY_MASK)//
			{
				if((Jump_Flag&FLAG_KEY_VAL_MASK) == 0)//��⵽�½���
				{
					Jump_Timeout = MODE1_JUMP_TRIG_TIME_LOAD_VAL;
					Jump_Flag |= FLAG_TIMEOUT_EN;
				}
				Jump_Flag &= ~FLAG_NEW_KEY;
			}
			if(Jump_Flag&FLAG_TIMEOUT_EN_MASK)
			{
				if(Jump_Timeout==0)
				{
					Jump_Motor_Set(Stop);
					Jump_Flag &= ~FLAG_MODE_MASK;
					Jump_Flag &= ~FLAG_TIMEOUT_EN_MASK;
				}
			}
		}
		else if((Jump_Flag&FLAG_MODE_MASK) == FLAG_MODE2_ACTIVE)//ģʽ2
		{
			if(Jump_Flag&FLAG_MODE2_READY_MASK)
			{
				if(Jump_Flag&FLAG_TIMEOUT_EN_MASK)
				{
					if(Jump_Timeout==0)
					{
						Jump_Motor_Set(Stop);
						Jump_Flag &= ~FLAG_MODE_MASK;
						Jump_Flag &= ~FLAG_TIMEOUT_EN_MASK;
						Jump_Flag &= ~FLAG_MODE2_READY;
						Jump_Flag &= ~FLAG_MODE2_TRIG;
					}
				}
			}
			else
			{
				if(Jump_Flag&FLAG_MODE2_TRIG_MASK)//�Ѿ������˵�λ����
				{
					if(Jump_Flag&FLAG_TIMEOUT_EN_MASK)
					{
						if(Jump_Timeout==0)//��ʱ����ʱ
						{
							Jump_Motor_Set(Stop);
							Jump_Flag &= ~FLAG_TIMEOUT_EN_MASK;
							Jump_Flag |= FLAG_MODE2_READY;
						}
					}
				}
				else//��û������λ����
				{
					if(Jump_Flag&FLAG_NEW_KEY_MASK)//
					{
						if((Jump_Flag&FLAG_KEY_VAL_MASK) == 0)//��⵽�½���
						{
							//����װ���Ѿ���λ
							Jump_Motor_Set(Backward);
							Jump_Timeout = MODE2_BACKWARD_TIME_LOAD_VAL;
							Jump_Flag |= FLAG_TIMEOUT_EN;
							Jump_Flag |= FLAG_MODE2_TRIG;
						}
						Jump_Flag &= ~FLAG_NEW_KEY;
					}
					else
					{
						if(Jump_Flag&FLAG_TIMEOUT_EN_MASK)
						{
							if(Jump_Timeout==0)//��ʱ����ʱ
							{
								Jump_Motor_Set(Stop);
								Jump_Flag &= ~FLAG_MODE_MASK;
								Jump_Flag &= ~FLAG_TIMEOUT_EN_MASK;
								Jump_Flag &= ~FLAG_MODE2_READY;
								Jump_Flag &= ~FLAG_MODE2_TRIG;
							}
						}
					}
				}
			}
		}
		else//δ����ģʽ
		{
			Jump_Motor_Set(Stop);
			Jump_Flag &= ~FLAG_MODE_MASK;
		}
	}
	Jump_Flag &= ~FLAG_NEW_KEY;//һ��ѭ������°�����־λ
}

uint8_t ucJump_GetStatus(void)
{
	return ((Jump_Flag&FLAG_MODE_MASK)?1:0);
}















































