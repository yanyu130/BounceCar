#ifndef _COMM_H_
#define _COMM_H_

#include <stdint.h>
#include "User\XN297L\RF.H"
#include "Timer.h"

//数据包长度PKT_LENG为14Byte
// |包头|Host地址|Device地址|PKT_TYPE|PKT_DATA|CRC|
// 包头        ->  1个Byte 0x55
// Host地址    ->  4个Byte主机地址
// Device地址  ->  4个Byte从机地址
// PKT_TYPE    ->  1个Byte包类型
// PKT_DATA    ->  5个Byte包数据
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





#define Comm_SendDataBit	Comm_Flag.0
#define Comm_DataReceived	Comm_Flag.1
#define Comm_TimeoutEnBit	Comm_Flag.2
#define Comm_WaitACK		Comm_Flag.3


#define STATUS_DISCONNECT				0x00
#define	STATUS_CONNECTING				0x01
#define STATUS_CONNECTED				0x02
#define STATUS_SLEEP					0x03
#define STATUS_LOST						0x04


#define COMM_MAX_RETRY_FAST_LINK		10
#define COMM_MAX_RETRY_SENDDATA			10


//定义各种超时时间
#define COMM_TIMEOUT_BIND					20			//配对阶段超时时间，单位Ms
#define COMM_TIMEOUT_BIND_LOADVAL			((COMM_TIMEOUT_BIND*T16_RateHz)/1000)
#define COMM_TIMEOUT_SENDDATA				20			//正常通讯过程超时时间，单位Ms
#define COMM_TIMEOUT_SENDDATA_LOADVAL		((COMM_TIMEOUT_SENDDATA*T16_RateHz)/1000)
#define COMM_TIMEOUT_FAST_LINK				10			//发起快速链接请求后等待的超时时间，单位Ms
#define COMM_TIMEOUT_FAST_LINK_LOADVAL		((COMM_TIMEOUT_FAST_LINK*T16_RateHz)/1000)
#define COMM_TIMEOUT_CHECK_ONLINE			150			//查询从机是否在线时间间隔，单位Ms
#define COMM_TIMEOUT_CHECK_ONLINE_LOADVAL	((COMM_TIMEOUT_CHECK_ONLINE*T16_RateHz)/1000)
#define COMM_TIME_SAME_DATA_DELAY			50			//重复数据延时发送等待时间，单位Ms
#define COMM_TIME_SAME_DATA_DELAY_LOADVAL	((COMM_TIME_SAME_DATA_DELAY*T16_RateHz)/1000)




//定义RF发射、接收地址宽度
#if (RF_AddressWith == AW_5Byte)
	#define HOST_ADDR_LENG					5
	#define DEVICE_ADDR_LENG				5
	#define CURRENT_ADDR_LENG				5
#elseif (RF_AddressWith == AW_3Byte)
	#define HOST_ADDR_LENG					3
	#define DEVICE_ADDR_LENG				3
	#define CURRENT_ADDR_LENG				3
#else
	#define HOST_ADDR_LENG					4
	#define DEVICE_ADDR_LENG				4
	#define CURRENT_ADDR_LENG				4
#endif



//配对期间，使用公共地址和频率
#define BIND_ADDR						0xEEEEEEEE
#define BIND_ADDR_LENG					ADDR_LENG_4BYTE
#define	BIND_CHANNEL					0x00


#define SET_DATARATE_1MBPS				0x00
#define SET_DATARATE_250KBPS			0xC0

//定义错误代码
#define ERR_CODE_SUCCESS				0x00
#define ERR_CODE_OFFLINE				0x01
#define ERR_CODE_BUSY					0x02



extern uint8_t	Comm_Flag;
extern uint8_t Comm_ErrCode;					//上层调用发送数据接口返回的错误代码
extern uint8_t Comm_Data[5];					//上层填充需要发送的数据
extern uint8_t Comm_Timeout;						//超时计数器
extern uint8_t Comm_SameDataDelay;
extern uint8_t Comm_CurrentStatus;				//通讯模块当前状态

void Comm_Init						(void);
void Comm_SendData					(void);
void Comm_Process					(void);
#endif






































