#ifndef _COMM_H_
#define _COMM_H_

#include "def.h"
#include <stdint.h>
#include "RF.h"

//���ݰ�����PKT_LENGΪ14Byte
// |��ͷ|Host��ַ|Device��ַ|PKT_TYPE|PKT_DATA|CRC|
// ��ͷ        ->  1��Byte 0x55
// Host��ַ    ->  4��Byte������ַ
// Device��ַ  ->  4��Byte�ӻ���ַ
// PKT_TYPE    ->  1��Byte������
// PKT_DATA    ->  3��Byte������
// CRC         ->  1���ֽ�CRCУ����
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

#define COMM_TIMEOUT_BIND					100			//��Խ׶γ�ʱʱ�䣬��λMs
#define COMM_TIMEOUT_BIND_LOADVAL						((COMM_TIMEOUT_BIND*SYSTEM_TICK_FREQ)/1000)
#define COMM_TIMEOUT_LOST					400			//����ͨѶ���̳�ʱʱ�䣬��λMs
#define COMM_TIMEOUT_SENDDATA_LOST					((COMM_TIMEOUT_LOST*SYSTEM_TICK_FREQ)/1000)
#define COMM_TIMEOUT_FAST_LINK		1000			//�ϵ���������Ӱ���ʱʱ�䣬��λMs
#define COMM_TIMEOUT_FAST_LINK_LOADVAL			((COMM_TIMEOUT_FAST_LINK*SYSTEM_TICK_FREQ)/1000)



//����RF���䡢���յ�ַ���
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




//����ڼ䣬ʹ�ù�����ַ��Ƶ��
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



