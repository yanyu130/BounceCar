#include "Motor.h"
#include "def.h"
#include "Timer_Ctrl.h"

//飞机正方向，左下角为电机1，逆时钟以此类推，电机2，电机3，电机4
#define MOTOR_1 0
#define MOTOR_2 1
#define MOTOR_3 2
#define MOTOR_4 3

int16_t motor1PWM, motor2PWM, motor3PWM, motor4PWM; 

void Motor_Init(void)
{
	CLK_EnableModuleClock(PWMCH01_MODULE);
	CLK_EnableModuleClock(PWMCH23_MODULE);
	CLK_SetModuleClock(PWMCH01_MODULE, CLK_CLKSEL1_PWMCH01SEL_HCLK, 0);
	CLK_SetModuleClock(PWMCH23_MODULE, CLK_CLKSEL1_PWMCH23SEL_HCLK, 0);
	SYS->P1_MFP = SYS_MFP_P12_PWM0_CH0 | SYS_MFP_P13_PWM0_CH1;
	SYS->P2_MFP = SYS_MFP_P24_PWM0_CH2 | SYS_MFP_P25_PWM0_CH3;
	
	MotorPwmOutput(0,0,0,0);
}


void Motor_Start(void)
{
	
	PWM_EnableOutput(PWM, 0x0f);
	PWM_Start(PWM, 0x0f);
	MotorPwmOutput(0,0,0,0);
}

void Motor_Stop(void)
{
	PWM_DisableOutput(PWM, 0xf);
	PWM_Stop(PWM, 0x0f);
	printf("Motor_Stop \n");
}

void MotorPwmOutput(int16_t MOTO1_PWM,int16_t MOTO2_PWM,int16_t MOTO3_PWM,int16_t MOTO4_PWM)
{
	if(MOTO1_PWM > 100)	MOTO1_PWM = 100;
	if(MOTO2_PWM > 100)	MOTO2_PWM = 100;
	if(MOTO3_PWM > 100)	MOTO3_PWM = 100;
	if(MOTO4_PWM > 100)	MOTO4_PWM = 100;
	
	if(MOTO1_PWM < 0)	MOTO1_PWM = 0;
	if(MOTO2_PWM < 0)	MOTO2_PWM = 0;
	if(MOTO3_PWM < 0)	MOTO3_PWM = 0;
	if(MOTO4_PWM < 0)	MOTO4_PWM = 0;
		
	PWM_ConfigOutputChannel(PWM, MOTOR_1, 400, MOTO1_PWM);
	
	PWM_ConfigOutputChannel(PWM, MOTOR_2, 400, MOTO2_PWM);

	PWM_ConfigOutputChannel(PWM, MOTOR_3, 400, MOTO3_PWM);

	PWM_ConfigOutputChannel(PWM, MOTOR_4, 400, MOTO4_PWM);
	
	motor1PWM = MOTO1_PWM;
	motor2PWM = MOTO2_PWM;
	motor3PWM = MOTO3_PWM;
	motor4PWM = MOTO4_PWM;

}

//void MotorPwmOutput2(void)
//{
//	PWM_ConfigOutputChannel(PWM, MOTOR_1, 400, motor1PWM);
//	
//	PWM_ConfigOutputChannel(PWM, MOTOR_2, 400, motor2PWM);

//	PWM_ConfigOutputChannel(PWM, MOTOR_3, 400, motor3PWM);

//	PWM_ConfigOutputChannel(PWM, MOTOR_4, 400, motor4PWM);

//}
