#ifndef CONFIG_TABLE_H
#define CONFIG_TABLE_H

#include "def.h"
#include "stdint.h"


typedef struct config_table_tt
{
float version;
float pidPitch[3];
float pidPitchRate[3];
float pidRoll[3];
float pidRollRate[3];
float pidYaw[3];
float pidYawRate[3];
float accOffset[3];
float gyroOffset[3];

}config_table_t;

extern config_table_t table;	
extern uint8_t gParamsSaveEEPROMRequset;

void LoadParamsFromFlash(void);
void ParamSetDefault(void);
void ParamToTable(void);
void TableToParam(void);
void TableWriteFlash(void);
void TableReadFlash(void);
void SaveParamsToFlash(void);

#endif
