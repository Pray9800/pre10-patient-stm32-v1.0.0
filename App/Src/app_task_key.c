/*******************************************************
 Copyright (C), HangZhou Jianjia Co.,Ltd.
 File name:         app_task_key.c
 Author: PENG       Version: V1.0       Date:2026/03/26
 Description:
 Function List:
 History:
*******************************************************/
#include "app_task.h"
#include "bsp_gpio.h"
#include "bsp_usart.h"



/*******************************************************
 Author: PAN       Version: V2.0       Date:2026/6/3
 Function:          StartKeyTask
 Description:       按键任务 (状态机防抖 + 边沿触发) 
                    主要用于U1传输调试时候知道已经按下与其他任务不冲突后续将注释
*******************************************************/
void StartKeyTask(void *argument)
{
    // 定义状态机变量：0表示未按下，1表示已按下
    static uint8_t key_state[6] = {0}; 
    // 防抖计数器
    static uint8_t key_cnt[6] = {0};   
    
    //  40ms 确认
    const uint8_t DEBOUNCE_TICKS = 2;  

    osDelay(3000);    // 等待其他任务初始化完成

    while(1)
    {
        // ==========================================
        // KEY1 - 机械臂上升 (高电平触发)
        // ==========================================
        if (KEY1_READ() == GPIO_PIN_SET) {
            if (key_state[1] == 0) {       // 之前是松开状态
                if (++key_cnt[1] >= DEBOUNCE_TICKS) {
                    key_state[1] = 1;      // 标记为已按下（锁定，防刷屏）
                    // --- 2. 软件事件反馈 ---
                    App_Printf("KeyTask: Key1 Pressed (Arm UP)\r\n");
                    if (keyEventHandle != NULL) osEventFlagsSet(keyEventHandle, K1_EVENT);
                }
            }
        } else {
            // 按键松开，立刻重置状态和计数器，为下一次按下做准备
            key_state[1] = 0;
            key_cnt[1] = 0;
        }
        // ==========================================
        // KEY2 - 机械臂下降 (高电平触发)
        // ==========================================
        if (KEY2_READ() == GPIO_PIN_SET) {
            if (key_state[2] == 0) {
                if (++key_cnt[2] >= DEBOUNCE_TICKS) {
                    key_state[2] = 1;
                    
                    App_Printf("KeyTask: Key2 Pressed (Arm DOWN)\r\n");
                    // 硬件反馈：比如蜂鸣器短鸣
                    if (keyEventHandle != NULL) osEventFlagsSet(keyEventHandle, K2_EVENT);
                }
            }
        } else {
            key_state[2] = 0;
            key_cnt[2] = 0;
        }
        // ==========================================
        // KEY3 - 台车移动使能 (高电平触发)
        // ==========================================
        if (KEY3_READ() == GPIO_PIN_SET) {
            if (key_state[3] == 0) {
                if (++key_cnt[3] >= DEBOUNCE_TICKS) {
                    key_state[3] = 1;
                    
                    App_Printf("KeyTask: Key3 Pressed (Move Enable)\r\n");
                    if (keyEventHandle != NULL) osEventFlagsSet(keyEventHandle, K3_EVENT);
                }
            }
        } else {
            key_state[3] = 0;
            key_cnt[3] = 0;
        }

        // ==========================================
        // KEY5 - 驻车按键 (高电平触发)
        // ==========================================
        if (KEY5_READ() == GPIO_PIN_SET) {
            if (key_state[5] == 0) {
                if (++key_cnt[5] >= DEBOUNCE_TICKS) {
                    key_state[5] = 1;
                    
                    App_Printf("KeyTask: Key5 Pressed (Park)\r\n");
                    if (keyEventHandle != NULL) osEventFlagsSet(keyEventHandle, K5_EVENT);
                }
            }
        } else {
            key_state[5] = 0;
            key_cnt[5] = 0;
        }

        // ==========================================
        // 核心轮询节拍 (取代之前的 osEventFlagsWait)
        // ==========================================
        osDelay(20); // 每 20ms 扫描一次按键，这是最佳的按键防抖基准心跳
    }
}