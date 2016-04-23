#ifndef MOTOR_H
#define MOTOR_H
#include "stdint.h"

extern int16_t motor1PWM, motor2PWM, motor3PWM, motor4PWM; 

void Motor_Init(void);
void Motor_Start(void);
void Motor_Stop(void);
void MotorPwmOutput(int16_t MOTO1_PWM,int16_t MOTO2_PWM,int16_t MOTO3_PWM,int16_t MOTO4_PWM);
//void MotorCtrl(void);
//void MotorPwmOutput2(void);

#endif
