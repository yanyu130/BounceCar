#ifndef _JUMP_H_
#define _JUMP_H_


#include "def.h"

typedef enum
{
	Stop = 0,
	Forward = 1,
	Backward = 2,
}MOTOR_DIR;


typedef enum
{
	Mode1 = 0,
	Mode2 = 1,
}JUMP_MODE;



#define FLAG_MODE_MASK								0x03
#define FLAG_MODE1_ACTIVE							0x01
#define FLAG_MODE2_ACTIVE							0x02

#define FLAG_NEW_KEY_MASK							0x04
#define FLAG_NEW_KEY									0x04

#define FLAG_KEY_VAL_MASK							0x08

#define FLAG_KEY_DELAY_EN_MASK				0x10
#define FLAG_KEY_DELAY_EN							0x10

#define FLAG_TIMEOUT_EN_MASK					0x20
#define FLAG_TIMEOUT_EN								0x20

#define FLAG_MODE2_READY_MASK					0x40
#define FLAG_MODE2_READY							0x40

#define FLAG_MODE2_TRIG_MASK					0x80
#define FLAG_MODE2_TRIG								0x80




#define	KEY_DELAY_TIME										50
#define	KEY_DELAY_TIME_LOAD_VAL						((KEY_DELAY_TIME*SYSTEM_TICK_FREQ)/1000)

#define	MODE2_BACKWARD_TIME								230
#define	MODE2_BACKWARD_TIME_LOAD_VAL			((MODE2_BACKWARD_TIME*SYSTEM_TICK_FREQ)/1000)

#define	MODE2_JUMP_TRIG_TIME							50
#define	MODE2_JUMP_TRIG_TIME_LOAD_VAL			((MODE2_JUMP_TRIG_TIME*SYSTEM_TICK_FREQ)/1000)

#define	MODE2_READY_TIME									2000
#define	MODE2_READY_TIME_LOAD_VAL					((MODE2_READY_TIME*SYSTEM_TICK_FREQ)/1000)

#define	MODE1_JUMP_TRIG_TIME							300
#define	MODE1_JUMP_TRIG_TIME_LOAD_VAL			((MODE1_JUMP_TRIG_TIME*SYSTEM_TICK_FREQ)/1000)

#define	MODE1_FINISH_TIME									2000
#define	MODE1_FINISH_TIME_LOAD_VAL				((MODE1_FINISH_TIME*SYSTEM_TICK_FREQ)/1000)

void 			Jump_Init									(void);
void 			Jump											(JUMP_MODE Mode);
void 			Jump_Sever								(void);
uint8_t 	ucJump_GetStatus					(void);












#endif
