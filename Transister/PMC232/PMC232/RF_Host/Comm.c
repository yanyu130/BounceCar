#include "Comm.h"



uint8_t	Comm_Flag;						//通讯模块标志位
uint8_t Comm_CurrentStatus;				//通讯模块当前状态
uint8_t Comm_CurrentChannel;				//当前通讯频道
uint32_t Comm_CurrentAddr;				//当前通讯地址
uint32_t Comm_HostAddr;					//主机地址
uint32_t Comm_DevAddr;					//从机地址
uint8_t Comm_SendDataBuf[PKT_LENG];		//RF发射数据缓冲区
uint8_t Comm_RecDataBuf[PKT_LENG];		//RF接收数据缓冲区
uint8_t Comm_Timeout;					//超时计数器
uint8_t Comm_RetryCount;				//重试计数器

uint8_t Comm_ErrCode;					//上层调用发送数据接口返回的错误代码
uint8_t Comm_Data[5];					//上层填充需要发送的数据
uint8_t Comm_Buf[5];
uint8_t Comm_LastData[5];				//保持上次发送的数据，相同数据不立即发送
uint8_t Comm_SameDataDelay;

uint16_t Comm_TimerTick;

/********************************************************************
  通讯模块初始化函数
********************************************************************/
void Comm_Init(void)
{
	//取芯片滚动码作为本身地址，主机的本身地址则是Host地址
	WORD    Point;
	Point = _SYS (ADR.ROLL);
	Comm_HostAddr$0 = *Point$L;
	Comm_HostAddr$1 = *Point$H;
	Point++;
	Comm_HostAddr$2 = *Point$L;
	Comm_HostAddr$3 = 0;//*Point$H;

//	Comm_HostAddr = 0xaabbccdd;


	RF_Init();				//初始化2.4G模块

	Comm_Timeout = 0;
	Comm_RetryCount = 0;
	Comm_Flag = 0;
	Comm_CurrentChannel = 0;
	Comm_CurrentAddr = 0;
	Comm_DevAddr = 0;
	Comm_CurrentStatus = 0;
	Comm_ErrCode = 0;
	Comm_Data[0] = 0;
	Comm_Data[1] = 0;
	Comm_Data[2] = 0;
	Comm_Data[3] = 0;
	Comm_Data[4] = 0;
	Comm_LastData[0] = 0;
	Comm_LastData[1] = 0;
	Comm_LastData[2] = 0;
	Comm_LastData[3] = 0;
	Comm_LastData[4] = 0;
	Comm_SameDataDelay = 0;
}
/*-------------------------------------------------------------------
-------------------------------------------------------------------*/



/********************************************************************
  通讯模块发送数据包函数
********************************************************************/
void Comm_SendPacket(void)
{
	Comm_SendDataBuf[0] = PKT_HEADER;
	//HostAddr
	Comm_SendDataBuf[1] = Comm_HostAddr$0;
	Comm_SendDataBuf[2] =  Comm_HostAddr$1;
	Comm_SendDataBuf[3] =  Comm_HostAddr$2;
	Comm_SendDataBuf[4] =  Comm_HostAddr$3;
	//DecAddr
	Comm_SendDataBuf[5] = Comm_DevAddr$0;
	Comm_SendDataBuf[6] = Comm_DevAddr$1;
	Comm_SendDataBuf[7] = Comm_DevAddr$2;
	Comm_SendDataBuf[8] = Comm_DevAddr$3;
	//ComputeSendDataCRC
	a = 0xAA;
	a = a + Comm_SendDataBuf[0];
	a = a + Comm_SendDataBuf[1];
	a = a + Comm_SendDataBuf[2];
	a = a + Comm_SendDataBuf[3];
	a = a + Comm_SendDataBuf[4];
	a = a + Comm_SendDataBuf[5];
	a = a + Comm_SendDataBuf[6];
	a = a + Comm_SendDataBuf[7];
	a = a + Comm_SendDataBuf[8];
	a = a + Comm_SendDataBuf[9];
	a = a + Comm_SendDataBuf[10];
	a = a + Comm_SendDataBuf[11];
	a = a + Comm_SendDataBuf[12];
	a = a + Comm_SendDataBuf[13];
	a = a + Comm_SendDataBuf[14];
	Comm_SendDataBuf[15] = a;
	////////////////////
	pRF_TxAddr = &Comm_CurrentAddr;
	RF_Leng = CURRENT_ADDR_LENG;
	RF_SetTxAddr();
	RF_Channel = Comm_CurrentChannel;
	RF_SetChannel();
	RF_TxMode();	//设置RF进入发射模式

	pRF_Data = Comm_SendDataBuf;
	RF_Leng = PKT_LENG;
	RF_TxData();
}
/*-------------------------------------------------------------------
-------------------------------------------------------------------*/


