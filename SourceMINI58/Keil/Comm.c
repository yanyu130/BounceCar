#include "Comm.h"
#include "Timer_Ctrl.h"

/*
  本机为从机，与主机建立连接后方可进行数据通讯。建立连接过程称为配对过程。
  在连接成功后，从机被动接收数据，收到主机数据后需要向主机返回ACK数据包确认。
  主机如无控制数据需要发送，则应该在不超过约定的超时时间内发送GETSTATUS类型
  数据包，如果超时，从机将将进入Lost状态。重新进行配对。
  配对过程：
      从机以公用地址和频率进行接收主机发送的配对请求，主机主动发起配对过程。
      配对阶段1：
          主机以公用频率向公用地址发送配对请求，请求数据包中包含该主机地址
      配对阶段2：
          从机收到配对请求后，以公用频率向公用地址回应主机配对请求，
          回应数据包中包含主机和从机地址。
      配对阶段3：
          主机收到从机回应数据后，再次以公用频率向公用地址发送确认配对数据包。
	  数据包中包含主机地址、从机地址、通讯频率。
      配对阶段4：
          从机接收到主机的确认数据包后，以主机发送的确认数据包中指定的通讯频率，
	  和地址进行接收数据。
      配对阶段5：
	  若接收到，则连接成功，并返回给主机确认数据；若超过一定时间未接收到数据，则连接不成功，重新进入配对阶段1。
  通讯过程：
      主机向从机发送控制数据，从机接收到正确的数据后，返回给主机ACK数据包，主机收到ACK数据包
      表示数据发送成功，否则发送失败，需要重发数据包。
  主机无控制数据：
      主机没有控制数据需要发送时，主机需要在约定的超时时间内发送GETSTATUS类型的数据包给从机，
      以便确认双方是否在线。
*/





uint8_t Comm_Flag;						//通讯模块标志位
uint8_t Comm_CurrentStatus;				//通讯模块当前状态
uint8_t Comm_CurrentChannel;				//当前通讯频道
uint32_t Comm_CurrentAddr;				//当前通讯地址
uint32_t Comm_HostAddr;					//主机地址
uint32_t Comm_DevAddr;					//从机地址
uint8_t Comm_SendDataBuf[PKT_LENG];		//RF发射数据缓冲区
uint8_t Comm_RecDataBuf[PKT_LENG];		//RF接收数据缓冲区
uint16_t Comm_Timeout;					//超时计数器
uint8_t Comm_Data[5];					//给上层访问的接收到的数据


uint32_t Comm_TimerTick;
/********************************************************************
  通讯模块初始化函数
********************************************************************/
void Comm_Init(void)
{

	//取芯片滚动码作为本身地址，从机的本身地址则是Device地址
/*
	call	_SYS(ADR.ROLL)
	Comm_DevAddr$0 = 0xaa//a
	call	_SYS(ADR.ROLL) + 1
	Comm_DevAddr$1 = 0xbb//a
	call	_SYS(ADR.ROLL) + 2
	Comm_DevAddr$2 = 0xcc//a
	Comm_DevAddr$3 = 0
*/

	Comm_DevAddr = 0x11223344;
	RF_Init();			//初始化2.4G基本寄存器
	
	
	Comm_Timeout = 0;
	Comm_Flag = 0;
	Comm_CurrentChannel = 0;
	Comm_CurrentAddr = 0;
	Comm_HostAddr = 0;
	Comm_CurrentStatus = 0;
	Comm_Data[0] = 0;
	Comm_Data[1] = 0;
	Comm_Data[2] = 0;
	Comm_Data[3] = 0;
	Comm_Data[4] = 0;
	
	Comm_CurrentAddr = BIND_ADDR;
	Comm_CurrentChannel = BIND_CHANNEL;
	Comm_SetToReceive();
	Comm_Timeout = COMM_TIMEOUT_FAST_LINK_LOADVAL;
	Comm_Flag |= Comm_TimeoutEnBit_Mask;
}
/*-------------------------------------------------------------------
-------------------------------------------------------------------*/



