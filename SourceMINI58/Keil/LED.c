#include "LED.h"
#include "def.h"
#include "Timer_Ctrl.h"

//#define NuTiny

uint8_t ledStatus = 0;
uint8_t ledLightLevel = 0;
uint32_t ioStatus;

void LED_Init(void)
{
	#ifdef NuTiny
		GPIO_SetMode(P3, BIT6, GPIO_MODE_OUTPUT);        // For NuTiny-SDK-Mini51L
		//P36 = 0;
	#elif defined MINI51
		GPIO_SetMode(P2, BIT6, GPIO_MODE_OUTPUT);
		//P26 = 1;
	#elif defined MINI58
		GPIO_SetMode(P1, BIT4, GPIO_MODE_OUTPUT);
		//P14 = 1;
	#endif

	//P14用于MPU6880 DMP中断
//	GPIO_SetMode(P1, BIT4, GPIO_PMD_INPUT);
//    GPIO_EnableInt(P1, 4, GPIO_INT_HIGH);
//    NVIC_EnableIRQ(GPIO01_IRQn);
}


//void GPIO01_IRQHandler(void)
//{
//    /* To check if P1.4 interrupt occurred */
//    if (P1->ISRC & BIT4) {
//        P1->ISRC = BIT4;
//        printf("P1.4 INT occurred. \n");

//    } else {
//        /* Un-expected interrupt. Just clear all PORT0, PORT1 interrupts */
//        P0->ISRC = P0->ISRC;
//        P1->ISRC = P1->ISRC;
//        printf("Un-expected interrupts. \n");
//    }
//}

void UpdateLED()
{
	
	ledStatus = !ledStatus;
	
	#ifdef NuTiny
		P36 = ledStatus;
	#elif defined MINI51
		P26 = ledStatus;
	#elif defined MINI58
		P14 = ledStatus;
	#endif
}

void LED_ON(void)
{
	P14 = 1;
}

void LED_Tongle(void)
{
	ledStatus = !ledStatus;
	P14 = ledStatus;
}

void LED_OFF(void)
{
	
	P14 = 0;
}
