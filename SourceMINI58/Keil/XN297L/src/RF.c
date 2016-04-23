//============================================================================//
//  * @file            RF.c
//  * @author         Shi Zheng 
//  * @version        V1.0
//  * @date           24/4/2015
//  * @brief          RFPN006 communication interface
//  * @modify user:   Shizheng
//  * @modify date:   24/4/2015
//============================================================================//
#include "RF.H"
#include "Timer_Ctrl.h"

const uint8_t TX_ADDRESS_DEF[5] = {0xCa,0xCb,0xCd,0xCC,0xCC};    		//RF 地址：接收端和发送端需一致


const uint8_t  BB_cal_data[]    = {0x0A,0x6D,0x67,0x9C,0x46};                               //1M速率配置
const uint8_t  RF_cal_data[]    = {0xF6,0x37,0x5D};
const uint8_t  RF_cal2_data[]   = {0x45,0x21,0xef,0x2C,0x5A,0x50};
const uint8_t  Dem_cal_data[]   = {0x01};  
const uint8_t  Dem_cal2_data[]  = {0x0b,0xDF,0x02};  
/*
const uint8_t  BB_cal_data[5]    = {0x0A,0x6D,0x67,0x9C,0x46};                                 //250K速率配置
const uint8_t  RF_cal_data[3]    = {0xF6,0x33,0x5D};
const uint8_t  RF_cal2_data[6]   = {0xD5,0x21,0xeb,0x2C,0x5A,0x40};
const uint8_t  Dem_cal_data[1]   = {0x1E};  
const uint8_t  Dem_cal2_data[3]  = {0x0b,0xDF,0x02}; 
 */

const uint8_t Carrier_BB_cal_data[]    = {0x0A,0x6D,0x67,0x9C,0x46}; 
const uint8_t Carrier_RF_cal_data[]    = {0xF6,0x37,0x5D};
const uint8_t Carrier_RF_cal2_data[]   = {0x45,0x21,0xEF,0x2C,0x5A,0x50};
const uint8_t Carrier_Dem_cal_data[]   = {0xE1}; 								
const uint8_t Carrier_Dem_cal2_data[]  = {0x0B,0xDF,0x02};
		
		
/******************************************************************************/
//            SPI_init
//               init spi pin  input/out mode
/******************************************************************************/
void SPI_init(void)
{    
	GPIO_SetMode(P0, BIT4, GPIO_MODE_OUTPUT);	//CSN PIN output High pulling push
	P04 = 1;
	GPIO_SetMode(P0, BIT7, GPIO_MODE_OUTPUT);	//SCK PIN output Low  pulling push 
	P07 = 0;
	GPIO_SetMode(P0, BIT6, GPIO_MODE_OUTPUT);  //DATA PIN output  DEFAULT  High pulling push
	P06 = 0;             
}

void SPI_CLK_Delay(void)
{
	uint8_t i=2;
	while(i--);
}

/******************************************************************************/
//           SPI_WW
//                SPI Write a byte for write regiest
/******************************************************************************/
void SPI_WW(uint8_t R_REG)
{
	uint8_t  i;
	for(i = 0; i < 8; i++)
	{
		SCK_LOW;
		if(R_REG & 0x80)
		{
			SPI_DATA_HIGH;
		}
		else
		{
			SPI_DATA_LOW;
		}
		SPI_CLK_Delay();
		R_REG = R_REG << 1;
		SCK_HIGH;
		SPI_CLK_Delay();
	}
	SCK_LOW;
}

/******************************************************************************/
//            SPI_WR
//                SPI Write a byte for read regiset
/******************************************************************************/
void SPI_WR(uint8_t R_REG)
{
	uint8_t  i;
	for(i = 0; i < 8; i++)
	{
		SCK_LOW;
		if(R_REG & 0x80)
		{
			SPI_DATA_HIGH;
		}
		else
		{
			SPI_DATA_LOW;
		}
		R_REG = R_REG << 1;
		SPI_CLK_Delay();
		SCK_HIGH;
		SPI_CLK_Delay();
	}
	SPI_DATA_INPUT_MODE;
	SCK_LOW;
}

/******************************************************************************/
//            ucSPI_Read
//                SPI Read BYTE
/******************************************************************************/
uint8_t ucSPI_Read(void)
{
	uint8_t i,data;  
	data = 0; 
	for(i = 0; i < 8; i++)
	{
		SCK_LOW;
		SPI_CLK_Delay();
		data = data << 1;
		SCK_HIGH;
		SPI_CLK_Delay();
		if(SPI_DATA_STATUS)
		{
			data |= 0x01;
		}
	}
	SCK_LOW;
	return data;
}


