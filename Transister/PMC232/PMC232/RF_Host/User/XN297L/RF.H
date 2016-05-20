//============================================================================//
//  * @file           RF.H
//  * @author         Lin YuLiang
//  * @version        V1.0
//  * @date           18/3/2016
//  * @brief          RF XN297L communication interface
//  * @modify user:   Lin YuLiang
//  * @modify date:   18/3/2016
//============================================================================//
#ifndef   _RF_H_
#define   _RF_H_

///////////////////////////////////////////////////////////////////////////////
//                  根据 不同的单片机，以下部分可能需要修改                       //
///////////////////////////////////////////////////////////////////////////////
/******************** Control I/O *****************************/
#include <stdint.h>

#define         CSN_HIGH                PC.0 = 1
#define         CSN_LOW                 PC.0 = 0
#define         SCK_HIGH                PC.5 = 1
#define         SCK_LOW                 PC.5 = 0

#define         SPI_DATA_HIGH           PA.4 = 1
#define         SPI_DATA_LOW            PA.4 = 0
#define         SPI_DATA_STATUS         PA.4
#define         SPI_DATA_OUTPUT_MODE	PAC.4 = 1
#define         SPI_DATA_INPUT_MODE		PAC.4 = 0
#define         CE_HIGH                 RF_RegAddr = SPI_COMMAND_CE_FSPI_ON; \
										RF_RegData = 0; \
										RF_WriteReg()

#define         CE_LOW					RF_RegAddr = SPI_COMMAND_CE_FSPI_OFF; \
										RF_RegData = 0; \
										RF_WriteReg()
////////////////////////////////////////////////////////////////////////////////
//                    以下为声明部分，不建议修改                              //
////////////////////////////////////////////////////////////////////////////////

#define		MAX_PAYLOAD_LENG										64
#define		AW_3Byte												0x01
#define		AW_4Byte												0x02
#define		AW_5Byte												0x03
#define		RF_AddressWith											AW_4Byte
/********************SPI  REGISTER  ********************/
#define		SPI_COMMAND_R_REGISTER							0x00                            //SPI read RF data
#define		SPI_COMMAND_W_REGISTER							0x20                            //SPI write RF data
#define		SPI_COMMAND_R_RX_PAYLOAD		        0x61                            //Read RX Payload
#define		SPI_COMMAND_W_TX_PAYLOAD		        0xA0                            //Write TX Payload
#define		SPI_COMMAND_FLUSH_TX								0xE1                            //Flush RX FIFO
#define		SPI_COMMAND_FLUSH_RX								0xE2                            //Flush TX FIFO
#define		SPI_COMMAND_REUSE_TX_PL							0xE3                            //Reuse TX Payload
#define		SPI_COMMAND_ACTIVATE								0x50                            //ACTIVATE
#define		SPI_COMMAND_DEACTIVATE							0x50                            //DEACTIVATE
#define		SPI_COMMAND_R_RX_PL_WID							0x60                            //Read width of RX data 
#define		SPI_COMMAND_W_ACK_PAYLOAD		        0xA8                            //Data with ACK
#define		SPI_COMMAND_W_TX_PAYLOAD_NOACK	    0xB0                            //TX Payload no ACK Request
#define		SPI_COMMAND_CE_FSPI_ON	            0xFD                            // CE HIGH
#define		SPI_COMMAND_CE_FSPI_OFF	            0xFC                            // CE LOW
#define		SPI_COMMAND_RST_FSPI	              0x53                            // RESET
#define		SPI_COMMAND_NOP_N										0xFF
             
/******************CONTROL  REGISTER*******************/
#define		CONFIG                          0x00            
#define		EN_AA				0x01
#define		EN_RXADDR			0x02
#define		SETUP_AW			0x03
#define		SETUP_RETR			0x04
#define		RF_CH				0x05
#define		RF_SETUP			0x06
#define		STATUS				0x07
#define		OBSERVE_TX			0x08
#define		DATAOUT			        0x09
#define		RX_ADDR_P0			0x0A
#define		RX_ADDR_P1			0x0B
#define		RX_ADDR_P2			0x0C
#define		RX_ADDR_P3			0x0D
#define		RX_ADDR_P4			0x0E
#define		RX_ADDR_P5			0x0F
#define		TX_ADDR				0x10
#define		RX_PW_P0			0x11
#define		RX_PW_P1			0x12
#define		RX_PW_P2			0x13
#define		RX_PW_P3			0x14
#define		RX_PW_P4			0x15
#define		RX_PW_P5			0x16
#define		FIFO_STATUS			0x17
#define		DEM_CAL				0x19
#define     RF_CAL2				0x1A
#define     DEM_CAL2			0x1B
#define		DYNPD				0x1C
#define		FEATURE				0x1D	
#define		RF_CAL				0x1E
#define		BB_CAL				0x1F