/********************************************************************
  通讯模块发送数据包函数
********************************************************************/
void Comm_SendPacket(void)
{
	uint8_t CRC,i;
	Comm_SendDataBuf[0] = PKT_HEADER;
	//HostAddr
	Comm_SendDataBuf[1] =  Comm_HostAddr&0xFF;
	Comm_SendDataBuf[2] =  (Comm_HostAddr&0xFF00)>>8;
	Comm_SendDataBuf[3] =  (Comm_HostAddr&0xFF0000)>>16;
	Comm_SendDataBuf[4] =  (Comm_HostAddr&0xFF000000)>>24;
	//DecAddr
	Comm_SendDataBuf[5] = Comm_DevAddr&0xFF;
	Comm_SendDataBuf[6] = (Comm_DevAddr&0xFF00)>>8;
	Comm_SendDataBuf[7] = (Comm_DevAddr&0xFF0000)>>16;
	Comm_SendDataBuf[8] = (Comm_DevAddr&0xFF000000)>>24;
	//ComputeSendDataCRC
	CRC = 0xAA;
	for(i = 0; i < (PKT_LENG-1); i++)
	{
		CRC += Comm_SendDataBuf[i];
	}
	Comm_SendDataBuf[i] = CRC;
	////////////////////
	
	RF_SetTxAddr((uint8_t *)&Comm_CurrentAddr,CURRENT_ADDR_LENG);
	RF_SetChannel(Comm_CurrentChannel);
	RF_TxMode();			//设置RF进入发射模式
	RF_TxData(Comm_SendDataBuf,PKT_LENG);
}
/*-------------------------------------------------------------------
-------------------------------------------------------------------*/

/********************************************************************
  通讯模块清空接收数据缓冲区函数
********************************************************************/
void Comm_ClrRecDataBuf(void)
{
	uint8_t i;
	for(i = 0; i < PKT_LENG; i++)
	{
		Comm_RecDataBuf[i] = 0;
	}
}
/*-------------------------------------------------------------------
-------------------------------------------------------------------*/

void Comm_SetToReceive(void)
{
	RF_SetRxP0Addr((uint8_t *)&Comm_CurrentAddr,CURRENT_ADDR_LENG);
	RF_SetChannel(Comm_CurrentChannel);
	RF_RxMode();
	Comm_ClrRecDataBuf();
}