/********************************************************************
  通讯模块清空接收数据缓冲区函数
********************************************************************/
void Comm_ClrRecDataBuf(void)
{
	Comm_RecDataBuf[0] = 0;
	Comm_RecDataBuf[1] = 0;
	Comm_RecDataBuf[2] = 0;
	Comm_RecDataBuf[3] = 0;
	Comm_RecDataBuf[4] = 0;
	Comm_RecDataBuf[5] = 0;
	Comm_RecDataBuf[6] = 0;
	Comm_RecDataBuf[7] = 0;
	Comm_RecDataBuf[8] = 0;
	Comm_RecDataBuf[9] = 0;
	Comm_RecDataBuf[10] = 0;
	Comm_RecDataBuf[11] = 0;
	Comm_RecDataBuf[12] = 0;
	Comm_RecDataBuf[13] = 0;
	Comm_RecDataBuf[14] = 0;
	Comm_RecDataBuf[15] = 0;
}
/*-------------------------------------------------------------------
-------------------------------------------------------------------*/


void Comm_SetToReceive(void)
{
	pRF_RxAddr = &Comm_CurrentAddr;
	RF_Leng = CURRENT_ADDR_LENG;
	RF_SetRxP0Addr();
	RF_Channel = Comm_CurrentChannel;
	RF_SetChannel();
	Comm_ClrRecDataBuf();
	RF_RxMode();
}





void Comm_GetChannel(void)
{
	a = (Comm_HostAddr & 0x3F) + 3;
}



void Comm_CheckDataReceived(void)
{
	Comm_DataReceived = false;
	pRF_Data = Comm_RecDataBuf;
	RF_Leng = PKT_LENG;
	ucRF_DumpRxData();
	if(RF_Status)
	{
		//接收到数据，检查包头是否有效
		if(Comm_RecDataBuf[0] == PKT_HEADER)
		{
			//收到有效的数据包头
			//检查CRC是否正确
			a = 0xAA;
			a = a + Comm_RecDataBuf[0];
			a = a + Comm_RecDataBuf[1];
			a = a + Comm_RecDataBuf[2];
			a = a + Comm_RecDataBuf[3];
			a = a + Comm_RecDataBuf[4];
			a = a + Comm_RecDataBuf[5];
			a = a + Comm_RecDataBuf[6];
			a = a + Comm_RecDataBuf[7];
			a = a + Comm_RecDataBuf[8];
			a = a + Comm_RecDataBuf[9];
			a = a + Comm_RecDataBuf[10];
			a = a + Comm_RecDataBuf[11];
			a = a + Comm_RecDataBuf[12];
			a = a + Comm_RecDataBuf[13];
			a = a + Comm_RecDataBuf[14];
			if(Comm_RecDataBuf[15] == a)
			{
				Comm_DataReceived = true;
			}
			else
			{
				//CRC校验出错
				//清除接收缓冲区数据重新接收数据包
				Comm_ClrRecDataBuf();
			}
		}
		else
		{
			//收到无效的数据包头
			//清除接收缓冲区数据重新接收数据包
			Comm_ClrRecDataBuf();
		}
	}
}
/*-------------------------------------------------------------------
-------------------------------------------------------------------*/



