#ifndef UARTCTRL_H
#define UARTCTRL_H
#include "stdint.h"

#define RXBUFSIZE 15

extern uint8_t g_u8RecData[RXBUFSIZE];

void setupUART(void);
void UART_NVIC_INIT(void);
void CommandProcess(void);

uint8_t GetUartChar(void);

#endif
