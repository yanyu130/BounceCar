#include <stdint.h>
#include "adc.h"

uint8_t ADC_Channel;
uint16_t ADC_Resoult;

void ADC_Init(void)
{
	/*************设置ADC时钟和分辨率**************/
	$ ADCM	12BIT,/1		//12Bit分辨率  系统时钟1分频
	$ ADCC	Enable			//使能ADC模块
}


void ADC_GetValue(void)
{
	//选择ADC通道
/*	if(ADC_Channel!=0x0F)
	{
		if(ADC_Channel > 6) ADC_Channel++;
	}*/
	ADCC &= ~0x3C;	//清除ADC通道
	ADCC |= (ADC_Channel << 2);//设置ADC通道
	set1	adcc.6	//启动ADC转换
	nop;
	wait1	adcc.6	//等待转换完成
	ADC_Resoult$1 = ADCRH;
	ADC_Resoult$0=  ADCRl;
	ADC_Resoult=ADC_Resoult>>4;
}




















