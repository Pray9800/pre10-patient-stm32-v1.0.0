/*******************************************************
 Copyright (C), HangZhou Jianjia Co.,Ltd.
 File name:         bsp_gpio.h
 Author: PENG       Version: V1.0       Date:2026/03/26
 Description:       GPIO配置头文件

 Function List:
 History:
*******************************************************/

#ifndef __BSP_GPIO_H_
#define __BSP_GPIO_H_

#include "main.h"
#include "gpio.h"
#include "cmsis_os2.h"


// 运行状态指示灯控制
#define LED0_ON()   		HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET)
#define LED0_OFF()   		HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET)
#define LED0_TOGGLE()		HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin)

// 光电开关状态读取
#define PS1_READ()          HAL_GPIO_ReadPin(PS1_GPIO_Port, PS1_Pin)
#define PS2_READ()          HAL_GPIO_ReadPin(PS2_GPIO_Port, PS2_Pin)
#define PS3_READ()          HAL_GPIO_ReadPin(PS3_GPIO_Port, PS3_Pin)
#define PS4_READ()          HAL_GPIO_ReadPin(PS4_GPIO_Port, PS4_Pin)
//暂定把PS4 替换老代码的KEY_PEAK引脚  GET_KEY_PARK采用原代码命名方式
#define GET_KEY_PARK()           KEY5_READ()  





// 脚踏按键状态读取
#define KEY1_READ()          HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin)
#define KEY2_READ()          HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin)
#define KEY3_READ()          HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin)
#define KEY5_READ()          HAL_GPIO_ReadPin(KEY5_GPIO_Port, KEY5_Pin)
#define KEY9_READ()          HAL_GPIO_ReadPin(KEY9_GPIO_Port, KEY9_Pin)
#define KEY10_READ()         HAL_GPIO_ReadPin(KEY10_GPIO_Port, KEY10_Pin)
#define KEY11_READ()         HAL_GPIO_ReadPin(KEY11_GPIO_Port, KEY11_Pin)

    

// 按键事件
#define K1_EVENT            (1UL << 0)      // 按键1按下事件
#define K2_EVENT            (1UL << 1)      // 按键2按下事件
#define K3_EVENT            (1UL << 2)      // 按键3按下事件
#define K4_EVENT            (1UL << 3)      // 按键4按下事件
#define K5_EVENT            (1UL << 4)      // 按键5按下事件
#define K6_EVENT            (1UL << 5)      // 按键6按下事件
#define K7_EVENT            (1UL << 6)      // 按键7按下事件
#define K8_EVENT            (1UL << 7)      // 按键8按下事件
#define KEY_TIME_MS         20              // 防抖时间（毫秒）

//UPS急停按钮读取
#define STOP_READ()              HAL_GPIO_ReadPin(STOP_GPIO_Port, STOP_Pin)

// UPS控制引脚
#define UPS_ON_SET()             HAL_GPIO_WritePin(UPS_ON_GPIO_Port, UPS_ON_Pin, GPIO_PIN_SET)
#define UPS_ON_RESET()           HAL_GPIO_WritePin(UPS_ON_GPIO_Port, UPS_ON_Pin, GPIO_PIN_RESET)
#define UPS_OFF_SET()            HAL_GPIO_WritePin(UPS_OFF_GPIO_Port, UPS_OFF_Pin, GPIO_PIN_SET)
#define UPS_OFF_RESET()          HAL_GPIO_WritePin(UPS_OFF_GPIO_Port, UPS_OFF_Pin, GPIO_PIN_RESET)


// 机械臂上升 (电机正转：0拉高，1拉低)
#define MOTOR_ARM_UP() \
{ \
    HAL_GPIO_WritePin(UPDOWN_0_GPIO_Port, UPDOWN_0_Pin, GPIO_PIN_SET); \
    HAL_GPIO_WritePin(UPDOWN_1_GPIO_Port, UPDOWN_1_Pin, GPIO_PIN_RESET); \
}

// 机械臂下降 (电机反转：0拉低，1拉高)
#define MOTOR_ARM_DOWN() \
{ \
    HAL_GPIO_WritePin(UPDOWN_0_GPIO_Port, UPDOWN_0_Pin, GPIO_PIN_RESET); \
    HAL_GPIO_WritePin(UPDOWN_1_GPIO_Port, UPDOWN_1_Pin, GPIO_PIN_SET); \
}

// 机械臂停止 (电机刹车/释放：双双拉低)
#define MOTOR_ARM_STOP() \
{ \
    HAL_GPIO_WritePin(UPDOWN_0_GPIO_Port, UPDOWN_0_Pin, GPIO_PIN_RESET); \
    HAL_GPIO_WritePin(UPDOWN_1_GPIO_Port, UPDOWN_1_Pin, GPIO_PIN_RESET); \
}


