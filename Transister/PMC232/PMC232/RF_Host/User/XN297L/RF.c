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

#include "../../Timer.h"

#define	BB_cal_data_Leng							5
#define	RF_cal_data_Leng							3
#define	RF_cal2_data_Leng							6
#define	Dem_cal_data_Leng							1
#define	Dem_cal2_data_Leng							3
const uint16_t  BB_cal_data[BB_cal_data_Leng]		= {0x0A,0x6D,0x67,0x9C,0x46};                               //1M速率配置
const uint16_t  RF_cal_data[RF_cal_data_Leng]		= {0xF6,0x37,0x5D};
const uint16_t  RF_cal2_data[RF_cal2_data_Leng]		= {0x45,0x21,0xef,0x2C,0x5A,0x50};
const uint16_t  Dem_cal_data[Dem_cal_data_Leng]		= {0x01};  
const uint16_t  Dem_cal2_data[Dem_cal2_data_Leng]	= {0x0b,0xDF,0x02};  
/*
const uint16_t  BB_cal_data[5]    = {0x0A,0x6D,0x67,0x9C,0x46};                                 //250K速率配置
const uint16_t  RF_cal_data[3]    = {0xF6,0x33,0x5D};
const uint16_t  RF_cal2_data[6]   = {0xD5,0x21,0xeb,0x2C,0x5A,0x40};
const uint16_t  Dem_cal_data[1]   = {0x1E};  
const uint16_t  Dem_cal2_data[3]  = {0x0b,0xDF,0x02}; 
 */

#define	Carrier_BB_cal_data_Leng									5
#define	Carrier_RF_cal_data_Leng									3
#define	Carrier_RF_cal2_data_Leng									6
#define	Carrier_Dem_cal_data_Leng									1
#define	Carrier_Dem_cal2_data_Leng									3
const uint16_t Carrier_BB_cal_data[Carrier_BB_cal_data_Leng]		= {0x0A,0x6D,0x67,0x9C,0x46}; 
const uint16_t Carrier_RF_cal_data[Carrier_RF_cal_data_Leng]		= {0xF6,0x37,0x5D};
const uint16_t Carrier_RF_cal2_data[Carrier_RF_cal2_data_Leng]		= {0x45,0x21,0xEF,0x2C,0x5A,0x50};
const uint16_t Carrier_Dem_cal_data[Carrier_Dem_cal_data_Leng]		= {0xE1}; 								
const uint16_t Carrier_Dem_cal2_data[Carrier_Dem_cal2_data_Leng]	= {0x0B,0xDF,0x02};





uint16_t pRF_TxAddr,pRF_RxAddr;
uint8_t	RF_Channel,RF_Status;//,RF_RSSI;

uint8_t	RF_Leng;
uint16_t pRF_Data;

uint8_t SPI_Data;
uint8_t RF_RegAddr,RF_RegData;
	
/******************************************************************************/
//            SPI_init
//               init spi pin  input/out mode
/******************************************************************************/
void SPI_init(void)
{    
	PCC.0 = 1;									//CSN PIN output High pulling push
	PC.0 = 1;
	PCC.5 = 1;									//SCK PIN output Low  pulling push 
	PC.5 = 0;
	PAC.4 = 1;									//DATA PIN output  DEFAULT  High pulling push
	PA.4 = 0;
}



/******************************************************************************/
//           SPI_WW
//                SPI Write a byte for write regiest
/******************************************************************************/
void SPI_WW(void)
{
	uint8_t  i = 0;
	while(i<8)
    {
        SCK_LOW;
        if(SPI_Data.7)
        {
            SPI_DATA_HIGH;
        }
        else
        {
            SPI_DATA_LOW;
        }
        SPI_Data = SPI_Data << 1;
        SCK_HIGH;
		i++;
    }
   SCK_LOW;
}

/******************************************************************************/
//            SPI_WR
//                SPI Write a byte for read regiset
/******************************************************************************/
void SPI_WR(void)
{
    uint8_t  i = 0;
	while(i<8)
    {
        SCK_LOW;
        if(SPI_Data.7)
        {
            SPI_DATA_HIGH;
        }
        else
        {
            SPI_DATA_LOW;
        }
        SPI_Data = SPI_Data << 1;
        SCK_HIGH;
		i++;
    }
   SPI_DATA_INPUT_MODE;
   SCK_LOW;
}

