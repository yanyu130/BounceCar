#include "FlashCtrl.h"
#include "def.h"

#include "fmc.h"
#include "def.h"
#include "stdio.h"

#define PAGE_SIZE 128 //512 Bytes

uint32_t data_flash_Start_address;

int  set_data_flash_base(uint32_t u32DFBA)
{
    uint32_t   au32Config[2];

    if (FMC_ReadConfig(au32Config, 2) < 0) {
        printf("\nRead User Config failed!\n");
        return -1;
    }

    if ((!(au32Config[0] & 0x1)) && (au32Config[1] == u32DFBA))
        return 0;

    FMC_ENABLE_CFG_UPDATE();

    au32Config[0] &= ~0x1;
    au32Config[1] = u32DFBA;

    if (FMC_WriteConfig(au32Config, 2) < 0)
        return -1;

    //printf("\nSet Data Flash base as 0x%x.\n", DATA_Flash_Start_ADD);

    // Perform chip reset to make new User Config take effect
    //SYS->IPRSTC1 = SYS_IPRSTC1_CHIP_RST_Msk;
		SYS->IPRST0 = SYS_IPRST0_CHIPRST_Msk;
    return 0;
}


void FlashInit()
{
	SYS_UnlockReg();
	FMC_Open();
	set_data_flash_base(DATA_Flash_Start_ADD);
	data_flash_Start_address = FMC_ReadDataFlashBaseAddr();
	printf("DATA_Flash_Start_ADD 0x%x.\n", data_flash_Start_address);
	FMC_Close();
	SYS_LockReg();

	printf("Flash initilize - [OK]\n");
}


void DATA_FLASH_Write(uint32_t WriteAddr,int32_t *pBuffer,uint8_t NumToWrite)	
{ 			 		 
	uint8_t i;
	//uint32_t data_buff[PAGE_SIZE];
	//__set_PRIMASK(1);//Avoid interrupt

	SYS_UnlockReg();
	FMC_Open();
	
	for(i=0; i<NumToWrite;i++)
	{
		FMC_Write(WriteAddr + i*4, pBuffer[i]);
	}

	FMC_Close();
	SYS_LockReg();
	//__set_PRIMASK(0);
} 

void FlashErase(uint32_t Addr)
{
	SYS_UnlockReg();
	FMC_Open();
	
	FMC_Erase(Addr);

	FMC_Close();
	SYS_LockReg();
}

void DATA_FLASH_Read(uint32_t ReadAddr,int32_t *pBuffer,uint8_t NumToRead)
{
		uint8_t i;
	//uint32_t data_buff[PAGE_SIZE];
//	__set_PRIMASK(1);//Avoid interrupt

	SYS_UnlockReg();
	FMC_Open();

		for(i=0;i<NumToRead;i++)
		{
			pBuffer[i] = FMC_Read(ReadAddr + i*4);
		}

	FMC_Close();
	SYS_LockReg();
//	__set_PRIMASK(0);
	
}

//int32_t float2dw(float f)
//{
//	int32_t* pdw;

//	pdw = (int32_t*)&f;
//	return *pdw;
//}
//float dw2float(int32_t dw)
//{
//	float* pf;

//	pf = (float*)&dw;
//	return *pf;
//}
//int32_t i162dw(int16_t i16)
//{
//	int32_t* pdw;
//	int32_t i32;
//	i32 = i16;

//	pdw = (int32_t*)&i32;
//	return *pdw;
//}
//int16_t dw2i16(int32_t dw)
//{
//	int16_t* pi16;
//	int16_t i16;
//	
//	i16 = dw;
//	pi16 = (int16_t*)&i16;
//	return *pi16;
//}

