#ifndef _KEY_H_
#define _KEY_H_






/*

#define PITCH_ADC_CH		1
#define ROLL_ADC_CH			3
#define YAW_ADC_CH			2
#define SPEED_ADC_CH		4
#define KEY_ADC_CH			7


#define PITCH_MIDDLE_POS_ADC_VAL	127
#define ROLL_MIDDLE_POS_ADC_VAL		127
#define YAW_MIDDLE_POS_ADC_VAL		128
#define SPEED_ACTIVE_ADC_VAL		10


#define KEY1_ACTIVE_ADC_VAL				0x16
#define KEY1_ACTIVE_ADC_VAL_LOW			(KEY1_ACTIVE_ADC_VAL-6)
#define KEY1_ACTIVE_ADC_VAL_HIGH		(KEY1_ACTIVE_ADC_VAL+6)

#define KEY2_ACTIVE_ADC_VAL				0x31
#define KEY2_ACTIVE_ADC_VAL_LOW			(KEY2_ACTIVE_ADC_VAL-6)
#define KEY2_ACTIVE_ADC_VAL_HIGH		(KEY2_ACTIVE_ADC_VAL+6)

#define KEY3_ACTIVE_ADC_VAL				0x3F
#define KEY3_ACTIVE_ADC_VAL_LOW			(KEY3_ACTIVE_ADC_VAL-6)
#define KEY3_ACTIVE_ADC_VAL_HIGH		(KEY3_ACTIVE_ADC_VAL+6)

#define KEY4_ACTIVE_ADC_VAL				0x67
#define KEY4_ACTIVE_ADC_VAL_LOW			(KEY4_ACTIVE_ADC_VAL-6)
#define KEY4_ACTIVE_ADC_VAL_HIGH		(KEY4_ACTIVE_ADC_VAL+6)

#define KEY5_ACTIVE_ADC_VAL				0x80
#define KEY5_ACTIVE_ADC_VAL_LOW			(KEY5_ACTIVE_ADC_VAL-6)
#define KEY5_ACTIVE_ADC_VAL_HIGH		(KEY5_ACTIVE_ADC_VAL+6)

#define KEY6_ACTIVE_ADC_VAL				0x9A
#define KEY6_ACTIVE_ADC_VAL_LOW			(KEY6_ACTIVE_ADC_VAL-6)
#define KEY6_ACTIVE_ADC_VAL_HIGH		(KEY6_ACTIVE_ADC_VAL+6)


*/

//void Key_Noise_Filter_Init	(void);
void Key_Process			(void);
void Key_Init				(void);







#endif

extern uint16_t Correct_Key;

FB		equ		PA.3
L		equ		PB.1	
R		equ		PB.2
fastL	equ		PB.4
fastR	equ		PB.5
jmp 	equ		PB.3

minL	equ		PA.6
s2		equ		PA.0
mode	equ		PA.5
minR	equ		PA.7

LED		equ		PB.0