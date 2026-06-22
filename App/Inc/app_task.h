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
 * ============================================================
 *                      串口资源分配
 * ============================================================
 *  USART1  →  调试打印 (App_Printf)
 *  USART2  →  左驱动轮 RS232 (IDS830 伺服)
 *  USART3  →  力矩传感器 RS232 (BSQ-JN-P4 Modbus)
 *  UART4   →  右驱动轮 RS232 (IDS830 伺服)
 *  USART6  →  网络通信 Modbus RTU (上位机)
 * ============================================================
 *
 * ============================================================
 *                   UART6 网络命令 (上位机 ↔ STM32)
 * ============================================================
 *  0x10 : 心跳
 *  0x11 : UPS 通断控制 (读/写)
 *  0x12 : 灯带控制 WS2812 (读/写)
 *  0x13 : 驻车状态读取 (只读)
 *  0x14 : 抱闸控制 (读/写)
 *  0x15 : 机械臂目标高度设定 (只写)
 *  0x16 : 机械臂升降指令 (只写: 1=升 2=降 0=停)
 * ============================================================
 *
 * ============================================================
 *                  本地按键 GPIO 输入 (只本地)
 * ============================================================
 *  KEY1  →  机械臂上升
 *  KEY2  →  机械臂下降
 *  KEY3  →  台车移动使能 (按下推车)
 *  KEY5  →  驻车推杆 伸出/收回
 *  STOP  →  急停 (UPS 强制断电)
 * ============================================================
 *
 * ============================================================
 *                  FreeRTOS 任务优先级 (高 → 低)
 * ============================================================
 *  comTask         : AboveNormal  — 串口事件分发 (UART1-6)
 *  torqueMoveTask  : High         — 力矩→速度转换 + 电机控制
 *  armTask         : Normal       — 机械臂升降控制
 *  keyTask         : Low1         — 按键调试 (调试用, 可裁剪)
 *  defaultTask     : Low          — LED闪烁 + 看门狗喂狗
 *  upsTask         : Low          — UPS通断
 *  parkTask        : BelowNormal  — 驻车推杆伸缩
 *  breakTask       : BelowNormal7 — 抱闸控制
 * ============================================================
 *
 * ============================================================
 *                  RTOS 资源一览
 * ============================================================
 *  [互斥锁]
 *    printfMutexHandle  — 保护 App_Printf 输出, 防止乱序
 *
 *  [事件标志组]
 *    comEventHandle     — UART 接收事件
 *        EVENT_UART6_RX (1<<0)  上位机网络指令到达
 *        EVENT_UART3_RX (1<<1)  力矩传感器数据到达
 *        EVENT_UART2_RX (1<<2)  左轮伺服应答到达
 *        EVENT_UART4_RX (1<<3)  右轮伺服应答到达
 *    keyEventHandle     — 按键事件 (调试用)
 *        K1_EVENT ~ K8_EVENT (1<<0 ~ 1<<7)
 *    ArmBKEventHandle   — 抱闸事件
 *        EVENT_BRAKE_UPDATE (0x01)
 *
 *  [任务通知 (Thread Flags)]
 *    torqueMoveTaskHandle 接收:
 *        FLAG_TORQUE_READY (1<<0)  — comTask 力矩解析完成
 *        FLAG_U2_ACK_READY (1<<1)  — 左轮伺服 ACK
 *        FLAG_U4_ACK_READY (1<<2)  — 右轮伺服 ACK
 *    upsTaskHandle 接收:
 *        FLAG_UPS_UPDATE   (1<<0)  — UART6/UPS 指令或急停触发
 * ============================================================
 */
 