/*******************************************************************
 数据发送接口
*******************************************************************/
void Comm_SendData(void)
{
	switch(Comm_CurrentStatus)
	{
		case STATUS_CONNECTED:
		{
			if(Comm_SendDataBit == false)
			{
				//当前没有在发数据
				if(Comm_LastData[0] == Comm_Data[0] && \
					Comm_LastData[1] == Comm_Data[1] && \
					Comm_LastData[2] == Comm_Data[2] && \
					Comm_LastData[3] == Comm_Data[3] && \
					Comm_LastData[4] == Comm_Data[4]) 
				{
					if(Comm_SameDataDelay == 0)
					{
						Comm_SendDataBit = true;
						Comm_Buf[0] = Comm_Data[0];
						Comm_Buf[1] = Comm_Data[1];
						Comm_Buf[2] = Comm_Data[2];
						Comm_Buf[3] = Comm_Data[3];
						Comm_Buf[4] = Comm_Data[4];
					}
				}
				else
				{
					Comm_LastData[0] = Comm_Data[0];
					Comm_LastData[1] = Comm_Data[1];
					Comm_LastData[2] = Comm_Data[2];
					Comm_LastData[3] = Comm_Data[3];
					Comm_LastData[4] = Comm_Data[4];
					//重载时间
					Comm_SameDataDelay = COMM_TIME_SAME_DATA_DELAY_LOADVAL;
					Comm_SendDataBit = true;
					Comm_Buf[0] = Comm_Data[0];
					Comm_Buf[1] = Comm_Data[1];
					Comm_Buf[2] = Comm_Data[2];
					Comm_Buf[3] = Comm_Data[3];
					Comm_Buf[4] = Comm_Data[4];
				}
				Comm_ErrCode = ERR_CODE_SUCCESS;
			}
			else
			{
				Comm_ErrCode = ERR_CODE_BUSY;
			}
			break;
		}
		case STATUS_DISCONNECT:
		case STATUS_CONNECTING:
		case STATUS_LOST:
		default:
		{
			Comm_ErrCode = ERR_CODE_OFFLINE;
			break;
		}
	}
}
/*-------------------------------------------------------------------
-------------------------------------------------------------------*/

void Comm_TimerProcess(void)
{
	uint8_t temp = 0;
	if(Comm_TimerTick != SysTick)
	{
		if(Comm_TimerTick < SysTick)
		{
			temp = SysTick - Comm_TimerTick;
			Comm_TimerTick = SysTick;
		}
		else if(Comm_TimerTick > SysTick)
		{
			Comm_TimerTick = ~Comm_TimerTick+1;//Comm_TimerTick = 0xFFFFFFFF - Comm_TimerTick;
			temp = Comm_TimerTick;
			temp += SysTick;
			Comm_TimerTick = SysTick;
		}

		if(Comm_SameDataDelay > temp) Comm_SameDataDelay -= temp;
		else Comm_SameDataDelay = 0;

		if(Comm_TimeoutEnBit == true)
		{
			if(Comm_Timeout > temp) Comm_Timeout -= temp;
			else Comm_Timeout = 0;
		}
	}
}


