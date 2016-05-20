#ifndef _TIMER_H_
#define _TIMER_H_

#include <stdint.h>

#define	T16_RateHz		1000

extern uint16_t T16_ReloadVal;
extern uint8_t	DelayTime;
extern uint16_t FlashLedTime;
extern uint16_t SysTick;


void Timer16_Init(void);
void DelayMs(void);































#endif
