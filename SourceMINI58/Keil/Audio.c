#include "Audio.h"
#include "def.h"
#include "Timer_Ctrl.h"

#define AUDIO_OKY P51		//引脚10
#define AUDIO_IO1 P50		//引脚11




void Audio_Init(void)
{
		SYS->P5_MFP |= (SYS_MFP_P50_GPIO | SYS_MFP_P51_GPIO);
	
		//引脚10
		GPIO_SetMode(P5, BIT0, GPIO_MODE_OUTPUT);
		//引脚11
		GPIO_SetMode(P5, BIT1, GPIO_MODE_OUTPUT);
		
		P50 = 0;
		P51 = 0;
}

void AudioPlay(uint8_t i)
{
	//Audio_Init();
		printf("play audio%d\n",i);
		
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
		
		//DelayMsec(50);
}



