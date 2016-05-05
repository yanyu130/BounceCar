#ifndef RC_H
#define RC_H

#include <stdint.h>

//#define FORWARD 1
//#define BACKWARD 2
//#define CLOCK_WISE_LITTLE 4
//#define ANTICLOCK_WISE_LITTLE 8
//#define CLOCK_WISE_BIG 16
//#define ANTICLOCK_WISE_BIG 32

//#define MODEL_NORMAL 0x40
//#define MODEL_HAND_UP 0x80

/*
需要的命令
1.模式切换：正常、倒立
2.旋转：顺时针小幅度旋转、大幅度旋转
				逆时针小幅度旋转、大幅度旋转
				快速多圈旋转
3.弹跳（2种）
4.翻180度

*/
typedef struct int16_rcget
{
    uint8_t SPEED;
    uint8_t PITCH;
    uint8_t THROTTLE;
    uint8_t COMMAND;
}RC_GETDATA;

extern RC_GETDATA RC_DATA;



//初始化遥控
void RC_init(void);

//更新遥控状态
void RC_Update(void);

//遥控命令监测
void RC_CommandDetect(void);
 
#endif
