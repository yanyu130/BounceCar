#ifndef IMUSO3_H
#define IMUSO3_H

//! Auxiliary variables to reduce number of repeated operations
static float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;	/** quaternion of sensor frame relative to auxiliary frame */
static float dq0 = 0.0f, dq1 = 0.0f, dq2 = 0.0f, dq3 = 0.0f;	/** quaternion of sensor frame relative to auxiliary frame */
static float gyro_bias[3] = {0.0f, 0.0f, 0.0f}; /** bias estimation */
static float q0q0, q0q1, q0q2, q0q3;
static float q1q1, q1q2, q1q3;
static float q2q2, q2q3;
static float q3q3;

#define so3_comp_params_Kp 1.0f
#define so3_comp_params_Ki  0.05f



void IMUSO3Thread(void);


void NonlinearSO3AHRSupdate(float gx, float gy, float gz, 
	float ax, float ay, float az, 
		float mx, float my, float mz, 
			float twoKp, float twoKi, float dt);

 
#endif
