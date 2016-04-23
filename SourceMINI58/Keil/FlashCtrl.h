#ifndef FLASHCTRL_H
#define FLASHCTRL_H
#include "stdint.h"

void FlashInit(void);
void DATA_FLASH_Write(uint32_t WriteAddr,int32_t *pBuffer,uint8_t NumToWrite);
void DATA_FLASH_Read(uint32_t ReadAddr,int32_t *pBuffer,uint8_t NumToRead);
//float dw2float(int32_t dw);
//int32_t float2dw(float f);

void FlashErase(uint32_t Addr);

#endif
