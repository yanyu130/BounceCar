/*============================================================================*
 * O     O          __                   ______  __                           *
 *  \   /      /\  / /_      _    __    / /___/ / /_     _                    *
 *   [+]      /  \/ / \\    //__ / /__ / /____ / / \\   //                    *
 *  /   \    / /\  /   \\__// --/ /---/ /----// /   \\_//                     *
 * O     O  /_/  \/     \__/    \_\/ /_/     /_/ ____/_/                      *
 *                                                                            *
 *                                                                            *
 * Multi-Rotor controller firmware for Nuvoton Cortex M4 series               *
 *                                                                            *
 * Written by by T.L. Shen for Nuvoton Technology.                            *
 * tlshen@nuvoton.com/tzulan611126@gmail.com                                  *
 *                                                                            *
 *============================================================================*
 */
#include <stdio.h>
#include "def.h"
#include "Def.h"
#include "i2c.h"
#include "NVT_I2C.h"


//#define BusError    1
//#define ArbitLoss   2
//#define TimeOut     4
/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t Device_W_Addr;
uint8_t Device_R_Addr;
uint8_t ReadFlag;

uint8_t Tx_Data0[MAX_I2C_DATA_LEN];
uint8_t Rx_Data0[MAX_I2C_DATA_LEN];
uint8_t DataLen0;
uint8_t RxLen0;
uint8_t ContinueLen;
volatile uint8_t EndFlag0 = 0;
volatile uint8_t ErrorFlag = 0;
//uint8_t  Addr1[3] = {0};
//uint8_t  DataLen1;
//uint8_t  Slave_Buff1[32] = {0};
//uint16_t Slave_Buff_Addr1;
//extern void DelayUsec(unsigned int usec);
//extern void DelayMsec(unsigned int usec);

