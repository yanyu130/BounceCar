#include	"extern.h"
#include	"User\XN297L\RF.H"
#include	"Comm.h"
#include	"Timer.h"
#include	"adc.h"
#include	"Key.h"


void IO_Init(void)
{
	/**************��IO����Ϊģ������**************/
//	$ PBDIER 0B0100_0001	//PB1~5ģ������
	//�����ǵ�λ��������������
	PBC.1 = 0;				//����Ϊ����
	PBPH.1 = 1;				//�ر��ڲ�����������
	//�����ǵ�λ��������������
	PBC.3 = 0;				//����Ϊ����
	PBPH.3 = 1;				//�ر��ڲ�����������
	//����ǵ�λ��������������
	PBC.2 = 0;				//����Ϊ����
	PBPH.2 = 1;				//�ر��ڲ�����������
	//���ŵ�λ��������������
	PBC.4 = 0;				//����Ϊ����
	PBPH.4 = 1;				//�ر��ڲ�����������
	//�͵�ѹ���������������
	PBC.5 = 0;				//����Ϊ����
	PBPH.5 = 1;				//�ر��ڲ�����������

	PAC.6 = 0;				//����Ϊ����
	PAPH.6 = 1;				//�ر��ڲ�����������

	PAC.7 = 0;				//����Ϊ����
	PAPH.7 = 1;				//�ر��ڲ�����������

	PAC.0 = 0;				//����Ϊ����
	PAPH.0 = 1;				//�ر��ڲ�����������

	PAC.5 = 0;				//����Ϊ����
//	PAPH.5 = 1;				//�ر��ڲ�����������
	$ PADIER 0B1111_1101	//PA3ģ������
	//�������������������
	PAC.3 = 0;				//����Ϊ����
	PAPH.3 = 0;				//�ر��ڲ�����������
	/**********************************************/

	PBC.0 = 1;//LED(PA7) Output
	//PBC.0 = 1;//�����������˿�
}




void	FPPA0 (void)
{
	.ADJUST_IC	SYSCLK=IHRC/8,IHRC=16MHz,VDD=3.3V,Bandgap=On

	clkmd.En_WatchDog	=	1;		//	WatchDog Enable

	//	Insert Initial Code
	Timer16_Init();	//��ʼ��T16��ʱ��
	$ INTEN	T16		//����T16�ж�
	engint			//��ȫ���ж�
	Comm_Init();
	IO_Init();
	ADC_Init();
	
	Key_Init();
//	Key_Noise_Filter_Init();
	FlashLedTime = 0;

	FPPEN.1 = 1;	//ʹ��FPPA1
 	while (1)
	{
//		...
		wdreset;
		if(FlashLedTime == 0)
		{
			switch(Comm_CurrentStatus)
			{
				case STATUS_CONNECTED:
				{
					FlashLedTime = 500;
					

					break;
				}
				case STATUS_DISCONNECT:
				case STATUS_CONNECTING:
				case STATUS_LOST:
				default:
				{
					FlashLedTime = 100;
					break;
				}
			}
			if(LED) LED = 0;
			else LED = 1;
		}

	}
}

void	FPPA1 (void)
{
	while(1)
	{
	
		Comm_Process();
	
			Key_Process();
		
	}
}


void	Interrupt (void)
{
	pushaf;

	if (Intrq.T16)
	{	//	T16 Trig
		//	User can add code
		SysTick++;
		if(FlashLedTime) FlashLedTime--;
		if(DelayTime) DelayTime--;
		stt16	T16_ReloadVal;
		Intrq.T16	=	0;
		//...
	}

	popaf;
}
