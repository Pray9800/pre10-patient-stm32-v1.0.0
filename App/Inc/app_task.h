/*******************************************************
 Copyright (C), HangZou Jianjia Co.,Ltd.
 File name:			app_task.h
 Author: PENG		Version: V1.0		Date:2026/03/26
 Description:    	线程配置头文件
 Function List:
 History:
*******************************************************/

#ifndef __APP_TASK_H_
#define __APP_TASK_H_

#include "main.h"
#include "cmsis_os.h"
#include "cmsis_os2.h"


// #define DEBUG_TASK
// #define DEBUG_STACK

// 软件版本
#define VER_X    1
#define VER_Y    0

#define TASK_TIME        100  // 任务循环时间 单位 ms

// 任务句柄
// extern osThreadId_t comTaskHandle;
extern osThreadId_t defaultTaskHandle;

// 指示灯状态
#define LED_ON           1    // 常亮
#define LED_OFF          2    // 常灭
#define LED_BLINK        3    // 闪烁

extern IWDG_HandleTypeDef hiwdg1;

/*******************************************************
 串口通信相关变量
*******************************************************/
// 串口事件
// #define COMRX_EVENT   (1UL << 0)    // 在bsp_usart.c中定义
// #define TEST_EVENT       (1UL << 1)    // 升高事件

// extern osEventFlagsId_t comEventHandle;

// 指令
#define CMD_TEST         0xA1


#endif /* __APP_TASK_H_ */