//推杠控制的六个GPIO引脚  同时组合成的顶起回收功能已经
#define M1_0_ON()           HAL_GPIO_WritePin(CTL_M1_0_GPIO_Port, CTL_M1_0_Pin, GPIO_PIN_SET); // M1_0拉高
#define M1_0_OFF()          HAL_GPIO_WritePin(CTL_M1_0_GPIO_Port, CTL_M1_0_Pin, GPIO_PIN_RESET) // M1_0拉低
#define M1_1_ON()           HAL_GPIO_WritePin(CTL_M1_1_GPIO_Port, CTL_M1_1_Pin, GPIO_PIN_SET) // M1_1拉高
#define M1_1_OFF()          HAL_GPIO_WritePin(CTL_M1_1_GPIO_Port, CTL_M1_1_Pin, GPIO_PIN_RESET) // M1_1拉低
#define M2_0_ON()           HAL_GPIO_WritePin(CTL_M2_0_GPIO_Port, CTL_M2_0_Pin, GPIO_PIN_SET) // M2_0拉   
#define M2_0_OFF()          HAL_GPIO_WritePin(CTL_M2_0_GPIO_Port, CTL_M2_0_Pin, GPIO_PIN_RESET) // M2_0拉低
#define M2_1_ON()           HAL_GPIO_WritePin(CTL_M2_1_GPIO_Port, CTL_M2_1_Pin, GPIO_PIN_SET) // M2_1拉高
#define M2_1_OFF()          HAL_GPIO_WritePin(CTL_M2_1_GPIO_Port, CTL_M2_1_Pin, GPIO_PIN_RESET) // M2_1拉低
#define M3_0_ON()           HAL_GPIO_WritePin(CTL_M3_0_GPIO_Port, CTL_M3_0_Pin, GPIO_PIN_SET) // M3_0拉高
#define M3_0_OFF()          HAL_GPIO_WritePin(CTL_M3_0_GPIO_Port, CTL_M3_0_Pin, GPIO_PIN_RESET) // M3_0拉低
#define M3_1_ON()           HAL_GPIO_WritePin(CTL_M3_1_GPIO_Port, CTL_M3_1_Pin, GPIO_PIN_SET) // M3_1拉高
#define M3_1_OFF()          HAL_GPIO_WritePin(CTL_M3_1_GPIO_Port, CTL_M3_1_Pin, GPIO_PIN_RESET) // M3_1拉低


//驻车的四个GPIO引脚 FOOTP   

#define FOOTP_0_ON()          HAL_GPIO_WritePin(FOOTP_0_GPIO_Port, FOOTP_0_Pin, GPIO_PIN_SET) // FOOTP_0_ON拉高
#define FOOTP_0_OFF()         HAL_GPIO_WritePin(FOOTP_0_GPIO_Port, FOOTP_0_Pin, GPIO_PIN_RESET) // FOOTP_0_ON拉低
#define FOOTP_1_ON()          HAL_GPIO_WritePin(FOOTP_1_GPIO_Port, FOOTP_1_Pin, GPIO_PIN_SET) // FOOTP_1_ON拉高
#define FOOTP_1_OFF()         HAL_GPIO_WritePin(FOOTP_1_GPIO_Port, FOOTP_1_Pin, GPIO_PIN_RESET) // FOOTP_1_ON拉低
#define FOOTP_2_ON()          HAL_GPIO_WritePin(FOOTP_2_GPIO_Port, FOOTP_2_Pin, GPIO_PIN_SET) // FOOTP_2_ON拉高
#define FOOTP_2_OFF()         HAL_GPIO_WritePin(FOOTP_2_GPIO_Port, FOOTP_2_Pin, GPIO_PIN_RESET) // FOOTP_2_ON拉低
#define FOOTP_3_ON()          HAL_GPIO_WritePin(FOOTP_3_GPIO_Port, FOOTP_3_Pin, GPIO_PIN_SET) // FOOTP_3_ON拉高
#define FOOTP_3_OFF()         HAL_GPIO_WritePin(FOOTP_3_GPIO_Port, FOOTP_3_Pin, GPIO_PIN_RESET) // FOOTP_3_ON拉低




// 台车移动使能信号  接收端
#define Trolley_Move()          KEY3_READ()  //1：使能电机推动 0：关闭使能电机推动

//机械臂上下按钮信号 接收端
#define ARM_UP_SIGN()            KEY1_READ()
#define ARM_DOWN_SIGN()          KEY2_READ()


extern osEventFlagsId_t     keyEventHandle; // 按键事件标志句柄


// 驻车推杆电机控制接口
void BSP_ParkMotors_Extend(void);  // 顶起 (对应老代码 case 2)
void BSP_ParkMotors_Retract(void); // 收回直线 (对应老代码 case 4)
void BSP_ParkMotors_Stop(void);    // 全停止 (对应老代码 case 3)

//抱闸控制由于是OO FF来实现控制 因此选择.c文件里面去分装
// 抱闸控制接口 (输入协议解析值)
void BSP_Brake_Control(uint8_t reg_val);




// 台车移动使能信号

#endif /* __BSP_GPIO_H_ */
