#include "Comm.h"
#include "Timer_Ctrl.h"

/*
  ����Ϊ�ӻ����������������Ӻ󷽿ɽ�������ͨѶ���������ӹ��̳�Ϊ��Թ��̡�
  �����ӳɹ��󣬴ӻ������������ݣ��յ��������ݺ���Ҫ����������ACK���ݰ�ȷ�ϡ�
  �������޿���������Ҫ���ͣ���Ӧ���ڲ�����Լ���ĳ�ʱʱ���ڷ���GETSTATUS����
  ���ݰ��������ʱ���ӻ���������Lost״̬�����½�����ԡ�
  ��Թ��̣�
      �ӻ��Թ��õ�ַ��Ƶ�ʽ��н����������͵����������������������Թ��̡�
      ��Խ׶�1��
          �����Թ���Ƶ�����õ�ַ������������������ݰ��а�����������ַ
      ��Խ׶�2��
          �ӻ��յ����������Թ���Ƶ�����õ�ַ��Ӧ�����������
          ��Ӧ���ݰ��а��������ʹӻ���ַ��
      ��Խ׶�3��
          �����յ��ӻ���Ӧ���ݺ��ٴ��Թ���Ƶ�����õ�ַ����ȷ��������ݰ���
	  ���ݰ��а���������ַ���ӻ���ַ��ͨѶƵ�ʡ�
      ��Խ׶�4��
          �ӻ����յ�������ȷ�����ݰ������������͵�ȷ�����ݰ���ָ����ͨѶƵ�ʣ�
	  �͵�ַ���н������ݡ�
      ��Խ׶�5��
	  �����յ��������ӳɹ��������ظ�����ȷ�����ݣ�������һ��ʱ��δ���յ����ݣ������Ӳ��ɹ������½�����Խ׶�1��
  ͨѶ���̣�
      ������ӻ����Ϳ������ݣ��ӻ����յ���ȷ�����ݺ󣬷��ظ�����ACK���ݰ��������յ�ACK���ݰ�
      ��ʾ���ݷ��ͳɹ���������ʧ�ܣ���Ҫ�ط����ݰ���
  �����޿������ݣ�
      ����û�п���������Ҫ����ʱ��������Ҫ��Լ���ĳ�ʱʱ���ڷ���GETSTATUS���͵����ݰ����ӻ���
      �Ա�ȷ��˫���Ƿ����ߡ�
*/





uint8_t Comm_Flag;						//ͨѶģ���־λ
uint8_t Comm_CurrentStatus;				//ͨѶģ�鵱ǰ״̬
uint8_t Comm_CurrentChannel;				//��ǰͨѶƵ��
uint32_t Comm_CurrentAddr;				//��ǰͨѶ��ַ
uint32_t Comm_HostAddr;					//������ַ
uint32_t Comm_DevAddr;					//�ӻ���ַ
uint8_t Comm_SendDataBuf[PKT_LENG];		//RF�������ݻ�����
uint8_t Comm_RecDataBuf[PKT_LENG];		//RF�������ݻ�����
uint16_t Comm_Timeout;					//��ʱ������
uint8_t Comm_Data[5];					//���ϲ���ʵĽ��յ�������


uint32_t Comm_TimerTick;
/********************************************************************
  ͨѶģ���ʼ������
********************************************************************/
void Comm_Init(void)
{

	//ȡоƬ��������Ϊ�����ַ���ӻ��ı����ַ����Device��ַ
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
	RF_Init();			//��ʼ��2.4G�����Ĵ���
	
	
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
  ͨѶģ�鷢�����ݰ�����
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
	RF_TxMode();			//����RF���뷢��ģʽ
	RF_TxData(Comm_SendDataBuf,PKT_LENG);
}
/*-------------------------------------------------------------------
-------------------------------------------------------------------*/