void Comm_CheckDataReceived(void)
{
	uint8_t CRC,i;
	Comm_Flag &= ~Comm_DataReceived_Mask;
	if(ucRF_DumpRxData(Comm_RecDataBuf, PKT_LENG))
	{
		//接收到数据，检查包头是否有效
		if(Comm_RecDataBuf[0] == PKT_HEADER)
		{
			//收到有效的数据包头
			//检查CRC是否正确
			CRC = 0xAA;
			for(i = 0; i < (PKT_LENG-1); i++)
			{
				CRC += Comm_RecDataBuf[i];
			}
			if(Comm_RecDataBuf[i] == CRC)
			{
				Comm_Flag |= Comm_DataReceived_Mask;
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


void Comm_TimerProcess(void)
{
	uint8_t temp = 0;
	if(Comm_TimerTick != getSystemTime())
	{
		if(Comm_TimerTick < getSystemTime())
		{
			temp = getSystemTime() - Comm_TimerTick;
			Comm_TimerTick = getSystemTime();
		}
		else if(Comm_TimerTick > getSystemTime())
		{
			Comm_TimerTick = ~Comm_TimerTick+1;//Comm_TimerTick = 0xFFFFFFFF - Comm_TimerTick;
			temp = Comm_TimerTick;
			temp += getSystemTime();
			Comm_TimerTick = getSystemTime();
		}

		if(Comm_Flag&Comm_TimeoutEnBit_Mask)
		{
			if(Comm_Timeout > temp) Comm_Timeout -= temp;
			else Comm_Timeout = 0;
		}
	}
}

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
			//无连接状态
			Comm_CheckDataReceived();
			if(Comm_Flag&Comm_DataReceived_Mask)
			{
				switch(Comm_RecDataBuf[9])
				{
					case PKT_TYPE_CONNECT_FAST:
					{
						//快速链接包
						if(Comm_RecDataBuf[10] == PKT_DATA_FAST_LINK_HOST)
						{
							//主机发送的快速链接包
							//本地的Device地址与数据包中的Device地址相同
							//则接受该快速链接包
							//检查数据包中的从机地址是否跟本机地址相同
							if((Comm_RecDataBuf[5] == (Comm_DevAddr&0xFF)) && \
										(Comm_RecDataBuf[6] == ((Comm_DevAddr&0xFF00)>>8)) && \
										(Comm_RecDataBuf[7] == ((Comm_DevAddr&0xFF0000)>>16)) && \
										(Comm_RecDataBuf[8] == ((Comm_DevAddr&0xFF000000)>>24)))
							//if(*((uint32_t *)&Comm_RecDataBuf[5]) == Comm_DevAddr)
							{
								//保存主机地址
								Comm_HostAddr =  ((uint32_t)Comm_RecDataBuf[4] << 24) | \
																	((uint32_t)Comm_RecDataBuf[3] << 16) | \
																	((uint32_t)Comm_RecDataBuf[2] << 8) | \
																	(uint32_t)Comm_RecDataBuf[1];
								//PKT_TYPE
								Comm_SendDataBuf[9] = PKT_TYPE_CONNECT_FAST;
								//fast connect data
								//快速链接包数据域第一个字节表示发送该包数据的角色
								Comm_SendDataBuf[10] = PKT_DATA_FAST_LINK_DEVICE;
								//快速链接包数据域第二个字节表示建立连接后使用的频率
								Comm_SendDataBuf[11] = Comm_RecDataBuf[11];
								Comm_SendDataBuf[12] = 0;
								Comm_SendDataBuf[13] = 0;
								Comm_SendDataBuf[14] = 0;
								Comm_CurrentChannel = BIND_CHANNEL;
								Comm_CurrentAddr = BIND_ADDR;
								Comm_SendPacket();

								//设置接收频率为主机指定的频率
								Comm_CurrentChannel = Comm_RecDataBuf[11];
								Comm_CurrentAddr = Comm_DevAddr;
								Comm_SetToReceive();
								//设置连接状态为连接成功
								Comm_CurrentStatus = STATUS_CONNECTED;
								Comm_Timeout = COMM_TIMEOUT_SENDDATA_LOST;
								Comm_Flag |= Comm_TimeoutEnBit_Mask;
							}
						}
						break;
					}
					//default:
					//{
						//忽略其他类型的数据包
						//清除接收缓冲区数据重新接收数据包
					//	call	Comm_ClrRecDataBuf
					//	break;
					//}
				}
				Comm_ClrRecDataBuf();
				Comm_Flag &= ~Comm_DataReceived_Mask;
			}
			else
			{
					if(Comm_Timeout==0)
					{
						//开机无检测到快速链接包
						Comm_HostAddr = 0;
						Comm_Flag &= ~Comm_TimeoutEnBit_Mask;
						Comm_CurrentStatus = STATUS_CONNECTING;
					}
			}
			break;
		}
		case STATUS_CONNECTING:
		{
			//正在尝试连接
			Comm_CheckDataReceived();
			if(Comm_Flag&Comm_DataReceived_Mask)
			{
				switch(Comm_RecDataBuf[9])
				{
					case PKT_TYPE_CONNECT_NORMAL:
					{
						//配对过程中只接受连接类型的数据包
						//配对类型的数据包，包类型的下一个字节
						//表示配对进行到哪个阶段
						switch(Comm_RecDataBuf[10])
						{
							case 1:
							{
								//配对阶段1，收到主机发送的配对请求数据包
								//关闭超时定时器
								Comm_Flag &= Comm_TimeoutEnBit_Mask;
								Comm_Timeout = 0;
								//PKT_TYPE
								Comm_SendDataBuf[9] = PKT_TYPE_CONNECT_NORMAL;
								//配对阶段2
								Comm_SendDataBuf[10] = 2;
								Comm_SendDataBuf[11] = 0;
								Comm_SendDataBuf[12] = 0;
								Comm_SendDataBuf[13] = 0;
								Comm_SendDataBuf[14] = 0;
								Comm_CurrentAddr = BIND_ADDR;
								Comm_CurrentChannel = BIND_CHANNEL;
								DelayMsec(getSystemTime()&0xF);//延时一个随机数，在多从机单主机环境下减少冲突
								Comm_SendPacket();
								//回应主机后重新进入接收状态，准备接收配对阶段3的数据
								Comm_CurrentAddr = BIND_ADDR;
								Comm_CurrentChannel = BIND_CHANNEL;
								Comm_SetToReceive();
								break;
							}
							case 3:
							{
								//检查数据包中的从机地址是否跟本机地址相同
								if((Comm_RecDataBuf[5] == (Comm_DevAddr&0xFF)) && \
										(Comm_RecDataBuf[6] == ((Comm_DevAddr&0xFF00)>>8)) && \
										(Comm_RecDataBuf[7] == ((Comm_DevAddr&0xFF0000)>>16)) && \
										(Comm_RecDataBuf[8] == ((Comm_DevAddr&0xFF000000)>>24)))
								//if(*((uint32_t *)&Comm_RecDataBuf[5]) == Comm_DevAddr)
								{
									//收到主机确认配对的数据，将接收频率改为主机指定频率
									//将接收地址设置为本机地址
									Comm_CurrentAddr = Comm_DevAddr;
									Comm_CurrentChannel = Comm_RecDataBuf[11];
									Comm_SetToReceive();
									Comm_Timeout = COMM_TIMEOUT_BIND_LOADVAL;
									Comm_Flag |= Comm_TimeoutEnBit_Mask;
								}
								break;
							}
							case 4:
							{
								//检查数据包中的从机地址是否跟本机地址相同
								if((Comm_RecDataBuf[5] == (Comm_DevAddr&0xFF)) && \
										(Comm_RecDataBuf[6] == ((Comm_DevAddr&0xFF00)>>8)) && \
										(Comm_RecDataBuf[7] == ((Comm_DevAddr&0xFF0000)>>16)) && \
										(Comm_RecDataBuf[8] == ((Comm_DevAddr&0xFF000000)>>24)))
								//if(*((uint32_t *)&Comm_RecDataBuf[5]) == Comm_DevAddr)
								{
									//收到主机发送的确认数据，连接成功。
									//立即返回确认数据给主机并将主机地址保存
									//PKT_TYPE
									Comm_SendDataBuf[9] = PKT_TYPE_CONNECT_NORMAL;
									//配对阶段5
									Comm_SendDataBuf[10] = 5;
									Comm_SendDataBuf[11] = 0;
									Comm_SendDataBuf[12] = 0;
									Comm_SendDataBuf[13] = 0;
									Comm_SendDataBuf[14] = 0;
									//保存主机地址
									Comm_HostAddr = ((uint32_t)Comm_RecDataBuf[4] << 24) | \
																	((uint32_t)Comm_RecDataBuf[3] << 16) | \
																	((uint32_t)Comm_RecDataBuf[2] << 8) | \
																	(uint32_t)Comm_RecDataBuf[1];
									//设置发射频率为主机指定的频率
									Comm_CurrentAddr = Comm_HostAddr;
									Comm_CurrentChannel = Comm_RecDataBuf[11];
									Comm_SendPacket(); //将数据包发送给主机
									//回应主机后重新进入接收状态，已经连接成功
									Comm_CurrentAddr = Comm_DevAddr;
									Comm_CurrentChannel = Comm_RecDataBuf[11];
									Comm_SetToReceive();
									//设置连接状态为连接成功
									Comm_CurrentStatus = STATUS_CONNECTED;
									/**********************************************/
									Comm_Timeout = COMM_TIMEOUT_SENDDATA_LOST;
									Comm_Flag |= Comm_TimeoutEnBit_Mask;
									/**********************************************/
								}
								break;
							}
							//default:
							//{
							//	call	Comm_ClrRecDataBuf
							//	break;
							//}
						}
						break;
					}
					//default:
					//{
						//忽略其他类型的数据包
						//清除接收缓冲区数据重新接收数据包
					//	call	Comm_ClrRecDataBuf
					//	break;
					//}
				}
				Comm_ClrRecDataBuf();
				Comm_Flag &= ~Comm_DataReceived_Mask;
			}
			else
			{
				if(Comm_Flag&Comm_TimeoutEnBit_Mask)
				{
					//超时定时器已经打开，检查是否超时
					if(Comm_Timeout==0)
					{
						//连接过程中超时
						Comm_HostAddr = 0;
						Comm_Flag &= ~Comm_TimeoutEnBit_Mask;
						Comm_CurrentAddr = BIND_ADDR;
						Comm_CurrentChannel = BIND_CHANNEL;
						Comm_SetToReceive();
					}
				}
			}
			break;
		}
		case STATUS_CONNECTED:
		{
			//已经连接上主机
			Comm_CheckDataReceived();
			if(Comm_Flag&Comm_DataReceived_Mask)
			{
				if(Comm_RecDataBuf[9] == PKT_TYPE_CONTROL || \
					Comm_RecDataBuf[9] == PKT_TYPE_CHECK_ONLINE)
				{
					//正常通讯只接受控制数据包和检查在线数据包
					//PKT_TYPE
					Comm_SendDataBuf[9] = PKT_TYPE_ACK;
					Comm_SendDataBuf[10] = 0;
					Comm_SendDataBuf[11] = 0;
					Comm_SendDataBuf[12] = 0;
					Comm_SendDataBuf[13] = 0;
					Comm_SendDataBuf[14] = 0;
					Comm_CurrentAddr = Comm_HostAddr;
					Comm_SendPacket();

					if(Comm_RecDataBuf[9] == PKT_TYPE_CONTROL)
					{
						//控制数据包，提取控制数据
						Comm_Data[0] = Comm_RecDataBuf[10];
						Comm_Data[1] = Comm_RecDataBuf[11];
						Comm_Data[2] = Comm_RecDataBuf[12];
						Comm_Data[3] = Comm_RecDataBuf[13];
						Comm_Data[4] = Comm_RecDataBuf[14];
						Comm_Flag |= Comm_NewData_Mask;
					}
					Comm_CurrentAddr = Comm_DevAddr;
					Comm_SetToReceive();
					Comm_Timeout = COMM_TIMEOUT_SENDDATA_LOST;
					Comm_Flag |= Comm_TimeoutEnBit_Mask;
				}
				Comm_ClrRecDataBuf();
				Comm_Flag &= ~Comm_DataReceived_Mask;
			}
			else
			{
				if(Comm_Flag&Comm_TimeoutEnBit_Mask)
				{
					//超时定时器已经打开，检查是否超时
					if(Comm_Timeout==0)
					{
						Comm_CurrentAddr = BIND_ADDR;
						Comm_CurrentChannel = BIND_CHANNEL;
						Comm_SetToReceive();
						Comm_CurrentStatus = STATUS_LOST;
					}
				}
			}
			break;
		}
		case STATUS_SLEEP:
		{
			//主机进入睡眠模式
			Comm_CurrentStatus = STATUS_LOST;
			break;
		}
		case STATUS_LOST:
		{
			//与主机连接中断
			Comm_CheckDataReceived();
			if(Comm_Flag&Comm_DataReceived_Mask)
			{
					if(Comm_RecDataBuf[9] == PKT_TYPE_CONNECT_FAST)
					{
						//快速链接包
						if(Comm_RecDataBuf[10] == PKT_DATA_FAST_LINK_HOST)
						{
							//主机发送的快速链接包
							//本地保存的Host地址与数据包中的Host地址相同
							//则接受该快速链接包
							//检查数据包中的主机地址是否跟本地保存的主机地址相同
							if((Comm_RecDataBuf[1] == (Comm_HostAddr&0xFF)) && \
										(Comm_RecDataBuf[2] == ((Comm_HostAddr&0xFF00)>>8)) && \
										(Comm_RecDataBuf[3] == ((Comm_HostAddr&0xFF0000)>>16)) && \
										(Comm_RecDataBuf[4] == ((Comm_HostAddr&0xFF000000)>>24)))
							//if(*((uint32_t *)&Comm_RecDataBuf[1]) == Comm_HostAddr)
							{
								//PKT_TYPE
								Comm_SendDataBuf[9] = PKT_TYPE_CONNECT_FAST;
								//fast connect data
								//快速链接包数据域第一个字节表示发送该包数据的角色
								Comm_SendDataBuf[10] = PKT_DATA_FAST_LINK_DEVICE;
								//快速链接包数据域第二个字节表示建立连接后使用的频率
								Comm_SendDataBuf[11] = Comm_RecDataBuf[11];
								Comm_SendDataBuf[12] = 0;
								Comm_SendDataBuf[13] = 0;
								Comm_SendDataBuf[14] = 0;
								Comm_CurrentChannel = BIND_CHANNEL;
								Comm_CurrentAddr = BIND_ADDR;
								Comm_SendPacket();

								//设置接收频率为主机指定的频率
								Comm_CurrentChannel = Comm_RecDataBuf[11];
								Comm_CurrentAddr = Comm_DevAddr;
								Comm_SetToReceive();
								//设置连接状态为连接成功
								Comm_CurrentStatus = STATUS_CONNECTED;
								Comm_Timeout = COMM_TIMEOUT_SENDDATA_LOST;
								Comm_Flag|=Comm_TimeoutEnBit_Mask;
							}
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

