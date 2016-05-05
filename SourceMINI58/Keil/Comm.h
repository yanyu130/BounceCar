#ifndef _COMM_H_
#define _COMM_H_

#include "def.h"
#include <stdint.h>
#include "RF.h"

//数据包长度PKT_LENG为14Byte
// |包头|Host地址|Device地址|PKT_TYPE|PKT_DATA|CRC|
// 包头        ->  1个Byte 0x55
// Host地址    ->  4个Byte主机地址
// Device地址  ->  4个Byte从机地址
// PKT_TYPE    ->  1个Byte包类型
// PKT_DATA    ->  3个Byte包数据
// CRC         ->  1个字节CRC校验码
// CRC = (0xAA + Byte0 + ... + Byte13) % 256
#define PKT_LENG						PAYLOAD_WIDTH
#define PKT_HEADER						0x55
#define PKT_TYPE_CONNECT_NORMAL			0x01
#define PKT_TYPE_CONNECT_FAST			0x02
#define PKT_TYPE_CONTROL				0x03
#define PKT_TYPE_ACK					0x04
#define PKT_TYPE_CHECK_ONLINE			0x05
#define PKT_DATA_FAST_LINK_HOST			0x01
#define PKT_DATA_FAST_LINK_DEVICE		0x02



#define Comm_NewData_Mask							0x01
#define Comm_DataReceived_Mask				0x02
#define Comm_TimeoutEnBit_Mask				0x04

	
#define STATUS_DISCONNECT					0x00
#define	STATUS_CONNECTING					0x01
#define STATUS_CONNECTED					0x02
#define STATUS_SLEEP						0x03
#define STATUS_LOST							0x04

#define COMM_TIMEOUT_BIND					100			//配对阶段超时时间，单位Ms
#define COMM_TIMEOUT_BIND_LOADVAL						((COMM_TIMEOUT_BIND*SYSTEM_TICK_FREQ)/1000)
#define COMM_TIMEOUT_LOST					400			//正常通讯过程超时时间，单位Ms
#define COMM_TIMEOUT_SENDDATA_LOST					((COMM_TIMEOUT_LOST*SYSTEM_TICK_FREQ)/1000)
#define COMM_TIMEOUT_FAST_LINK		1000			//上电检测快速链接包超时时间，单位Ms
#define COMM_TIMEOUT_FAST_LINK_LOADVAL			((COMM_TIMEOUT_FAST_LINK*SYSTEM_TICK_FREQ)/1000)



//定义RF发射、接收地址宽度
#if (RF_AddressWith == AW_5Byte)
	#define HOST_ADDR_LENG					5
	#define DEVICE_ADDR_LENG				5
	#define CURRENT_ADDR_LENG				5
#elif (RF_AddressWith == AW_3Byte)
	#define HOST_ADDR_LENG					3
	#define DEVICE_ADDR_LENG				3
	#define CURRENT_ADDR_LENG				3
#else
	#define HOST_ADDR_LENG					4
	#define DEVICE_ADDR_LENG				4
	#define CURRENT_ADDR_LENG				4
#endif




//配对期间，使用公共地址和频率
#define BIND_ADDR							0xEEEEEEEE
#define BIND_ADDR_LENG						ADDR_LENG_4BYTE
#define	BIND_CHANNEL						0x00
	

#define SET_DATARATE_1MBPS					0x00
#define SET_DATARATE_250KBPS				0xC0





extern uint8_t	Comm_Flag;				
extern uint8_t Comm_Data[5];
extern uint16_t Comm_Timeout;
extern uint8_t Comm_CurrentStatus;


void Comm_Init							(void);
void Comm_Process						(void);
void Comm_SetToReceive			(void);
#endif