void RF_WriteReg(uint8_t RegAddr, uint8_t Data)
{
	CSN_LOW;
	SPI_WW(SPI_COMMAND_W_REGISTER | RegAddr);
	SPI_WW(Data);
	CSN_HIGH;
}

uint8_t ucRF_ReadReg(uint8_t RegAddr)
{
	uint8_t ret;
	CSN_LOW;
	SPI_WR(SPI_COMMAND_R_REGISTER | RegAddr);
	ret = ucSPI_Read();
	SPI_DATA_OUTPUT_MODE;
	CSN_HIGH;
	return ret;
}

void RF_WriteRegWitchBytes(uint8_t RegAddr, uint8_t *pData, uint8_t Leng)
{
	uint8_t i;
	CSN_LOW;
	SPI_WW(SPI_COMMAND_W_REGISTER | RegAddr);
	for(i = 0; i < Leng; i++)
	{
		SPI_WW(*(pData+i));
	}
	CSN_HIGH;
}

void RF_ReadRegWitchBytes(uint8_t RegAddr, uint8_t *pData, uint8_t Leng)
{
	uint8_t i;
	CSN_LOW;
	SPI_WR(SPI_COMMAND_R_REGISTER | RegAddr);
	for(i = 0; i < Leng; i++)
	{
		*(pData+i) = ucSPI_Read();
	}
	SPI_DATA_OUTPUT_MODE;
	CSN_HIGH;
}

void RF_WriteFIFO(uint8_t *pData, uint8_t Leng)
{
	uint8_t i;
	if(Leng > MAX_PAYLOAD_LENG) return;
	CSN_LOW;
	SPI_WW(SPI_COMMAND_W_TX_PAYLOAD);
	for(i = 0; i < Leng; i++)
	{
		SPI_WW(*(pData+i));
	}
	CSN_HIGH;
}


void RF_ReadFIFO(uint8_t *pData, uint8_t Leng)
{
	uint8_t i;
	if(Leng > MAX_PAYLOAD_LENG) return;
	CSN_LOW;
	SPI_WR(SPI_COMMAND_R_RX_PAYLOAD);
	for(i = 0; i < Leng; i++)
	{
		*(pData+i) = ucSPI_Read();
	}
	SPI_DATA_OUTPUT_MODE;
	CSN_HIGH;
}



/******************************************************************************/
//            RF_ClearFIFO
//                clear RF TX/RX FIFO
/******************************************************************************/
void RF_ClearFIFO(void)
{
	CSN_LOW;
	SPI_WW(SPI_COMMAND_FLUSH_TX);		                                		//清除RF 的 TX FIFO		
	CSN_HIGH;
	
	CSN_LOW;
	SPI_WW(SPI_COMMAND_FLUSH_RX);	                                  		//清除RF 的 RX FIFO	
	CSN_HIGH;
}





/******************************************************************************/
//            RF_GetStatus
//            read RF IRQ status,3bits return
/******************************************************************************/
uint8_t ucRF_GetStatus(void)
{
	return ucRF_ReadReg(STATUS)&0x70;	//读取RF的状态 			
}
/******************************************************************************/
//            ucRF_GetRSSI
//                读取rssi 值
/******************************************************************************/
uint8_t ucRF_GetRSSI(void)
{
	return ucRF_ReadReg(DATAOUT);
}
/******************************************************************************/
//            RF_ClearStatus
//                clear RF IRQ
/******************************************************************************/
void RF_ClearStatus(void)
{
	RF_WriteReg(STATUS,0x70);	//清除RF的IRQ标志
}

/******************************************************************************/
//            RF_SetChannel
//                Set RF TX/RX channel:Channel
/******************************************************************************/
void RF_SetChannel( uint8_t Channel)
{    
	RF_WriteReg(RF_CH,Channel);
}

/******************************************************************************/
//            RF_SetRxP0Addr
//                Set RF RX Pipe0 address
/******************************************************************************/
void RF_SetRxP0Addr(uint8_t *Addr, uint8_t Leng)
{
	uint8_t i;
	CSN_LOW;
	SPI_WW(SPI_COMMAND_W_REGISTER | RX_ADDR_P0);
	for(i = 0; i < Leng; i++)
	{
		SPI_WW(*(Addr+i));
	}
	CSN_HIGH;
}