typedef void (*I2C_FUNC)(uint32_t u32Status);
static volatile I2C_FUNC s_I2C0HandlerFn = NULL;
void I2CX_IRQHandler(uint8_t id)
{
	uint32_t u32Status;

	u32Status = I2C_GET_STATUS(I2C);

	if(I2C_GET_TIMEOUT_FLAG(I2C)) 
	{
		ErrorFlag = 1;
	/* Clear I2C0 Timeout Flag */
		I2C_ClearTimeoutFlag(I2C);
	}
	else {
		switch (u32Status) {	
			case 0x38:{/* Arbitration loss */
				I2C_SET_CONTROL_REG(I2C, I2C_STO | I2C_SI); 
				I2C_SET_CONTROL_REG(I2C, I2C_STA);
				//printf("I2C Arbitration loss\n");
				break;
			}
			case 0x00:	/* Bus error */
			case 0x30:
			case 0xF8:
			case 0x48: {
				I2C_SET_CONTROL_REG(I2C, I2C_STO | I2C_SI);
				//I2C_SET_CONTROL_REG(I2C_PORT, I2C_CTL_SI);
				I2C_SET_CONTROL_REG(I2C, I2C_STA);
				//printf("I2C bus error\n");
				break;
			}
			default: 
				{
				if(id==0) 
				{
					if(s_I2C0HandlerFn != NULL)
						s_I2C0HandlerFn(u32Status);
				}	
			}
		}
	}
}
void I2C0_IRQHandler(void)
{
	I2CX_IRQHandler(0);
}

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C0 (Master) Rx Callback Function                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
//void I2C_Callback_Rx(uint32_t status)
//{
//    if (status == 0x08)                     /* START has been transmitted and prepare SLA+W */
//    {
//#ifdef M451
//        I2C_SET_DATA(I2C_PORT, Device_W_Addr << 1);    /* Write SLA+W to Register I2CDAT */
//        I2C_SET_CONTROL_REG(I2C_PORT, I2C_CTL_SI);
//#else
//        I2C_SET_DATA(I2C, Device_W_Addr << 1);     /* Write SLA+W to Register I2CDAT */
//        I2C_SET_CONTROL_REG(I2C, I2C_SI);
//#endif
//    }   
//    else if (status == 0x18)                /* SLA+W has been transmitted and ACK has been received */
//    {
//#ifdef M451
//			I2C_SET_DATA(I2C_PORT, Tx_Data0[DataLen0++]);
//      I2C_SET_CONTROL_REG(I2C_PORT, I2C_CTL_SI);
//#else
//      I2C_SET_DATA(I2C, Tx_Data0[DataLen0++]);
//      I2C_SET_CONTROL_REG(I2C, I2C_SI);
//#endif
//    }
//    else if (status == 0x20)                /* SLA+W has been transmitted and NACK has been received */
//    {
//#ifdef M451
//      I2C_SET_CONTROL_REG(I2C_PORT, I2C_CTL_STA_STO_SI);
//#else
//      I2C_SET_CONTROL_REG(I2C, I2C_STO | I2C_SI);
//#endif
//    }
//    else if (status == 0x28)                /* DATA has been transmitted and ACK has been received */
//    {
//        if (DataLen0 != 2)
//        {
//#ifdef M451
//          I2C_SET_DATA(I2C_PORT, Tx_Data0[DataLen0++]);
//          I2C_SET_CONTROL_REG(I2C_PORT, I2C_CTL_SI);
//#else
//          I2C_SET_DATA(I2C, Tx_Data0[DataLen0++]);
//          I2C_SET_CONTROL_REG(I2C, I2C_SI);
//#endif
//        }
//        else
//        {
//#ifdef M451
//          I2C_SET_CONTROL_REG(I2C_PORT, I2C_CTL_STA_SI);
//#else
//          I2C_SET_CONTROL_REG(I2C, I2C_STA | I2C_SI);
//#endif
//        }
//    }
//    else if (status == 0x10)                /* Repeat START has been transmitted and prepare SLA+R */
//    {
//#ifdef M451
//      I2C_SET_DATA(I2C_PORT, ((Device_W_Addr << 1) | 0x01));   /* Write SLA+R to Register I2CDAT */
//      I2C_SET_CONTROL_REG(I2C_PORT, I2C_CTL_SI);
//#else
//      I2C_SET_DATA(I2C, ((Device_W_Addr << 1) | 0x01));   /* Write SLA+R to Register I2CDAT */
//      I2C_SET_CONTROL_REG(I2C, I2C_SI);
//#endif
//    }
//    else if (status == 0x40)                /* SLA+R has been transmitted and ACK has been received */
//    {
//#ifdef M451
//      I2C_SET_CONTROL_REG(I2C_PORT, I2C_CTL_SI);
//#else
//      I2C_SET_CONTROL_REG(I2C, I2C_SI);
//#endif
//    }
//    else if (status == 0x58)                /* DATA has been received and NACK has been returned */
//    {
//#ifdef M451
//      Rx_Data0[RxLen0++] = (unsigned char) I2C_GET_DATA(I2C_PORT);
//      I2C_SET_CONTROL_REG(I2C_PORT, I2C_CTL_STO_SI);
//#else
//      //Rx_Data0[0] = DrvI2C_ReadData(I2C_PORT);
//      //DrvI2C_Ctrl(I2C_PORT, 0, 1, 1, 0);
//      EndFlag0 = 1;
//			Rx_Data0[RxLen0++] = I2C_GET_DATA(I2C);
//      I2C_SET_CONTROL_REG(I2C, I2C_STO | I2C_SI);
//      //g_u8EndFlag = 1;
//#endif 
//    }
//    else
//    {
//        printf("Status 0x%x is NOT processed\n", status);
//    }
//}

