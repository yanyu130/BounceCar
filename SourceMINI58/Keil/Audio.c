#include "Audio.h"
//#include "def.h"
#include "Timer_Ctrl.h"

#define AUDIO_OKY P51		//引脚10
#define AUDIO_IO1 P50		//引脚11




void Audio_Init(void)
	{
		//引脚10
		GPIO_SetMode(P5, BIT0, GPIO_MODE_OUTPUT);
		//引脚11
		GPIO_SetMode(P5, BIT1, GPIO_MODE_OUTPUT);
		
		P50 = 0;
		P51 = 0;
	}

	void AudioSelect(uint8_t i)
	{
		AUDIO_IO1 = 1;
		DelayMsec(2);
		AUDIO_IO1 = 0;
		DelayMsec(2);
		
		while(i>0)  
		{
			AUDIO_OKY = 1;
			DelayMsec(1);
			AUDIO_OKY = 0; 
			DelayMsec(1);
			i --;
		}
	}



