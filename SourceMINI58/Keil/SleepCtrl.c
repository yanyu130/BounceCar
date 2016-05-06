#include "SleepCtrl.h"
#include "def.h"
#include "Timer_Ctrl.h"
void InitSleepIO(void)
{
	GPIO_SetMode(P3, BIT0, GPIO_MODE_QUASI);
		
   GPIO_EnableInt(P3, 0, GPIO_INT_FALLING);
	NVIC_EnableIRQ(GPIO234_IRQn);
}

void IntoSleep(void)
{
	printf("sleep\n");
	CLK_PowerDown();
	printf("out sleep\n");
}

void GPIO234_IRQHandler(void)
{
    /* To check if P1.4 interrupt occurred */
    if (P3->INTSRC & BIT0) {
        P3->INTSRC = BIT0;
        printf("P3.0 INT occurred. \n");

    } else {
        /* Un-expected interrupt. Just clear all PORT0, PORT1 interrupts */
        P2->INTSRC = P2->INTSRC;
        P3->INTSRC = P3->INTSRC;
				P4->INTSRC = P4->INTSRC;
        printf("Un-expected interrupts. \n");
    }
}