void I2C_Callback_Rx_Continue(uint32_t status)
{
    if (status == 0x08)                     /* START has been transmitted and prepare SLA+W */
    {
        I2C_SET_DATA(I2C, Device_W_Addr << 1|ReadFlag);    /* Write SLA+W to Register I2CDAT */
        I2C_SET_CONTROL_REG(I2C, I2C_SI);

    }   
    else if (status == 0x18)                /* SLA+W has been transmitted and ACK has been received */
    {
      I2C_SET_DATA(I2C, Tx_Data0[DataLen0++]);
      I2C_SET_CONTROL_REG(I2C, I2C_SI);

    }
    else if (status == 0x20)                /* SLA+W has been transmitted and NACK has been received */
    {

      I2C_SET_CONTROL_REG(I2C, I2C_STO | I2C_SI);

    }
    else if (status == 0x28)                /* DATA has been transmitted and ACK has been received */
    {
      I2C_SET_CONTROL_REG(I2C, I2C_STA | I2C_SI);

    }
    else if (status == 0x10)                /* Repeat START has been transmitted and prepare SLA+R */
    {
      I2C_SET_DATA(I2C, ((Device_W_Addr << 1) | 0x01));   /* Write SLA+R to Register I2CDAT */
      I2C_SET_CONTROL_REG(I2C, I2C_SI);
    }
    else if (status == 0x40)                /* SLA+R has been transmitted and ACK has been received */
    {
      if(ContinueLen>1)
        I2C_SET_CONTROL_REG(I2C, I2C_SI | I2C_AA);
      else 
        I2C_SET_CONTROL_REG(I2C, I2C_SI);
    }       
    else if (status == 0x58)                /* DATA has been received and NACK has been returned */
    {

      Rx_Data0[RxLen0++] = (unsigned char) I2C_GET_DATA(I2C);  
      I2C_SET_CONTROL_REG(I2C, I2C_STO | I2C_SI);
      EndFlag0 = 1;

    }
    else if (status == 0x50)                /* DATA has been received and ACK has been returned */
    {

      Rx_Data0[RxLen0++] = (unsigned char) I2C_GET_DATA(I2C);  
      if(RxLen0<(ContinueLen-1))
        I2C_SET_CONTROL_REG(I2C, I2C_SI | I2C_AA);
      else {
        I2C_SET_CONTROL_REG(I2C, I2C_SI);
      }

    }
    else
    {
//        printf("Status 0x%x is NOT processed\n", status);
    }           
}

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C0 (Master) Tx Callback Function                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
//void I2C_Callback_Tx(uint32_t status)
//{
//    if (status == 0x08)                     /* START has been transmitted */
//    {
//#ifdef M451
//      I2C_SET_DATA(I2C_PORT, Device_W_Addr << 1);    /* Write SLA+W to Register I2CDAT */
//      I2C_SET_CONTROL_REG(I2C_PORT, I2C_CTL_SI);
//#else
//      I2C_SET_DATA(I2C, Device_W_Addr << 1);     /* Write SLA+W to Register I2CDAT */
//      I2C_SET_CONTROL_REG(I2C, I2C_SI);
//#endif
//    }   
//    else if (status == 0x18)                /* SLA+W has been transmitted and ACK has been received */
//    {
//#ifdef M451
//      I2C_SET_DATA(I2C_PORT, Tx_Data0[DataLen0++]);
//      I2C_SET_CONTROL_REG(I2C_PORT, I2C_CTL_SI);
//#else
//      I2C_SET_DATA(I2C, Tx_Data0[DataLen0++]);
//        I2C_SET_CONTROL_REG(I2C, I2C_SI);
//#endif
//    }
//    else if (status == 0x20)                /* SLA+W has been transmitted and NACK has been received */
//    {
//#ifdef M451
//      I2C_SET_CONTROL_REG(I2C_PORT, I2C_CTL_STA_STO_SI);
//#else
//      I2C_SET_CONTROL_REG(I2C, I2C_STO | I2C_SI);
//#endif
//    }   
//    else if (status == 0x28)                /* DATA has been transmitted and ACK has been received */
//    {
//        if (DataLen0 != 3)
//        {
//#ifdef M451
//          I2C_SET_DATA(I2C_PORT, Tx_Data0[DataLen0++]);
//          I2C_SET_CONTROL_REG(I2C_PORT, I2C_CTL_SI);
//#else
//          I2C_SET_DATA(I2C, Tx_Data0[DataLen0++]);
//          I2C_SET_CONTROL_REG(I2C, I2C_SI);
//#endif
//        }
//        else
//        {
//#ifdef M451
//          I2C_SET_CONTROL_REG(I2C_PORT, I2C_CTL_STO_SI);
//#else
//          I2C_SET_CONTROL_REG(I2C, I2C_STO | I2C_SI);
//            //g_u8EndFlag = 1;
//#endif 
//          EndFlag0 = 1;
//        }       
//    }
//    else
//    {
//        printf("Status 0x%x is NOT processed\n", status);
//    }       
//}

