#include "SleepCtrl.h"
#include "def.h"
#include "Timer_Ctrl.h"
#include "Led.h"
uint32_t LastPressTime;
bool PressIsOn = false;
bool PermitTonggleOperation = false;

uint8_t OPERTION_MODE = SLEEP;

void InitSleepIO(void)
{
	GPIO_SetMode(P3, BIT0, GPIO_MODE_QUASI);
	GPIO_EnableInt(P3, 0, GPIO_INT_FALLING | GPIO_INT_RISING);
	NVIC_EnableIRQ(GPIO234_IRQn);
}

void PrepareForWakeUp()
{
	//LED_OFF();
}

void IntoSleep(void)
{
	printf("sleep\n");
	PrepareForSleep();
	CLK_PowerDown();
	PrepareForWakeUp();
	printf("out sleep\n");
	
}

void CLK_Delay(uint16_t delay)
{
	uint16_t i=delay;
	while(i--);
}

void PrepareForSleep()
{
	LED_OFF();
}



void GPIO234_IRQHandler(void)
{
    /* To check if P3.0 interrupt occurred */
    if (P3->INTSRC & BIT0) {
        P3->INTSRC = BIT0;
        if(P30 == 0)
				{
					CLK_Delay(30000);
					if(P30 == 0)
					{
						PressIsOn = true;
						PermitTonggleOperation = true;
						printf("Press is on \n");
						//OPERTION_MODE = SLEEP;
						LastPressTime = getSystemTime();
					}
				}
				else
				{
					CLK_Delay(30000);
					if(P30 == 1)
					{
						printf("Press is off\n");
						PressIsOn = false;
					}
				}
				

    } else {
        /* Un-expected interrupt. Just clear all PORT2, PORT3, PORT4 interrupts */
        P2->INTSRC = P2->INTSRC;
        P3->INTSRC = P3->INTSRC;
				P4->INTSRC = P4->INTSRC;
        printf("Un-expected interrupts. \n");
    }
}
