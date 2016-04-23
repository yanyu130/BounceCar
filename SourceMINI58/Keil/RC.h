#ifndef RC_H
#define RC_H

//RC遥控
typedef struct int16_rcget
{
    float ROOL;
    float PITCH;
    float THROTTLE;
    float YAW;
}RC_GETDATA;

extern RC_GETDATA RC_DATA;//经过处理的RC数据

 
#endif