/*
主机进入休眠需要发信号通知从机，在休眠状态，主机发送询问从机在线的
数据包间隔延长到10S。正常通讯的查询包间隔100Ms
*/
/********************************************************************
  通讯模块主循环
********************************************************************/
void Comm_Process(void)
{
	Comm_TimerProcess();
	switch(Comm_CurrentStatus)
	{
		case STATUS_DISCONNECT:
		{
			//无连接状态，发送快速链接请求。
			//主机突然断电后重新上电，通过发送快速链接请求包实现与
			//之前配对的从机快速建立连接
			if(Comm_Timeout == 0)
			{
				Comm_RetryCount++;
				if(Comm_RetryCount <= COMM_MAX_RETRY_FAST_LINK)
				{
					//接收超时，重新发送一次快速链接请求包
					//PKT_TYPE
					Comm_SendDataBuf[9] = PKT_TYPE_CONNECT_FAST;
					//快速链接包数据域第一个字节表示发送该包数据的角色
					Comm_SendDataBuf[10] = PKT_DATA_FAST_LINK_HOST;
					//快速链接包数据域第二个字节表示建立连接后使用的频率
					Comm_GetChannel();
					Comm_SendDataBuf[11] = a;	//GetChannel 将通道号保存到a中
					Comm_SendDataBuf[12] = 0;
					Comm_SendDataBuf[13] = 0;
					Comm_SendDataBuf[14] = 0;
					Comm_CurrentAddr = BIND_ADDR;
					Comm_CurrentChannel = BIND_CHANNEL;
					Comm_SendPacket();

					Comm_CurrentAddr = BIND_ADDR;
					Comm_CurrentChannel = BIND_CHANNEL;
					Comm_SetToReceive();
					Comm_Timeout = COMM_TIMEOUT_FAST_LINK_LOADVAL;
					Comm_TimeoutEnBit = true;
				}
				else
				{
					//快速链接超过最大重试次数
					//进入常规配对过程
					Comm_RetryCount = 0;
					Comm_Timeout = 0;
					Comm_TimeoutEnBit = false;
					Comm_CurrentStatus = STATUS_CONNECTING;
				}
			}
			else
			{
				Comm_CheckDataReceived();
				if(Comm_DataReceived == True)
				{
					if(Comm_RecDataBuf[9] == PKT_TYPE_CONNECT_FAST)
					{
						if(Comm_RecDataBuf[10] == PKT_DATA_FAST_LINK_DEVICE)
						{
							//收到来自从机返回的快速链接请求的应答
							Comm_DevAddr$0 = Comm_RecDataBuf[5];
							Comm_DevAddr$1 = Comm_RecDataBuf[6];
							Comm_DevAddr$2 = Comm_RecDataBuf[7];
							Comm_DevAddr$3 = Comm_RecDataBuf[8];
							
							Comm_CurrentAddr = Comm_DevAddr;
							Comm_GetChannel();
							Comm_CurrentChannel = a;
							Comm_SetToReceive();
							Comm_RetryCount = 0;
							Comm_SendDataBit = false;
							Comm_WaitACK = false;
							Comm_Timeout = COMM_TIMEOUT_CHECK_ONLINE_LOADVAL;
							Comm_TimeoutEnBit = true;
							Comm_CurrentStatus = STATUS_CONNECTED;
						}
						//else
						//{
						//	call	Comm_ClrRecDataBuf
						//}
					}
					//else
					//{
					//	call	Comm_ClrRecDataBuf
					//}
					Comm_DataReceived = false;
					Comm_ClrRecDataBuf();
				}				
			}
			break;
		}
		case STATUS_CONNECTING:
		{
			//正在尝试连接
			if(Comm_Timeout == 0)
			{
				//接收超时，重新发送一次链接请求包
				Comm_DevAddr = 0;
				//PKT_TYPE
				Comm_SendDataBuf[9] = PKT_TYPE_CONNECT_NORMAL;
				//配对阶段1
				Comm_SendDataBuf[10] = 1;
				Comm_SendDataBuf[11] = 0;
				Comm_SendDataBuf[12] = 0;
				Comm_SendDataBuf[13] = 0;
				Comm_SendDataBuf[14] = 0;
				Comm_CurrentAddr = BIND_ADDR;
				Comm_CurrentChannel = BIND_CHANNEL;
				Comm_SendPacket();

				Comm_CurrentAddr = BIND_ADDR;
				Comm_CurrentChannel = BIND_CHANNEL;
				Comm_SetToReceive();
				Comm_Timeout = COMM_TIMEOUT_BIND_LOADVAL;
				Comm_TimeoutEnBit = true;
			}
			else
			{
				Comm_CheckDataReceived();
				if(Comm_DataReceived == True)
				{
					if(Comm_RecDataBuf[9] == PKT_TYPE_CONNECT_NORMAL)
					{
						//配对过程中只接受连接类型的数据包
						//配对类型的数据包，包类型的下一个字节
						//表示配对进行到哪个阶段
						switch(Comm_RecDataBuf[10])
						{
							case 2:
							{
								//保存从机地址
								Comm_DevAddr$0 = Comm_RecDataBuf[5];
								Comm_DevAddr$1 = Comm_RecDataBuf[6];
								Comm_DevAddr$2 = Comm_RecDataBuf[7];
								Comm_DevAddr$3 = Comm_RecDataBuf[8];
								//PKT_TYPE
								Comm_SendDataBuf[9] = PKT_TYPE_CONNECT_NORMAL;
								//配对阶段3
								Comm_SendDataBuf[10] = 3;
								Comm_GetChannel();
								Comm_SendDataBuf[11] = a;
								Comm_SendDataBuf[12] = 0;
								Comm_SendDataBuf[13] = 0;
								Comm_SendDataBuf[14] = 0;
								Comm_CurrentAddr = BIND_ADDR;
								Comm_CurrentChannel = BIND_CHANNEL;
								Comm_SendPacket();
								DelayTime = 3;
								DelayMs();
								//PKT_TYPE
								Comm_SendDataBuf[9] = PKT_TYPE_CONNECT_NORMAL;
								//配对阶段4
								Comm_SendDataBuf[10] = 4;
								Comm_GetChannel();
								Comm_CurrentChannel = a;
								Comm_SendDataBuf[11] = Comm_CurrentChannel;
								Comm_SendDataBuf[12] = 0;
								Comm_SendDataBuf[13] = 0;
								Comm_SendDataBuf[14] = 0;
								Comm_CurrentAddr = Comm_DevAddr;
								Comm_SendPacket();

								Comm_CurrentAddr = Comm_HostAddr;
								Comm_SetToReceive();
								//Set Timeout
								Comm_Timeout = COMM_TIMEOUT_BIND_LOADVAL;
								Comm_TimeoutEnBit = true;
								break;
							}
							case 5:
							{
								//设置查询从机在线超时时间
								Comm_TimeoutEnBit = true;
								Comm_Timeout = COMM_TIMEOUT_CHECK_ONLINE_LOADVAL;
								Comm_SendDataBit = false;
								//设置连接状态为连接成功
								Comm_CurrentStatus = STATUS_CONNECTED;
								break;
							}
						}
					}
					Comm_DataReceived = false;
					Comm_ClrRecDataBuf();
				}
			}
			break;
		}
		case STATUS_CONNECTED:
		{
			//已经连接上从机
			if(Comm_WaitACK == true)
			{
				//正在等待从机响应
				Comm_CheckDataReceived();
				if(Comm_DataReceived == True)
				{
					if(Comm_RecDataBuf[9] == PKT_TYPE_ACK)
					{
						//接收到从机的回应
						//重新加载查询从机在线超时时间
						Comm_Timeout = COMM_TIMEOUT_CHECK_ONLINE_LOADVAL;
						Comm_TimeoutEnBit = true;
						//清除发送数据标志位，表示数据发送成功
						Comm_SendDataBit = false;
						//
						Comm_WaitACK = false;
						Comm_RetryCount = 0;
					}
					Comm_DataReceived = false;
					Comm_ClrRecDataBuf();
				}
				else
				{
					if(Comm_TimeoutEnBit == true)
					{
						if(Comm_Timeout == 0)
						{
							//超时没有接收到从机回应
							if(Comm_RetryCount >= COMM_MAX_RETRY_SENDDATA)
							{
								//超过最大数据重发次数
								//设置状态为丢失
								Comm_CurrentStatus = STATUS_LOST;
								Comm_Timeout = 0;
								Comm_TimeoutEnBit = true;
								Comm_RetryCount = 0;
								Comm_WaitACK = false;
							}
							else
							{
								//还没超过最大重发数据次数
								//重发上次数据包
								Comm_CurrentAddr = Comm_DevAddr;
								Comm_SendPacket();
								Comm_RetryCount++;
								//重新接收从机响应
								Comm_CurrentAddr = Comm_HostAddr;
								Comm_SetToReceive();
								Comm_Timeout = COMM_TIMEOUT_SENDDATA_LOADVAL;
								Comm_TimeoutEnBit = true;
								Comm_WaitACK = true;
							}
						}
					}
				}
			}
			else
			{
				//没有在等待从机响应
				if(Comm_SendDataBit == true)
				{
					//有新数据需要发送
					//PKT_TYPE
					Comm_SendDataBuf[9] = PKT_TYPE_CONTROL;
					//Ctrl data
					Comm_SendDataBuf[10] = Comm_Buf[0];
					Comm_SendDataBuf[11] = Comm_Buf[1];
					Comm_SendDataBuf[12] = Comm_Buf[2];
					Comm_SendDataBuf[13] = Comm_Buf[3];
					Comm_SendDataBuf[14] = Comm_Buf[4];
					Comm_CurrentAddr = Comm_DevAddr;
					Comm_SendPacket();
					//设置接收从机响应
					Comm_CurrentAddr = Comm_HostAddr;
					Comm_SetToReceive();
					Comm_Timeout = COMM_TIMEOUT_SENDDATA_LOADVAL;
					Comm_TimeoutEnBit = true;
					Comm_WaitACK = true;
				}
				else
				{
					//无新数据需要发送
					if(Comm_TimeoutEnBit == true)
					{
						if(Comm_Timeout == 0)
						{
							//超时，需要发送询问从机在线数据包
							//PKT_TYPE
							Comm_SendDataBuf[9] = PKT_TYPE_CHECK_ONLINE;
							//Ctrl data
							Comm_SendDataBuf[10] = 0;
							Comm_SendDataBuf[11] = 0;
							Comm_SendDataBuf[12] = 0;
							Comm_SendDataBuf[13] = 0;
							Comm_SendDataBuf[14] = 0;
							Comm_CurrentAddr = Comm_DevAddr;
							Comm_SendPacket();
							//设置接收从机响应
							Comm_CurrentAddr = Comm_HostAddr;
							Comm_SetToReceive();
							Comm_Timeout = COMM_TIMEOUT_SENDDATA_LOADVAL;
							Comm_TimeoutEnBit = true;
							Comm_WaitACK = true;
						}
					}
				}
			}
			break;
		}
//		case STATUS_SLEEP:
//		{
//			//主机进入睡眠模式
//
//			break;
//		}
		case STATUS_LOST:
		{
			//与从机连接中断
			if(Comm_TimeoutEnBit == true)
			{
				if(Comm_Timeout == 0)
				{
					//PKT_TYPE
					Comm_SendDataBuf[9] = PKT_TYPE_CONNECT_FAST;
					//fast connect data
					//快速链接包数据域第一个字节表示发送该包数据的角色
					Comm_SendDataBuf[10] = PKT_DATA_FAST_LINK_HOST;
					//快速链接包数据域第二个字节表示建立连接后使用的频率
					Comm_GetChannel();
					Comm_SendDataBuf[11] = a;
					Comm_SendDataBuf[12] = 0;
					Comm_SendDataBuf[13] = 0;
					Comm_SendDataBuf[14] = 0;
					Comm_CurrentChannel = BIND_CHANNEL;
					Comm_CurrentAddr = BIND_ADDR;
					Comm_SendPacket();

					//设置接收从机响应
					Comm_SetToReceive();
					Comm_Timeout = COMM_TIMEOUT_FAST_LINK_LOADVAL;
					Comm_TimeoutEnBit = true;
				}
				else
				{
					Comm_CheckDataReceived();
					if(Comm_DataReceived == True)
					{
						if(Comm_RecDataBuf[9] == PKT_TYPE_CONNECT_FAST)
						{
							if(Comm_RecDataBuf[10] == PKT_DATA_FAST_LINK_DEVICE)
							{
								//收到来自从机返回的快速链接请求的应答
								Comm_CurrentAddr = Comm_DevAddr;
								Comm_GetChannel();
								Comm_CurrentChannel = a;
								Comm_SetToReceive();
								Comm_RetryCount = 0;
								Comm_WaitACK = false;
								Comm_SendDataBit = false;
								Comm_Timeout = COMM_TIMEOUT_CHECK_ONLINE_LOADVAL;
								Comm_TimeoutEnBit = true;
								Comm_CurrentStatus = STATUS_CONNECTED;
							}
						}
						Comm_DataReceived = false;
						Comm_ClrRecDataBuf();
					}
				}

			}

			break;
		}
		default:
		{
			//其他未定义状态

			break;
		}
	}
}
/*-------------------------------------------------------------------
-------------------------------------------------------------------*/

