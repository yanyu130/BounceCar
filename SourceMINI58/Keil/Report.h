#ifndef _REPORT_H
#define _REPORT_H

#define REPORT_AHRS_EULER         1 
#define REPORT_AHRS_QUATERNION    2 
#define REPORT_SENSORS_CALIBRATED 3 
#define REPORT_SENSORS_RAW        4 
#define REPORT_MOTOR_POWER        5 
#define REPORT_ANGLE_PID          6
#define REPORT_RATE_PID           7
#define REPORT_VELOCITY           8
#define REPORT_RC_STATUS          9
#define REPORT_ALTHOLD_PID       10
#define REPORT_ALTHOLD_STATUS    11
#define REPORT_MODE_STATUS       12
#define REPORT_FLASH_STATUS      13

extern char report_mode;

void CheckVersion(void);
void ReportMessage(void);
#endif