/******************************************************************************/
//            RF_SetRxP1Addr
//                Set RF RX Pipe1 address
/******************************************************************************/
void RF_SetRxP1Addr(uint8_t *Addr, uint8_t Leng)
{
	uint8_t i;
	CSN_LOW;
	SPI_WW(SPI_COMMAND_W_REGISTER | RX_ADDR_P1);
	for(i = 0; i < Leng; i++)
	{
		SPI_WW(*(Addr+i));
	}
	CSN_HIGH;
}
/******************************************************************************/
//            RF_SetRxP2-5Addr
//                Set RF RX Pipe2-5 address Byte0,Byte1-4 same as Pipe1
/******************************************************************************/
void RF_SetRxP2_5Addr(PipeNumberDef Pipe, uint8_t Addr)
{
	CSN_LOW;
	switch(Pipe)
	{
		case Pipe2:
			SPI_WW(SPI_COMMAND_W_REGISTER | RX_ADDR_P2);
			break;
		case Pipe3:
			SPI_WW(SPI_COMMAND_W_REGISTER | RX_ADDR_P3);
			break;
		case Pipe4:
			SPI_WW(SPI_COMMAND_W_REGISTER | RX_ADDR_P4);
			break;
		case Pipe5:
		default:
			SPI_WW(SPI_COMMAND_W_REGISTER | RX_ADDR_P5);
			break;
	}
	SPI_WW(Addr);
	CSN_HIGH;
}

/******************************************************************************/
//            RF_SetTxAddr
//                Set RF TX address
/******************************************************************************/
void RF_SetTxAddr(uint8_t *Addr, uint8_t Leng)
{
	uint8_t i;
	CSN_LOW;
	SPI_WW(SPI_COMMAND_W_REGISTER | TX_ADDR);
	for(i = 0; i < Leng; i++)
	{
		SPI_WW(*(Addr+i));
	}
	CSN_HIGH;
}


/******************************************************************************/
//            RF_TxMode
//                Set RF into TX mode
/******************************************************************************/
void RF_TxMode(void)
{
    CE_LOW;
    RF_WriteReg(CONFIG,  0X8E);							// 将RF设置成TX模式
 //   delay_10us(1);
}


/******************************************************************************/
//            RF_RxMode
//            将RF设置成RX模式，准备接收数据
/******************************************************************************/
void RF_RxMode(void)
{
    CE_LOW;
    RF_WriteReg(CONFIG,  0X8F);							// 将RF设置成RX模式
    CE_HIGH;											// Set CE pin high 开始接收数据
 //   delay_ms(2);
}



/******************************************************************************/
//            发送数据：
//            参数：
//              1. ucPayload：需要发送的数据首地址
//              2. length:  需要发送的数据长度     
//              length 通常等于 PAYLOAD_WIDTH
/******************************************************************************/
void RF_TxData( uint8_t *ucPayload,  uint8_t length)
{
	uint8_t Status;
	Status = ucRF_GetStatus();
	if(0 == Status)                                                                        // rf free status                                                                                                                                                                   
	{
		RF_WriteFIFO(ucPayload, length); 
		CE_HIGH;                                                                    		//rf entery tx mode start send data 
		//keep ce high at least 600us
		do
		{
			DelayMsec(1);
			Status = ucRF_GetStatus()&TX_DS_FLAG;
		}while(Status == 0);
		CE_LOW;                                                                                     //rf entery stb3    
		RF_ClearStatus();
	}
}

/******************************************************************************/
//            ucRF_DumpRxData
//            读出接收到的数据：
//            参数：
//              1. ucPayload：存储读取到的数据的Buffer
//              2. length:    读取的数据长度
//              Return:
//              1. 0: 没有接收到数据
//              2. 1: 读取接收到的数据成功
//              note: Only use in Rx Mode
//              length 通常等于 PAYLOAD_WIDTH
/******************************************************************************/
uint8_t ucRF_DumpRxData( uint8_t *ucPayload,  uint8_t length)
{ 
   if(ucRF_GetStatus()&RX_DR_FLAG)
   {
   
        CE_LOW;
        RF_ReadFIFO(ucPayload, length);                                		//将接收到的数据读出到ucPayload，且清除rxfifo
        RF_ClearFIFO();
        RF_ClearStatus ();                              		                        //清除Status     
        CE_HIGH;                                                                    		//继续开始接        
        return 1;
    }
     return 0;
}



////////////////////////////////////////////////////////////////////////////////

//          以下部分与RF通信相关，不建议修改
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
//            PN006_Initial
//                Initial RF
/******************************************************************************/