/////////////////////////////////////////////////////////////////////////////////////
/*************************CONTROL CMD***********************************************/
#define         RF13dBm                         0x3F                            // 13dBm  发射档位功率设置
#define         RF10dBm                         0X0F                            // 10dBm 
#define         RF8dBm                          0x15                            // 8dbm      
#define         RF7dBm                          0x07                            // 7dbm   
#define         RF5dBm                          0x2c                            // 5dbm   
#define         RF4dBm                          0x06                            // 4dbm   
#define         RF2dBm                          0x05                            // 2dbm  
#define         RF0dBm                          0X0B                            // 0dBm  
#define         RF_3dBm                         0x04                            // -3dBm     
#define         RF_6dBm                         0x0A                            // -6dBm 
#define         RF_10dBm                        0x02                            // -10dBm 
#define         RF_18dBm                        0x01                            // -18dBm 
#define         RF_30dBm                        0x00                            // -30dBm 

#define         DR_1M                           0X00				//通信速率 1Mbps
#define         DR_2M                           0X40				//通信速率 2Mbps
#define         DR_250K                         0XC0                             //通信速率 250Kbps
#define         RX_DR_FLAG                      0X40                            // 接收中断标志位
#define         TX_DS_FLAG                      0X20                            // 发送完成中断标志位
#define         RX_TX_CMP_FLAG                  0X60                            // 发送接收完成中断标志位，ack_payload 模式下使用
#define         MAX_RT_FLAG                     0X10                            // 发送重传超时中断标志位
#define         TRANS_ENHANCE_MODE              1                               //传输类型增强型
#define         TRANS_BURST_MODE                2                               //传输类型普通型
///////////////////////////////////////////////////////////////////////////////
//                  根据用户应用，以下部分可能需要修改                       //
///////////////////////////////////////////////////////////////////////////////

/*
typedef enum
{
	Pipe2 = 0,
	Pipe3 = 1,
	Pipe4 = 2,
	Pipe5 = 3,
}PipeNumberDef;
*/


extern uint16_t pRF_TxAddr,pRF_RxAddr;
extern uint8_t	RF_Channel,RF_Status,RF_RSSI;

extern uint8_t	RF_Leng;
extern uint16_t pRF_Data;

extern uint8_t SPI_Data;
extern uint8_t RF_RegAddr,RF_RegData;

/******************* Function declare *******************/
void	SPI_WW(void);
void	SPI_WR(void);
void	ucSPI_Read(void);
void	RF_WriteReg(void);
void	ucRF_ReadReg(void);
//void	RF_WriteRegWitchBytes(void);
//void	RF_ReadRegWitchBytes(void);
void	RF_WriteFIFO(void);
void	RF_ReadFIFO(void);
void	RF_ClearFIFO(void);
void	ucRF_GetStatus(void);
//void	ucRF_GetRSSI(void);
void	RF_ClearStatus(void);
void	RF_SetChannel(void);
void	RF_SetRxP0Addr(void);
//void	RF_SetRxP1Addr(void);
void	RF_SetTxAddr(void);
void	RF_TxMode(void);
void	RF_RxMode(void);
void	RF_TxData(void);
void	ucRF_DumpRxData(void);
void	RF_SetCal(void);
void	RF_Init(void);
//void	RF_Carrier(void);



/******************Parameter define **********************/
        

#define         DEFAULT_CHANNEL                0				//初始化时的频率： 2400 MHz           
#define         PAYLOAD_WIDTH                  16				//Payload宽度：     14bytes				
#define         TRANSMIT_TYPE                  TRANS_BURST_MODE 		//使用普通型模式           TRANS_BURST_MODE    TRANS_ENHANCE_MODE
#define         DATA_RATE                      DR_1M                           //通信速率1Mbps            DR_2M    DR_1M  DR_250K
#define         RF_POWER                      (RF8dBm |DATA_RATE)		//发射功率13dBm 
#endif
/**************************************************************/



/****************************************end of file********************************************/