/********************************************************************
  ͨѶģ����ս������ݻ���������
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
		//���յ����ݣ�����ͷ�Ƿ���Ч
		if(Comm_RecDataBuf[0] == PKT_HEADER)
		{
			//�յ���Ч�����ݰ�ͷ
			//���CRC�Ƿ���ȷ
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
  ͨѶģ����ѭ��
********************************************************************/
void Comm_Process(void)
{
	Comm_TimerProcess();
	switch(Comm_CurrentStatus)
	{
		case STATUS_DISCONNECT:
		{
			//������״̬
			Comm_CheckDataReceived();
			if(Comm_Flag&Comm_DataReceived_Mask)
			{
				switch(Comm_RecDataBuf[9])
				{
					case PKT_TYPE_CONNECT_FAST:
					{
						//�������Ӱ�
						if(Comm_RecDataBuf[10] == PKT_DATA_FAST_LINK_HOST)
						{
							//�������͵Ŀ������Ӱ�
							//���ص�Device��ַ�����ݰ��е�Device��ַ��ͬ
							//����ܸÿ������Ӱ�
							//������ݰ��еĴӻ���ַ�Ƿ��������ַ��ͬ
							if((Comm_RecDataBuf[5] == (Comm_DevAddr&0xFF)) && \
										(Comm_RecDataBuf[6] == ((Comm_DevAddr&0xFF00)>>8)) && \
										(Comm_RecDataBuf[7] == ((Comm_DevAddr&0xFF0000)>>16)) && \
										(Comm_RecDataBuf[8] == ((Comm_DevAddr&0xFF000000)>>24)))
							//if(*((uint32_t *)&Comm_RecDataBuf[5]) == Comm_DevAddr)
							{
								//����������ַ
								Comm_HostAddr =  ((uint32_t)Comm_RecDataBuf[4] << 24) | \
																	((uint32_t)Comm_RecDataBuf[3] << 16) | \
																	((uint32_t)Comm_RecDataBuf[2] << 8) | \
																	(uint32_t)Comm_RecDataBuf[1];
								//PKT_TYPE
								Comm_SendDataBuf[9] = PKT_TYPE_CONNECT_FAST;
								//fast connect data
								//�������Ӱ��������һ���ֽڱ�ʾ���͸ð����ݵĽ�ɫ
								Comm_SendDataBuf[10] = PKT_DATA_FAST_LINK_DEVICE;
								//�������Ӱ�������ڶ����ֽڱ�ʾ�������Ӻ�ʹ�õ�Ƶ��
								Comm_SendDataBuf[11] = Comm_RecDataBuf[11];
								Comm_SendDataBuf[12] = 0;
								Comm_SendDataBuf[13] = 0;
								Comm_SendDataBuf[14] = 0;
								Comm_CurrentChannel = BIND_CHANNEL;
								Comm_CurrentAddr = BIND_ADDR;
								Comm_SendPacket();

								//���ý���Ƶ��Ϊ����ָ����Ƶ��
								Comm_CurrentChannel = Comm_RecDataBuf[11];
								Comm_CurrentAddr = Comm_DevAddr;
								Comm_SetToReceive();
								//��������״̬Ϊ���ӳɹ�
								Comm_CurrentStatus = STATUS_CONNECTED;
								Comm_Timeout = COMM_TIMEOUT_SENDDATA_LOST;
								Comm_Flag |= Comm_TimeoutEnBit_Mask;
							}
						}
						break;
					}
					//default:
					//{
						//�����������͵����ݰ�
						//������ջ������������½������ݰ�
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
						//�����޼�⵽�������Ӱ�
						Comm_HostAddr = 0;
						Comm_Flag &= ~Comm_TimeoutEnBit_Mask;
						Comm_CurrentStatus = STATUS_CONNECTING;
					}
			}
			break;
		}
		case STATUS_CONNECTING:
		{
			//���ڳ�������
			Comm_CheckDataReceived();
			if(Comm_Flag&Comm_DataReceived_Mask)
			{
				switch(Comm_RecDataBuf[9])
				{
					case PKT_TYPE_CONNECT_NORMAL:
					{
						//��Թ�����ֻ�����������͵����ݰ�
						//������͵����ݰ��������͵���һ���ֽ�
						//��ʾ��Խ��е��ĸ��׶�
						switch(Comm_RecDataBuf[10])
						{
							case 1:
							{
								//��Խ׶�1���յ��������͵�����������ݰ�
								//�رճ�ʱ��ʱ��
								Comm_Flag &= Comm_TimeoutEnBit_Mask;
								Comm_Timeout = 0;
								//PKT_TYPE
								Comm_SendDataBuf[9] = PKT_TYPE_CONNECT_NORMAL;
								//��Խ׶�2
								Comm_SendDataBuf[10] = 2;
								Comm_SendDataBuf[11] = 0;
								Comm_SendDataBuf[12] = 0;
								Comm_SendDataBuf[13] = 0;
								Comm_SendDataBuf[14] = 0;
								Comm_CurrentAddr = BIND_ADDR;
								Comm_CurrentChannel = BIND_CHANNEL;
								DelayMsec(getSystemTime()&0xF);//��ʱһ����������ڶ�ӻ������������¼��ٳ�ͻ
								Comm_SendPacket();
								//��Ӧ���������½������״̬��׼��������Խ׶�3������
								Comm_CurrentAddr = BIND_ADDR;
								Comm_CurrentChannel = BIND_CHANNEL;
								Comm_SetToReceive();
								break;
							}
							case 3:
							{
								//������ݰ��еĴӻ���ַ�Ƿ��������ַ��ͬ
								if((Comm_RecDataBuf[5] == (Comm_DevAddr&0xFF)) && \
										(Comm_RecDataBuf[6] == ((Comm_DevAddr&0xFF00)>>8)) && \
										(Comm_RecDataBuf[7] == ((Comm_DevAddr&0xFF0000)>>16)) && \
										(Comm_RecDataBuf[8] == ((Comm_DevAddr&0xFF000000)>>24)))
								//if(*((uint32_t *)&Comm_RecDataBuf[5]) == Comm_DevAddr)
								{
									//�յ�����ȷ����Ե����ݣ�������Ƶ�ʸ�Ϊ����ָ��Ƶ��
									//�����յ�ַ����Ϊ������ַ
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
								//������ݰ��еĴӻ���ַ�Ƿ��������ַ��ͬ
								if((Comm_RecDataBuf[5] == (Comm_DevAddr&0xFF)) && \
										(Comm_RecDataBuf[6] == ((Comm_DevAddr&0xFF00)>>8)) && \
										(Comm_RecDataBuf[7] == ((Comm_DevAddr&0xFF0000)>>16)) && \
										(Comm_RecDataBuf[8] == ((Comm_DevAddr&0xFF000000)>>24)))
								//if(*((uint32_t *)&Comm_RecDataBuf[5]) == Comm_DevAddr)
								{
									//�յ��������͵�ȷ�����ݣ����ӳɹ���
									//��������ȷ�����ݸ���������������ַ����
									//PKT_TYPE
									Comm_SendDataBuf[9] = PKT_TYPE_CONNECT_NORMAL;
									//��Խ׶�5
									Comm_SendDataBuf[10] = 5;
									Comm_SendDataBuf[11] = 0;
									Comm_SendDataBuf[12] = 0;
									Comm_SendDataBuf[13] = 0;
									Comm_SendDataBuf[14] = 0;
									//����������ַ
									Comm_HostAddr = ((uint32_t)Comm_RecDataBuf[4] << 24) | \
																	((uint32_t)Comm_RecDataBuf[3] << 16) | \
																	((uint32_t)Comm_RecDataBuf[2] << 8) | \
																	(uint32_t)Comm_RecDataBuf[1];
									//���÷���Ƶ��Ϊ����ָ����Ƶ��
									Comm_CurrentAddr = Comm_HostAddr;
									Comm_CurrentChannel = Comm_RecDataBuf[11];
									Comm_SendPacket(); //�����ݰ����͸�����
									//��Ӧ���������½������״̬���Ѿ����ӳɹ�
									Comm_CurrentAddr = Comm_DevAddr;
									Comm_CurrentChannel = Comm_RecDataBuf[11];
									Comm_SetToReceive();
									//��������״̬Ϊ���ӳɹ�
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
						//�����������͵����ݰ�
						//������ջ������������½������ݰ�
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
					//��ʱ��ʱ���Ѿ��򿪣�����Ƿ�ʱ
					if(Comm_Timeout==0)
					{
						//���ӹ����г�ʱ
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
			//�Ѿ�����������
			Comm_CheckDataReceived();
			if(Comm_Flag&Comm_DataReceived_Mask)
			{
				if(Comm_RecDataBuf[9] == PKT_TYPE_CONTROL || \
					Comm_RecDataBuf[9] == PKT_TYPE_CHECK_ONLINE)
				{
					//����ͨѶֻ���ܿ������ݰ��ͼ���������ݰ�
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
						//�������ݰ�����ȡ��������
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
					//��ʱ��ʱ���Ѿ��򿪣�����Ƿ�ʱ
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
			//��������˯��ģʽ
			Comm_CurrentStatus = STATUS_LOST;
			break;
		}
		case STATUS_LOST:
		{
			//�����������ж�
			Comm_CheckDataReceived();
			if(Comm_Flag&Comm_DataReceived_Mask)
			{
					if(Comm_RecDataBuf[9] == PKT_TYPE_CONNECT_FAST)
					{
						//�������Ӱ�
						if(Comm_RecDataBuf[10] == PKT_DATA_FAST_LINK_HOST)
						{
							//�������͵Ŀ������Ӱ�
							//���ر����Host��ַ�����ݰ��е�Host��ַ��ͬ
							//����ܸÿ������Ӱ�
							//������ݰ��е�������ַ�Ƿ�����ر����������ַ��ͬ
							if((Comm_RecDataBuf[1] == (Comm_HostAddr&0xFF)) && \
										(Comm_RecDataBuf[2] == ((Comm_HostAddr&0xFF00)>>8)) && \
										(Comm_RecDataBuf[3] == ((Comm_HostAddr&0xFF0000)>>16)) && \
										(Comm_RecDataBuf[4] == ((Comm_HostAddr&0xFF000000)>>24)))
							//if(*((uint32_t *)&Comm_RecDataBuf[1]) == Comm_HostAddr)
							{
								//PKT_TYPE
								Comm_SendDataBuf[9] = PKT_TYPE_CONNECT_FAST;
								//fast connect data
								//�������Ӱ��������һ���ֽڱ�ʾ���͸ð����ݵĽ�ɫ
								Comm_SendDataBuf[10] = PKT_DATA_FAST_LINK_DEVICE;
								//�������Ӱ�������ڶ����ֽڱ�ʾ�������Ӻ�ʹ�õ�Ƶ��
								Comm_SendDataBuf[11] = Comm_RecDataBuf[11];
								Comm_SendDataBuf[12] = 0;
								Comm_SendDataBuf[13] = 0;
								Comm_SendDataBuf[14] = 0;
								Comm_CurrentChannel = BIND_CHANNEL;
								Comm_CurrentAddr = BIND_ADDR;
								Comm_SendPacket();

								//���ý���Ƶ��Ϊ����ָ����Ƶ��
								Comm_CurrentChannel = Comm_RecDataBuf[11];
								Comm_CurrentAddr = Comm_DevAddr;
								Comm_SetToReceive();
								//��������״̬Ϊ���ӳɹ�
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
			//����δ����״̬

			break;
		}
	}
}
/*-------------------------------------------------------------------
-------------------------------------------------------------------*/