void RF_Init(void)
{
	SPI_init();

	DelayMsec(2);
	RF_WriteReg(SPI_COMMAND_RST_FSPI, 0x5A);								//Software Reset    			
	RF_WriteReg(SPI_COMMAND_RST_FSPI, 0XA5);    
	//RF_WriteReg(FEATURE, 0x20);                                                    // enable Software control ce 

#if(PAYLOAD_WIDTH <33)											
	RF_WriteReg(FEATURE, 0x20);							//切换到32byte模式   使能CE
#else
	RF_WriteReg(FEATURE, 0x38);							//切换到64byte模式	   
#endif

	CE_LOW;                    
	RF_ClearFIFO();													// CLEAR TXFIFO	 CLEAR  RXFIFO	 
	RF_WriteReg(STATUS, 0x70);							// CLEAR  STATUS	
	RF_WriteReg(EN_RXADDR, 0x01);							// Enable Pipe0
	RF_WriteReg(SETUP_AW,  AW_4Byte);							// address witdth is 4 bytes
	RF_WriteReg(RF_CH,     DEFAULT_CHANNEL);       // 2400M HZ
	RF_WriteReg(RX_PW_P0,  PAYLOAD_WIDTH);						// 14 bytes
	RF_WriteRegWitchBytes(RF_CAL, (uint8_t *)RF_cal_data, sizeof(RF_cal_data));				//设置RF射频参数
	RF_WriteRegWitchBytes(RF_CAL2, (uint8_t *)RF_cal2_data, sizeof(RF_cal2_data));
	RF_WriteRegWitchBytes(DEM_CAL, (uint8_t *)Dem_cal_data, sizeof(Dem_cal_data));		//设置解调器参数
	RF_WriteRegWitchBytes(DEM_CAL2, (uint8_t *)Dem_cal2_data, sizeof(Dem_cal2_data));
	RF_WriteRegWitchBytes(BB_CAL, (uint8_t *)BB_cal_data, sizeof(BB_cal_data));				//设置基带参数
	RF_WriteReg(DYNPD, 0x00);					
	RF_WriteReg(RF_SETUP,  RF_POWER);						
		
	//RF_SetTxAddr(( uint8_t*)TX_ADDRESS_DEF, sizeof(TX_ADDRESS_DEF));	// Writes TX_Address to PN006
	//RF_SetRxP0Addr(( uint8_t*)TX_ADDRESS_DEF, sizeof(TX_ADDRESS_DEF));	// RX_Addr0 same as TX_Adr for Auto.Ack   
		

#if(TRANSMIT_TYPE == TRANS_ENHANCE_MODE)      
    RF_WriteReg(SETUP_RETR,0x03);							//  3 retrans... 	
    RF_WriteReg(EN_AA,     0x01);							// Enable Auto.Ack:Pipe0  	
#elif(TRANSMIT_TYPE == TRANS_BURST_MODE)                                                                
    RF_WriteReg(SETUP_RETR,0x00);							// Disable retrans... 	
    RF_WriteReg(EN_AA,     0x00);							// Disable AutoAck 
#endif
}

/******************************************************************************/
//            		进入载波模式
/******************************************************************************/
void RF_Carrier( uint8_t ucChannel_Set)
{
	RF_WriteReg(SPI_COMMAND_RST_FSPI, 0x5A);								//Software Reset    			
	RF_WriteReg(SPI_COMMAND_RST_FSPI, 0XA5);
	RF_WriteReg(FEATURE, 0x20);
	CE_LOW;
	//   delay_ms(200);
	RF_WriteReg(RF_CH, ucChannel_Set);						//单载波频点	   
	RF_WriteReg(RF_SETUP, RF_POWER);      						//13dbm
		
	RF_WriteRegWitchBytes(RF_CAL, (uint8_t *)Carrier_RF_cal_data, sizeof(Carrier_RF_cal_data));				//设置RF射频参数
	RF_WriteRegWitchBytes(RF_CAL2, (uint8_t *)Carrier_RF_cal2_data, sizeof(Carrier_RF_cal2_data));
	RF_WriteRegWitchBytes(DEM_CAL, (uint8_t *)Carrier_Dem_cal_data, sizeof(Carrier_Dem_cal_data));		//设置解调器参数
	RF_WriteRegWitchBytes(DEM_CAL2, (uint8_t *)Carrier_Dem_cal2_data, sizeof(Carrier_Dem_cal2_data));
	RF_WriteRegWitchBytes(BB_CAL, (uint8_t *)Carrier_BB_cal_data, sizeof(Carrier_BB_cal_data));				//设置基带参数
}

/***************************************end of file ************************************/












