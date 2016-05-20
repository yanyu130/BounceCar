#include "Comm.h"



uint8_t	Comm_Flag;						//ͨѶģ���־λ
uint8_t Comm_CurrentStatus;				//ͨѶģ�鵱ǰ״̬
uint8_t Comm_CurrentChannel;				//��ǰͨѶƵ��
uint32_t Comm_CurrentAddr;				//��ǰͨѶ��ַ
uint32_t Comm_HostAddr;					//������ַ
uint32_t Comm_DevAddr;					//�ӻ���ַ
uint8_t Comm_SendDataBuf[PKT_LENG];		//RF�������ݻ�����
uint8_t Comm_RecDataBuf[PKT_LENG];		//RF�������ݻ�����
uint8_t Comm_Timeout;					//��ʱ������
uint8_t Comm_RetryCount;				//���Լ�����

uint8_t Comm_ErrCode;					//�ϲ���÷������ݽӿڷ��صĴ������
uint8_t Comm_Data[5];					//�ϲ������Ҫ���͵�����
uint8_t Comm_Buf[5];
uint8_t Comm_LastData[5];				//�����ϴη��͵����ݣ���ͬ���ݲ���������
uint8_t Comm_SameDataDelay;

uint16_t Comm_TimerTick;

/********************************************************************
  ͨѶģ���ʼ������
********************************************************************/
void Comm_Init(void)
{
	//ȡоƬ��������Ϊ�����ַ�������ı����ַ����Host��ַ
	WORD    Point;
	Point = _SYS (ADR.ROLL);
	Comm_HostAddr$0 = *Point$L;
	Comm_HostAddr$1 = *Point$H;
	Point++;
	Comm_HostAddr$2 = *Point$L;
	Comm_HostAddr$3 = 0;//*Point$H;

//	Comm_HostAddr = 0xaabbccdd;


	RF_Init();				//��ʼ��2.4Gģ��

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
  ͨѶģ�鷢�����ݰ�����
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
	RF_TxMode();	//����RF���뷢��ģʽ

	pRF_Data = Comm_SendDataBuf;
	RF_Leng = PKT_LENG;
	RF_TxData();
}
/*-------------------------------------------------------------------
-------------------------------------------------------------------*/


/********************************************************************
  ͨѶģ����ս������ݻ���������
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
		//���յ����ݣ�����ͷ�Ƿ���Ч
		if(Comm_RecDataBuf[0] == PKT_HEADER)
		{
			//�յ���Ч�����ݰ�ͷ
			//���CRC�Ƿ���ȷ
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
				//CRCУ�����
				//������ջ������������½������ݰ�
				Comm_ClrRecDataBuf();
			}
		}
		else
		{
			//�յ���Ч�����ݰ�ͷ
			//������ջ������������½������ݰ�
			Comm_ClrRecDataBuf();
		}
	}
}
/*-------------------------------------------------------------------
-------------------------------------------------------------------*/