void I2C_Callback_Tx_Continue(uint32_t status)
{
    if (status == 0x08)                     /* START has been transmitted */
    {

      I2C_SET_DATA(I2C, Device_W_Addr << 1);    /* Write SLA+W to Register I2CDAT */
      I2C_SET_CONTROL_REG(I2C, I2C_SI);

    }   
    else if (status == 0x18)                /* SLA+W has been transmitted and ACK has been received */
    {
      I2C_SET_DATA(I2C, Tx_Data0[DataLen0++]);
      I2C_SET_CONTROL_REG(I2C, I2C_SI);
    }
    else if (status == 0x20)                /* SLA+W has been transmitted and NACK has been received */
    {

      I2C_SET_CONTROL_REG(I2C, I2C_STA | I2C_STO | I2C_SI);

    }   
    else if (status == 0x28)                /* DATA has been transmitted and ACK has been received */
    {
        if (DataLen0 != ContinueLen)
        {
          I2C_SET_DATA(I2C, Tx_Data0[DataLen0++]);
          I2C_SET_CONTROL_REG(I2C, I2C_SI);
        }
        else
        {

          I2C_SET_CONTROL_REG(I2C, I2C_STO | I2C_SI);

          EndFlag0 = 1;
        }       
    }
    else
    {
//        printf("Status 0x%x is NOT processed\n", status);
    }       
}


void WaitEndFlag0(uint16_t timeout)
{
	if(timeout)
		I2C_EnableTimeout(I2C, (uint8_t) timeout);
	
	while (EndFlag0 == 0) {
		if(ErrorFlag) {
			break;
		}
	};
	I2C_DisableTimeout(I2C);
}


uint8_t NVT_WriteByteContinue_addr8(uint8_t address,uint8_t* data, uint8_t len)
{
	uint8_t i;
	
	Tx_Data0[0]=address;
	
	for(i=0;i<len;i++)
		Tx_Data0[1+i]=data[i];
	
	DataLen0 = 0;
	EndFlag0 = 0;
	ErrorFlag = 0;
	ContinueLen=len+1;

	s_I2C0HandlerFn = (I2C_FUNC)I2C_Callback_Tx_Continue;
	while(I2C->CTL & I2C_CTL_STO_Msk);
	I2C_SET_CONTROL_REG(I2C, I2C_STA);
	WaitEndFlag0(1);

	return ErrorFlag;
}

uint8_t NVT_ReadByteContinue_addr8(uint8_t address,uint8_t* data, uint8_t len, uint16_t timeout)
{
	uint8_t i;
	
	RxLen0 = 0;
	DataLen0 = 0;
	EndFlag0 = 0;
	ReadFlag = 0;
	ErrorFlag = 0;
	ContinueLen = len;
	Tx_Data0[0] = address;

	s_I2C0HandlerFn = (I2C_FUNC)I2C_Callback_Rx_Continue;
	while(I2C->CTL & I2C_CTL_STO_Msk);
	I2C_SET_CONTROL_REG(I2C, I2C_STA);

	WaitEndFlag0(timeout);

	for(i = 0; i<len; i++)
		data[i]=Rx_Data0[i];
	
	return ErrorFlag;
}


void NVT_I2C_Init()
{
	  CLK_SetModuleClock(I2C0_MODULE, 0, 0);
		CLK_EnableModuleClock(I2C0_MODULE);
	
	/* Set P3.4 and P3.5 for I2C SDA and SCL */
    SYS->P3_MFP |= SYS_MFP_P34_I2C0_SDA | SYS_MFP_P35_I2C0_SCL;

	/* Open I2C and set clock to 400k */
    I2C_Open(I2C, 400000);

//    /* Get I2C Bus Clock */
//    printf("I2C clock %d Hz\n", I2C_GetBusClockFreq(I2C));

    I2C_EnableInt(I2C);
    NVIC_EnableIRQ(I2C0_IRQn);
}

void NVT_SetDeviceAddress(uint8_t devAddr)
{
	Device_W_Addr=devAddr;
	Device_R_Addr=devAddr+1;
}

