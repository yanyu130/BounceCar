#include "Timer.h"


uint16_t T16_ReloadVal;
uint8_t	DelayTime;
uint16_t FlashLedTime;
uint16_t SysTick;
void DelayMs(void)
{
	while(DelayTime);
}

/***************************************************************
 16位定时器初始化
***************************************************************/
void Timer16_Init(void)
{
	//IHRC=16MHz 溢出时间T=(1/(IHRC/16))*(32768-T16_ReloadVal)
	T16_ReloadVal = 32768-(16000000/16/T16_RateHz);
	stt16	T16_ReloadVal;
	$ T16M	 IHRC,/16,BIT15
}
/*--------------------------------------------------------------
--------------------------------------------------------------*/