
#include "Battery.h"
#include "adc.h"
#include "stdio.h"


uint32_t u32BatteryAD;

void BatteryCheckInit(void)
{
	CLK_EnableModuleClock(ADC_MODULE);
	CLK_SetModuleClock(ADC_MODULE,CLK_CLKSEL1_ADCSEL_HIRC,CLK_CLKDIV_ADC(6));
	
	/* Set P1 multi-function pins for ADC channel 5 input*/
    SYS->P1_MFP = SYS_MFP_P15_ADC_CH5;

    /* Analog pin OFFD to prevent leakage */
    P1->DINOFF |= (1 << 5) << GP_DINOFF_DINOFF0_Pos;
	
	// Enable channel 5
    ADC_Open(ADC, 0, 0, 0x01 << 5);
	
	// Power on ADC
    ADC_POWER_ON(ADC);
	
	// Enable ADC convert complete interrupt
    ADC_EnableInt(ADC, ADC_ADIF_INT);
    NVIC_EnableIRQ(ADC_IRQn);
	
}

void ADC_IRQHandler(void)
{
		uint32_t u32Flag;
    // Get ADC comparator interrupt flag
    u32Flag = ADC_GET_INT_FLAG(ADC, ADC_ADIF_INT);

    // Get ADC convert result
    //printf("Convert result is %x\n", (uint32_t)ADC_GET_CONVERSION_DATA(ADC, 0));
		u32BatteryAD = (uint32_t)ADC_GET_CONVERSION_DATA(ADC, 0);
	
    ADC_CLR_INT_FLAG(ADC, u32Flag);
}

//检查电池电压
void BatteryCheck(void)
{
	if(!ADC_IS_BUSY(ADC)) 
	{
		/* 开始转换 */
		ADC_START_CONV(ADC);
	}
}

//判断是否低电压
bool isLowBattery()
{
	float battery = u32BatteryAD/1024*AD_REFERENCE;
	if(battery < LOW_BATTERY)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//获取电池电压AD
uint32_t GetBatteryAD(void)
{
	return u32BatteryAD;
}
