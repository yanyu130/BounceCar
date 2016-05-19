#include "def.h"
#include "ConfigTable.h"
#include "uart.h"
#include "FlashCtrl.h"
#include "stdio.h"
#include "control.h"

#define TABLE_ADDRESS 	DATA_Flash_Start_ADD
//用来存放Flash列表上的存放的参数变量的信息
config_table_t table;	
//请求保存参数到EEPROM的信号量
uint8_t gParamsSaveEEPROMRequset=0;

#define Flash_DEFAULT_VERSION 4

//all default value 
 void ParamSetDefault(void)
{
		 
     pitch_angle_PID.P = 1.3;		//4
     pitch_angle_PID.I = 0.07;
     pitch_angle_PID.D = 1.1;
	   pitch_angle_PID.iLimit = 60;	
		 
     pitch_rate_PID.P  = 4; //0.09
     pitch_rate_PID.I  = 0; 		//9.5/8.9
     pitch_rate_PID.D  = 0.0;
		 pitch_rate_PID.iLimit = 30;
//////////////////////////////////////////////
     roll_angle_PID.P = 0.1;		//4
     roll_angle_PID.I = 0.1;
     roll_angle_PID.D = 0;
		 roll_angle_PID.iLimit = 30;	//or 1000

     roll_rate_PID.P  = 0.15;	//0.09
     roll_rate_PID.I  = 0.05; 		//9.5/8.9
     roll_rate_PID.D  = 0; 
		 roll_rate_PID.iLimit = 30;

     yaw_rate_PID.P  = 0.04;
     yaw_rate_PID.I  = 0.01; 
     yaw_rate_PID.D  = 0; 

		 yaw_angle_PID.P = 0;
     yaw_angle_PID.I = 0;
     yaw_angle_PID.D = 0;

//
//			alt_PID.P=1.0;
//			alt_PID.I=0;
//			alt_PID.D=0;
//			
//			alt_vel_PID.P=0.1f;
//			alt_vel_PID.I=0.02f;
//			alt_vel_PID.D=0;
//			
//			 //should chango to read Flash cfg. should be 0.
//			imu.accOffset[0]=-0.1620515;
//			imu.accOffset[1]=0.07422026;	
//			imu.accOffset[2]=0.7743073;

//			imu.gyroOffset[0]=-0.06097556;
//			imu.gyroOffset[1]=-0.03780485;
//			imu.gyroOffset[2]=0;
			
		
//		AttiCtrlParamsFromPIDTable();

}

//


uint8_t  isFlashValid(void)
{
	DATA_FLASH_Read(TABLE_ADDRESS,(int32_t*)(&table),1);
	//printf("table.version = %dw\n",table.version);
	if(table.version==Flash_DEFAULT_VERSION)
			return 1;
	else
			return 0;
}

//table defalat . if 
void TableResetDefault(void)
{		
		uint8_t paramNums=sizeof(table)/sizeof(float);
		DATA_FLASH_Write(TABLE_ADDRESS,(int32_t *)(&(table)),paramNums);
}

//load params for Flash
void TableReadFlash(void)
{
		uint8_t paramNums=sizeof(table)/sizeof(float);
		
		DATA_FLASH_Read(TABLE_ADDRESS,(int32_t *)(&table),paramNums);
}

void TableWriteFlash(void)
{
		uint8_t paramNums=sizeof(table)/sizeof(float);
	//	printf("paramNums = %d \n",paramNums);
		DATA_FLASH_Write(TABLE_ADDRESS,(int32_t *)(&table),paramNums);
}

void WriteVersionToFlash(void)
{
		DATA_FLASH_Write(TABLE_ADDRESS,(int32_t *)(&table),1);
}


//extern uint8_t RX_ADDRESS[RX_ADR_WIDTH];


//extern uint8_t NRFMatched;


void TableToParam(void)
{
		uint8_t i=0;
		for(i=0;i<3;i++)
		{
			((float *)(&pitch_angle_PID))[i]=((float *)(&table.pidPitch))[i];
			((float *)(&roll_angle_PID))[i]=((float *)(&table.pidRoll))[i];
			((float *)(&yaw_angle_PID))[i]=((float *)(&table.pidYaw))[i];
			
			((float *)(&pitch_rate_PID))[i]=((float *)(&table.pidPitchRate))[i];
			((float *)(&roll_rate_PID))[i]=((float *)(&table.pidRollRate))[i];
			((float *)(&yaw_rate_PID))[i]=((float *)(&table.pidYawRate))[i];
			
//			((float *)(&alt_PID))[i]=((float *)(&table.pidAlt))[i];
//			((float *)(&alt_vel_PID))[i]=((float *)(&table.pidAltVel))[i];
			
//			imu.accOffset[i]=table.accOffset[i];
//			imu.gyroOffset[i]=table.gyroOffset[i];
//			
			
		}
		
//		for(i=0;i<5;i++){
//			((u8 *)(&RX_ADDRESS))[i] = ((float *)(&table.NRFaddr))[i];
//			
//			printf("RX_ADDRESS[%d]:0x%x\r\n",i,RX_ADDRESS[i]);
//		}
		
//	  BTstate = table.BTstate;
//		NRFMatched = table.NRFmatchFlag;
		
		
		
}


void ParamToTable(void)
{
		uint8_t i=0;
		float temp = 0.0;
		for(i=0;i<3;i++)
		{
			((float *)(&table.pidPitch))[i]=((float *)(&pitch_angle_PID))[i];
				temp=((float *)(&roll_angle_PID))[i];
				*((float *)(&table.pidRoll) + i) =  ((float *)(&roll_angle_PID))[i];
			((float *)(&table.pidRoll))[i]=((float *)(&roll_angle_PID))[i];
			((float *)(&table.pidYaw))[i]=((float *)(&yaw_angle_PID))[i];
			
			((float *)(&table.pidPitchRate))[i]=((float *)(&pitch_rate_PID))[i];
			((float *)(&table.pidRollRate))[i]=((float *)(&roll_rate_PID))[i];
			((float *)(&table.pidYawRate))[i]=((float *)(&yaw_rate_PID))[i];
			
//			((float *)(&table.pidAlt))[i]=((float *)(&alt_PID))[i];
//			((float *)(&table.pidAltVel))[i]=((float *)(&alt_vel_PID))[i];

//			 
//			table.accOffset[i]=imu.accOffset[i];
//			table.gyroOffset[i]=imu.gyroOffset[i];
		}
		
//		for(i=0;i<5;i++)
//		((float *)(&table.NRFaddr))[i] = ((u8 *)(&RX_ADDRESS))[i];
//		
//		
//		table.BTstate = BTstate;
//		table.NRFmatchFlag = NRFMatched;
		
		
}

void LoadParamsFromFlash(void)
{
	if(isFlashValid())
	{
			TableReadFlash();
			TableToParam();
		
	}
	else
	{
			//printf("load params from Flash failed,set default value\r\n");
		
			ParamSetDefault();//版本检测不对，各项参数设为默认值
			ParamToTable();
			table.version = Flash_DEFAULT_VERSION;
		
			printf("table.version = %d\n",(int)table.version);
			FlashErase(TABLE_ADDRESS);	//必须先擦除，才能成功写入
			TableWriteFlash();
			//WriteVersionToFlash();
	}
}

void SaveParamsToFlash(void)
{
	printf("SaveParamsToFlash \n");
		ParamToTable();
	FlashErase(TABLE_ADDRESS);	//必须先擦除，才能成功写入
		TableWriteFlash();
}


