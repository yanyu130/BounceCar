#include "Motor.h"
#include "def.h"
#include "Timer_Ctrl.h"
#include "control.h"
#include "math.h"

#define MOTOR_DEAD_ZONE 15

//飞机正方向，左下角为电机1，逆时钟以此类推，电机2，电机3，电机4
#define MOTOR_1 5
#define MOTOR_2 4
#define MOTOR_3 0
#define MOTOR_4 1

int16_t motor1PWM, motor2PWM, motor3PWM, motor4PWM; 

void Motor_Init(void)
{
	CLK_EnableModuleClock(PWMCH01_MODULE);
	CLK_EnableModuleClock(PWMCH45_MODULE);
	CLK_SetModuleClock(PWMCH01_MODULE, CLK_CLKSEL1_PWMCH01SEL_HCLK, 0);
	CLK_SetModuleClock(PWMCH45_MODULE, CLK_CLKSEL2_PWMCH45SEL_HCLK, 0);
	SYS->P1_MFP |= SYS_MFP_P12_PWM0_CH0 | SYS_MFP_P13_PWM0_CH1 | SYS_MFP_P14_PWM0_CH4;
	SYS->P0_MFP |= SYS_MFP_P04_PWM0_CH5;
	
	MotorPwmOutput(0,0,0,0);
}


void Motor_Start(void)
{
	
	PWM_EnableOutput(PWM, 0x33);
	PWM_Start(PWM, 0x33);
	//MotorPwmOutput(0,0,0,0);
	//g_fSpeedControlIntegral = 0;
}

void Motor_Stop(void)
{
	PWM_DisableOutput(PWM, 0x33);
	PWM_Stop(PWM, 0x33);
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
	
	//低于MOTOR_DEAD_ZONE
	if(MOTO1_PWM <= MOTOR_DEAD_ZONE)
	{
		MOTO1_PWM =0;
	}
	if(MOTO2_PWM <= MOTOR_DEAD_ZONE)
	{
		MOTO2_PWM =0;
	}
	if(MOTO3_PWM <= MOTOR_DEAD_ZONE)
	{
		MOTO3_PWM =0;
	}
	if(MOTO4_PWM <= MOTOR_DEAD_ZONE)
	{
		MOTO4_PWM =0;
	}
	
	PWM_ConfigOutputChannel(PWM, MOTOR_1, 100, MOTO1_PWM);
	PWM_ConfigOutputChannel(PWM, MOTOR_2, 100, MOTO2_PWM);
	PWM_ConfigOutputChannel(PWM, MOTOR_3, 100, MOTO3_PWM);
	PWM_ConfigOutputChannel(PWM, MOTOR_4, 100, MOTO4_PWM);
	
	motor1PWM = MOTO1_PWM;
	motor2PWM = MOTO2_PWM;
	motor3PWM = MOTO3_PWM;
	motor4PWM = MOTO4_PWM;

}
