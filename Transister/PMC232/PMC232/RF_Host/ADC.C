#include <stdint.h>
#include "adc.h"

uint8_t ADC_Channel;
uint16_t ADC_Resoult;

void ADC_Init(void)
{
	/*************����ADCʱ�Ӻͷֱ���**************/
	$ ADCM	12BIT,/1		//12Bit�ֱ���  ϵͳʱ��1��Ƶ
	$ ADCC	Enable			//ʹ��ADCģ��
}


void ADC_GetValue(void)
{
	//ѡ��ADCͨ��
/*	if(ADC_Channel!=0x0F)
	{
		if(ADC_Channel > 6) ADC_Channel++;
	}*/
	ADCC &= ~0x3C;	//���ADCͨ��
	ADCC |= (ADC_Channel << 2);//����ADCͨ��
	set1	adcc.6	//����ADCת��
	nop;
	wait1	adcc.6	//�ȴ�ת�����
	ADC_Resoult$1 = ADCRH;
	ADC_Resoult$0=  ADCRl;
	ADC_Resoult=ADC_Resoult>>4;
}



