/*******************************************************************
 ���ݷ��ͽӿ�
*******************************************************************/
void Comm_SendData(void)
{
	switch(Comm_CurrentStatus)
	{
		case STATUS_CONNECTED:
		{
			if(Comm_SendDataBit == false)
			{
				//��ǰû���ڷ�����
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
					//����ʱ��
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
��������������Ҫ���ź�֪ͨ�ӻ���������״̬����������ѯ�ʴӻ����ߵ�
���ݰ�����ӳ���10S������ͨѶ�Ĳ�ѯ�����100Ms
*/
/********************************************************************
  ͨѶģ����ѭ��
********************************************************************/
void Comm_Process(void)
{
	Comm_TimerProcess();
	switch(Comm_CurrentStatus)
	{
		case STATUS_DISCONNECT:
		{
			//������״̬�����Ϳ�����������
			//����ͻȻ�ϵ�������ϵ磬ͨ�����Ϳ������������ʵ����
			//֮ǰ��ԵĴӻ����ٽ�������
			if(Comm_Timeout == 0)
			{
				Comm_RetryCount++;
				if(Comm_RetryCount <= COMM_MAX_RETRY_FAST_LINK)
				{
					//���ճ�ʱ�����·���һ�ο������������
					//PKT_TYPE
					Comm_SendDataBuf[9] = PKT_TYPE_CONNECT_FAST;
					//�������Ӱ��������һ���ֽڱ�ʾ���͸ð����ݵĽ�ɫ
					Comm_SendDataBuf[10] = PKT_DATA_FAST_LINK_HOST;
					//�������Ӱ�������ڶ����ֽڱ�ʾ�������Ӻ�ʹ�õ�Ƶ��
					Comm_GetChannel();
					Comm_SendDataBuf[11] = a;	//GetChannel ��ͨ���ű��浽a��
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
					//�������ӳ���������Դ���
					//���볣����Թ���
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
							//�յ����Դӻ����صĿ������������Ӧ��
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
			//���ڳ�������
			if(Comm_Timeout == 0)
			{
				//���ճ�ʱ�����·���һ�����������
				Comm_DevAddr = 0;
				//PKT_TYPE
				Comm_SendDataBuf[9] = PKT_TYPE_CONNECT_NORMAL;
				//��Խ׶�1
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
						//��Թ�����ֻ�����������͵����ݰ�
						//������͵����ݰ��������͵���һ���ֽ�
						//��ʾ��Խ��е��ĸ��׶�
						switch(Comm_RecDataBuf[10])
						{
							case 2:
							{
								//����ӻ���ַ
								Comm_DevAddr$0 = Comm_RecDataBuf[5];
								Comm_DevAddr$1 = Comm_RecDataBuf[6];
								Comm_DevAddr$2 = Comm_RecDataBuf[7];
								Comm_DevAddr$3 = Comm_RecDataBuf[8];
								//PKT_TYPE
								Comm_SendDataBuf[9] = PKT_TYPE_CONNECT_NORMAL;
								//��Խ׶�3
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
								//��Խ׶�4
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
								//���ò�ѯ�ӻ����߳�ʱʱ��
								Comm_TimeoutEnBit = true;
								Comm_Timeout = COMM_TIMEOUT_CHECK_ONLINE_LOADVAL;
								Comm_SendDataBit = false;
								//��������״̬Ϊ���ӳɹ�
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
			//�Ѿ������ϴӻ�
			if(Comm_WaitACK == true)
			{
				//���ڵȴ��ӻ���Ӧ
				Comm_CheckDataReceived();
				if(Comm_DataReceived == True)
				{
					if(Comm_RecDataBuf[9] == PKT_TYPE_ACK)
					{
						//���յ��ӻ��Ļ�Ӧ
						//���¼��ز�ѯ�ӻ����߳�ʱʱ��
						Comm_Timeout = COMM_TIMEOUT_CHECK_ONLINE_LOADVAL;
						Comm_TimeoutEnBit = true;
						//����������ݱ�־λ����ʾ���ݷ��ͳɹ�
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
							//��ʱû�н��յ��ӻ���Ӧ
							if(Comm_RetryCount >= COMM_MAX_RETRY_SENDDATA)
							{
								//������������ط�����
								//����״̬Ϊ��ʧ
								Comm_CurrentStatus = STATUS_LOST;
								Comm_Timeout = 0;
								Comm_TimeoutEnBit = true;
								Comm_RetryCount = 0;
								Comm_WaitACK = false;
							}
							else
							{
								//��û��������ط����ݴ���
								//�ط��ϴ����ݰ�
								Comm_CurrentAddr = Comm_DevAddr;
								Comm_SendPacket();
								Comm_RetryCount++;
								//���½��մӻ���Ӧ
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
				//û���ڵȴ��ӻ���Ӧ
				if(Comm_SendDataBit == true)
				{
					//����������Ҫ����
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
					//���ý��մӻ���Ӧ
					Comm_CurrentAddr = Comm_HostAddr;
					Comm_SetToReceive();
					Comm_Timeout = COMM_TIMEOUT_SENDDATA_LOADVAL;
					Comm_TimeoutEnBit = true;
					Comm_WaitACK = true;
				}
				else
				{
					//����������Ҫ����
					if(Comm_TimeoutEnBit == true)
					{
						if(Comm_Timeout == 0)
						{
							//��ʱ����Ҫ����ѯ�ʴӻ��������ݰ�
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
							//���ý��մӻ���Ӧ
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
//			//��������˯��ģʽ
//
//			break;
//		}
		case STATUS_LOST:
		{
			//��ӻ������ж�
			if(Comm_TimeoutEnBit == true)
			{
				if(Comm_Timeout == 0)
				{
					//PKT_TYPE
					Comm_SendDataBuf[9] = PKT_TYPE_CONNECT_FAST;
					//fast connect data
					//�������Ӱ��������һ���ֽڱ�ʾ���͸ð����ݵĽ�ɫ
					Comm_SendDataBuf[10] = PKT_DATA_FAST_LINK_HOST;
					//�������Ӱ�������ڶ����ֽڱ�ʾ�������Ӻ�ʹ�õ�Ƶ��
					Comm_GetChannel();
					Comm_SendDataBuf[11] = a;
					Comm_SendDataBuf[12] = 0;
					Comm_SendDataBuf[13] = 0;
					Comm_SendDataBuf[14] = 0;
					Comm_CurrentChannel = BIND_CHANNEL;
					Comm_CurrentAddr = BIND_ADDR;
					Comm_SendPacket();

					//���ý��մӻ���Ӧ
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
								//�յ����Դӻ����صĿ������������Ӧ��
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
			//����δ����״̬

			break;
		}
	}
}
/*-------------------------------------------------------------------
-------------------------------------------------------------------*/