/******************************************************************************/
//            ucSPI_Read
//                SPI Read BYTE
/******************************************************************************/
void ucSPI_Read(void)
{
    uint8_t i = 0;  
    SPI_Data = 0; 
    while(i<8)
    {
        SCK_HIGH;
		i++;
        SPI_Data = SPI_Data << 1;
        if(SPI_DATA_STATUS)
        {
          SPI_Data |= 0x01;
        }
        SCK_LOW;
    }
    SCK_LOW;
}







void RF_WriteReg(void)
{
	CSN_LOW;
	SPI_Data = SPI_COMMAND_W_REGISTER | RF_RegAddr;
	SPI_WW();
	SPI_Data = RF_RegData;
	SPI_WW();
	CSN_HIGH;
}

void ucRF_ReadReg(void)
{
	CSN_LOW;
	SPI_Data = SPI_COMMAND_R_REGISTER | RF_RegAddr;
	SPI_WR();
	ucSPI_Read();
	SPI_DATA_OUTPUT_MODE;
	CSN_HIGH;
	RF_RegData = SPI_Data;
}
/*
void RF_WriteRegWitchBytes(void)
{
	uint8_t i = 0;
	CSN_LOW;
	SPI_Data = SPI_COMMAND_W_REGISTER | RF_RegAddr;
	SPI_WW();
	while(i < RF_Leng)
	{
		SPI_Data = *pRF_Data;
		SPI_WW();
		i++;
		pRF_Data++;
	}
	CSN_HIGH;
}

void RF_ReadRegWitchBytes(void)
{
	uint8_t i = 0;
	CSN_LOW;
	SPI_Data = SPI_COMMAND_R_REGISTER | RF_RegAddr;
	SPI_WR();
	while(i < RF_Leng)
	{
		ucSPI_Read();
		*pRF_Data = SPI_Data;
		i++;
		pRF_Data++;
	}
	SPI_DATA_OUTPUT_MODE;
	CSN_HIGH;
}
*/
void RF_WriteFIFO(void)
{
	uint8_t i = 0;
	if(RF_Leng > MAX_PAYLOAD_LENG) return;
	CSN_LOW;
	SPI_Data = SPI_COMMAND_W_TX_PAYLOAD;
	SPI_WW();
	while(i < RF_Leng)
	{
		SPI_Data = *pRF_Data;
		SPI_WW();
		i++;
		pRF_Data++;
	}
	CSN_HIGH;
}


