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

// 脚踏按键状态读取
#define KEY1_READ()          HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin)
#define KEY2_READ()          HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin)
#define KEY3_READ()          HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin)
#define KEY4_READ()          HAL_GPIO_ReadPin(KEY4_GPIO_Port, KEY4_Pin)
#define KEY5_READ()          HAL_GPIO_ReadPin(KEY5_GPIO_Port, KEY5_Pin)
#define KEY6_READ()          HAL_GPIO_ReadPin(KEY6_GPIO_Port, KEY6_Pin)
#define KEY7_READ()          HAL_GPIO_ReadPin(KEY7_GPIO_Port, KEY7_Pin)
#define KEY8_READ()          HAL_GPIO_ReadPin(KEY8_GPIO_Port, KEY8_Pin)

// 按键事件
#define K1_EVENT            (1UL << 0)      // 按键1按下事件
#define K2_EVENT            (1UL << 1)      // 按键2按下事件
#define K3_EVENT            (1UL << 2)      // 按键3按下事件
#define K4_EVENT            (1UL << 3)      // 按键4按下事件
#define K5_EVENT            (1UL << 4)      // 按键5按下事件
#define K6_EVENT            (1UL << 5)      // 按键6按下事件
#define K7_EVENT            (1UL << 6)      // 按键7按下事件
#define K8_EVENT            (1UL << 7)      // 按键
#define KEY_TIME_MS         20              // 防抖时间（毫秒）
extern osEventFlagsId_t     keyEventHandle; // 按键事件标志句柄

#endif /* __BSP_GPIO_H_ */
