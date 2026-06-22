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
#define debug_t  0
#define TASK_TIME        100  // 任务循环时间 单位 ms


//抱闸解码事件
#define EVENT_BRAKE_UPDATE    0x01

// 驱动轮力矩控制专用通知 (发给 torqueMoveTaskHandle)
#define FLAG_TORQUE_READY       (1UL << 0)  // 0x01: 双侧力矩数据解析完成
#define FLAG_U2_ACK_READY       (1UL << 1)  // 0x02: 驱动轮1 (左轮) 指令应答完成
#define FLAG_U4_ACK_READY       (1UL << 2)  // 0x04: 驱动轮2 (右轮) 指令应答完成


//UPS状态更新(发给 torqueMoveTaskHandle)
#define FLAG_UPS_UPDATE         (1UL << 0)



// 任务句柄
// extern osThreadId_t comTaskHandle;
extern osThreadId_t defaultTaskHandle;  //默认任务句柄
extern osThreadId_t torqueMoveTaskHandle; //力矩控制移动句柄
extern osThreadId_t upsTaskHandle;    //UPS供电控制句柄


// 指示灯状态
#define LED_ON           1    // 常亮
#define LED_OFF          2    // 常灭
#define LED_BLINK        3    // 闪烁



#define UPS_ON            1    // UPS开启
#define UPS_OFF           0    // UPS关闭



// 定义系统状态结构体 暂时只有力矩数据直接 预备用于模式
typedef struct {
    int32_t Torque[2];       // [0]:右力矩, [1]:左力矩 
// 暂时就应该 后续加
} SystemMsg_t;

extern SystemMsg_t SysMsg;
extern IWDG_HandleTypeDef hiwdg1;

/*******************************************************
 串口通信相关变量
*******************************************************/
// 串口事件
// #define EVENT_UART6_RX  (1UL << 0)    // 在bsp_usart.c中定义
// #define TEST_EVENT       (1UL << 1)    // 升高事件

// extern osEventFlagsId_t comEventHandle;

extern osEventFlagsId_t ArmBKEventHandle; //抱闸事件标志
 




//UART6网口所接收的指令
extern volatile uint8_t g_brake_state_ctrl; // 全局变量：抱闸状态控制
extern volatile uint8_t g_park_state_ctrl;  // 全局变量：驻车状态控制
extern volatile uint8_t g_ups_state_ctrl ;
extern volatile uint8_t light_reg[4]; //灯带状态寄存器数据区
extern volatile uint16_t  adc_current_height; 
extern volatile uint16_t  adc_targart_height  ; 
extern volatile uint8_t  height_auto_mode  ; // 高度自动模式标志位
extern volatile uint8_t    height_update_down; //升降指令

void FOOTP_Ctrl(uint8_t state);
void App_UPS_Request(uint8_t req_state);
#endif /* __APP_TASK_H_ */


/**
 * ┌─────────────────────────────────────────────────────────┐
│                    UART6 网络命令                        │
│     0x10(心跳) 0x11(UPS-未实现) 0x12(灯带)              │
│     0x13(踏板) 0x14(抱闸) 0x15(状态上传)                │
└─────────────────────────────────────────────────────────┘
                           │
        ┌──────────────────┼──────────────────┐
        ▼                  ▼                  ▼
    灯带控制          踏板控制             抱闸控制
   (WS2812)         (4路GPIO)            (PC13)
   ✅ 只网络          ✅ 只网络            ✅ 只网络


┌─────────────────────────────────────────────────────────┐


│              本地按钮 GPIO 输入                           │
│   KEY_UP/DOWN(升降) KEY_PARK(驻车) KEY_MOVE(运动启停)    │
└─────────────────────────────────────────────────────────┘
        │                  │                  │
        ▼                  ▼                  ▼
    升降电机           推杆方向          伺服启停
    ✅ 只本地          ✅ 只本地         ✅ 只本地
 */

 //专门高度测试
 