void RF_ReadFIFO(void)
{
	uint8_t i = 0;
	if(RF_Leng > MAX_PAYLOAD_LENG) return;
	CSN_LOW;
	SPI_Data = SPI_COMMAND_R_RX_PAYLOAD;
	SPI_WR();
	while(i < RF_Leng)
	{
		ucSPI_Read();
		*pRF_Data = SPI_Data;
		i++;
		pRF_Data++;
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
	SPI_Data = SPI_COMMAND_FLUSH_TX;
	SPI_WW();		                                		//清除RF 的 TX FIFO		
	CSN_HIGH;

	CSN_LOW;
	SPI_Data = SPI_COMMAND_FLUSH_RX;
	SPI_WW();	                                  		//清除RF 的 RX FIFO	
	CSN_HIGH;
}





/******************************************************************************/
//            RF_GetStatus
//            read RF IRQ status,3bits return
/******************************************************************************/
void ucRF_GetStatus(void)
{
	RF_RegAddr = STATUS;
	ucRF_ReadReg();			//读取RF的状态 	
	RF_Status = SPI_Data & 0x70;
}
/******************************************************************************/
//            ucRF_GetRSSI
//                读取rssi 值
/******************************************************************************/
/*void ucRF_GetRSSI(void)
{
	RF_RegAddr = DATAOUT;
	ucRF_ReadReg();
	RF_RSSI = SPI_Data;
}*/
/******************************************************************************/
//            RF_ClearStatus
//                clear RF IRQ
/******************************************************************************/
void RF_ClearStatus(void)
{
	RF_RegAddr = STATUS;
	RF_RegData = 0x70;
	RF_WriteReg();	//清除RF的IRQ标志
}

/******************************************************************************/
//            RF_SetChannel
//                Set RF TX/RX channel:Channel
/******************************************************************************/
void RF_SetChannel(void)
{    
	CSN_LOW;
	SPI_Data = SPI_COMMAND_W_REGISTER | RF_CH;
	SPI_WW();
	SPI_Data = RF_Channel;
	SPI_WW();
	CSN_HIGH;
}

/******************************************************************************/
//            RF_SetRxP0Addr
//                Set RF RX Pipe0 address
/******************************************************************************/
void RF_SetRxP0Addr(void)
{
	uint8_t i = 0;
	CSN_LOW;
	SPI_Data = SPI_COMMAND_W_REGISTER | RX_ADDR_P0;
	SPI_WW();
	while(i < RF_Leng)
	{
		SPI_Data = *pRF_RxAddr;
		SPI_WW();
		i++;
		pRF_RxAddr++;
	}
	CSN_HIGH;
}

/******************************************************************************/
//            RF_SetRxP1Addr
//                Set RF RX Pipe1 address
/******************************************************************************/
/*void RF_SetRxP1Addr(void)
{
	uint8_t i = 0;
	CSN_LOW;
	SPI_Data = SPI_COMMAND_W_REGISTER | RX_ADDR_P1;
	SPI_WW();
	while(i < RF_Leng)
	{
		SPI_Data = *pRF_RxAddr;
		SPI_WW();
		i++;
		pRF_RxAddr++;
	}
	CSN_HIGH;
}*/
/******************************************************************************/
//            RF_SetRxP2-5Addr
//                Set RF RX Pipe2-5 address Byte0,Byte1-4 same as Pipe1
/******************************************************************************/
/*void RF_SetRxP2_5Addr(PipeNumberDef Pipe, uint8_t Addr)
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
}*/

/******************************************************************************/
//            RF_SetTxAddr
//                Set RF TX address
/******************************************************************************/
void RF_SetTxAddr(void)
{
	uint8_t i = 0;
	CSN_LOW;
	SPI_Data = SPI_COMMAND_W_REGISTER | TX_ADDR;
	SPI_WW();
	while(i < RF_Leng)
	{
		SPI_Data = *pRF_TxAddr;
		SPI_WW();
		i++;
		pRF_TxAddr++;
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
	RF_RegAddr = CONFIG;
	RF_RegData = 0X8E;
    RF_WriteReg();							// 将RF设置成TX模式
 //   delay_10us(1);
}


/******************************************************************************/
//            RF_RxMode
//            将RF设置成RX模式，准备接收数据
/******************************************************************************/
void RF_RxMode(void)
{
    CE_LOW;
	RF_RegAddr = CONFIG;
	RF_RegData = 0X8F;
    RF_WriteReg();							// 将RF设置成RX模式
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
void RF_TxData(void)
{
	ucRF_GetStatus();
	if(0 == RF_Status)                                                                        // rf free status                                                                                                                                                                   
	{
		RF_WriteFIFO(); 
		CE_HIGH;                                                                    		//rf entery tx mode start send data 
		//keep ce high at least 600us
		do
		{
			DelayTime = 1;
			DelayMs();
			ucRF_GetStatus();
			RF_Status &= TX_DS_FLAG;
		}while(RF_Status == 0);
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
void ucRF_DumpRxData(void)
{
	ucRF_GetStatus();
	RF_Status &= RX_DR_FLAG;
	if(RF_Status)
	{
        CE_LOW;
        RF_ReadFIFO();                                			//将接收到的数据读出到ucPayload，且清除rxfifo
        RF_ClearFIFO();
        RF_ClearStatus();                              		                        //清除Status     
        CE_HIGH;                                                                    		//继续开始接        
    }
}


void RF_SetCal(void)
{
	uint8_t i = 0;

	/////////////设置RF射频参数///////////////////
	i = 0;
	pRF_Data = RF_cal_data;
	CSN_LOW;
	SPI_Data = SPI_COMMAND_W_REGISTER | RF_CAL;
	SPI_WW();
	while(i < RF_cal_data_Leng)
	{
		SPI_Data = *pRF_Data$L;
		SPI_WW();
		i++;
		pRF_Data++;
	}
	CSN_HIGH;

	i = 0;
	pRF_Data = RF_cal2_data;
	CSN_LOW;
	SPI_Data = SPI_COMMAND_W_REGISTER | RF_CAL2;
	SPI_WW();
	while(i < RF_cal2_data_Leng)
	{
		SPI_Data = *pRF_Data$L;
		SPI_WW();
		i++;
		pRF_Data++;
	}
	CSN_HIGH;
	////////////////////////////////////////////////


	///////////设置解调器参数/////////////////////////
	i = 0;
	pRF_Data = Dem_cal_data;
	CSN_LOW;
	SPI_Data = SPI_COMMAND_W_REGISTER | DEM_CAL;
	SPI_WW();
	while(i < Dem_cal_data_Leng)
	{
		SPI_Data = *pRF_Data$L;
		SPI_WW();
		i++;
		pRF_Data++;
	}
	CSN_HIGH;

	i = 0;
	pRF_Data = Dem_cal2_data;
	CSN_LOW;
	SPI_Data = SPI_COMMAND_W_REGISTER | DEM_CAL2;
	SPI_WW();
	while(i < Dem_cal2_data_Leng)
	{
		SPI_Data = *pRF_Data$L;
		SPI_WW();
		i++;
		pRF_Data++;
	}
	CSN_HIGH;
	//////////////////////////////////////////////////


	//////////////设置基带参数////////////////////////
	i = 0;
	pRF_Data = BB_cal_data;
	CSN_LOW;
	SPI_Data = SPI_COMMAND_W_REGISTER | BB_CAL;
	SPI_WW();
	while(i <BB_cal_data_Leng)
	{
		SPI_Data = *pRF_Data$L;
		SPI_WW();
		i++;
		pRF_Data++;
	}
	CSN_HIGH;	
	//////////////////////////////////////////////////
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
	pRF_Data = 0;
	pRF_TxAddr = 0;
	pRF_RxAddr = 0;
	RF_Leng = 0;
	RF_Channel = 0;
	SPI_init();

	RF_RegAddr = SPI_COMMAND_RST_FSPI;
	RF_RegData = 0x5A;
	RF_WriteReg();								//Software Reset    		
	RF_RegAddr = SPI_COMMAND_RST_FSPI;
	RF_RegData = 0XA5;
	RF_WriteReg(); 
//	RF_RegAddr = FEATURE;
//	RF_RegData = 0x20;
//	RF_WriteReg();                            // enable Software control ce 

#if(PAYLOAD_WIDTH <33)
	RF_RegAddr = FEATURE;
	RF_RegData = 0x20;
	RF_WriteReg();							//切换到32byte模式   使能CE
#else
	RF_RegAddr = FEATURE;
	RF_RegData = 0x38;
	RF_WriteReg();							//切换到64byte模式	   
#endif

	CE_LOW;                    
	RF_ClearFIFO();													// CLEAR TXFIFO	 CLEAR  RXFIFO	 
	RF_RegAddr = STATUS;
	RF_RegData = 0x70;
	RF_WriteReg();							// CLEAR  STATUS	
	RF_RegAddr = EN_RXADDR;
	RF_RegData = 0x01;
	RF_WriteReg();							// Enable Pipe0
	RF_RegAddr = SETUP_AW;
	RF_RegData = AW_4Byte;
	RF_WriteReg();							// address witdth is 4 bytes
	RF_RegAddr = RF_CH;
	RF_RegData = DEFAULT_CHANNEL;
	RF_WriteReg();    					   // 2400M HZ
	RF_RegAddr = RX_PW_P0;
	RF_RegData = PAYLOAD_WIDTH;
	RF_WriteReg();							// 14 bytes

	RF_SetCal();


	RF_RegAddr = DYNPD;
	RF_RegData = 0x00;
	RF_WriteReg();		
	RF_RegAddr = RF_SETUP;
	RF_RegData = RF_POWER;
	RF_WriteReg();						// 13DBM  	
		
		
	//RF_SetTxAddr(( uint8_t*)TX_ADDRESS_DEF, sizeof(TX_ADDRESS_DEF));	// Writes TX_Address to PN006
	//RF_SetRxP0Addr(( uint8_t*)TX_ADDRESS_DEF, sizeof(TX_ADDRESS_DEF));	// RX_Addr0 same as TX_Adr for Auto.Ack   
		

#if(TRANSMIT_TYPE == TRANS_ENHANCE_MODE) 
	RF_RegAddr = SETUP_RETR;
	RF_RegData = 0x03;
    RF_WriteReg();							//  3 retrans... 	
	RF_RegAddr = EN_AA;
	RF_RegData = 0x01;
    RF_WriteReg();						// Enable Auto.Ack:Pipe0  	
#elseif(TRANSMIT_TYPE == TRANS_BURST_MODE)    
	RF_RegAddr = SETUP_RETR;
	RF_RegData = 0x00;
    RF_WriteReg();							// Disable retrans... 	
	RF_RegAddr = EN_AA;
	RF_RegData = 0x00;
    RF_WriteReg();							// Disable AutoAck 
#endif
}

/******************************************************************************/
//            		进入载波模式
/******************************************************************************/
/*
void RF_Carrier(void)
{
	RF_RegAddr = RST_FSPI;
	RF_RegData = 0x5A;
	RF_WriteReg();								//Software Reset    
	RF_RegAddr = RST_FSPI;
	RF_RegData = 0XA5;
	RF_WriteReg();
	RF_RegAddr = FEATURE;
	RF_RegData = 0x20;
	RF_WriteReg();
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
*/




/***************************************end of file ************************************/












