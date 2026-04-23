/* app_task_arm.c */
#include "app_task.h"
#include "bsp_gpio.h"
#include "bsp_adc.h"
#include "bsp_usart.h"


// //控制机械臂高度 和机械臂是否抱闸
// void StartArmTask(void *argument)
// {
//    uint16_t adc_current_height = 0; 

//     for(;;)
//     {
        
//         // 获取当前机械臂真实高度 (ADC拉绳传感器)
//         adc_current_height = BSP_ADC_GetArmHeight();

//         // 2. 核心控制逻辑 (完美复刻你的老程序，并加入限位保护)
//         if (ARM_UP_SIGN() == 1) // 假设读到 1 是按下
//         {
//             // adc 相关作用于机械臂上下的限位保护，防止电机过度运行损坏设备 暂不确定
//             // if (adc_current_height < 60000) { 
//             //     MOTOR_ARM_UP();
//             // } else {
//             //     MOTOR_ARM_STOP(); // 到顶了，强制刹车
//             // }

//             MOTOR_ARM_UP()
//         }
//         else if (ARM_DOWN_SIGN() == 1) 
//         {
//             MOTOR_ARM_DOWN();
//         }
//         else 
//         {
//             // 两个都没按停止！
//             MOTOR_ARM_STOP();
//         }
//         // 消抖
//         osDelay(20);
//     }
// }

 

void StartArmTask(void *argument)
{
    uint16_t adc_current_height = 0; 
    
    // 记录上一次的物理抱闸状态，避免重复刷 GPIO
    uint8_t last_brake_state = 0xFF; 
    
    // 初始化抱闸 后面解锁使用
    BSP_Brake_Control(last_brake_state);

    for(;;)
    {
        // 1. 获取高度 (ADC拉绳传感器)
        adc_current_height = BSP_ADC_GetArmHeight();

        // ========================================================
        // 模块 A：抱闸控制 (独立运行)
        // ========================================================
        if (g_brake_state_ctrl != last_brake_state) 
        {
            BSP_Brake_Control(g_brake_state_ctrl);
            last_brake_state = g_brake_state_ctrl; // 更新记录
        }

        // ========================================================
        // 模块 B：升降电机控制 (完全独立，解除锁死)
        // ========================================================
        if (ARM_UP_SIGN() == 1) 
        {
            MOTOR_ARM_UP();
        }
        else if (ARM_DOWN_SIGN() == 1) 
        {
            MOTOR_ARM_DOWN();
        }
        else 
        {
            // 两个都没按停止
            MOTOR_ARM_STOP();
        }

        // ========================================================
        // 核心灵魂：带有 20ms 超时的事件等待 (完美替代 osDelay)
        // ========================================================
        // 如果 COM 任务来了抱闸指令，瞬间(0ms)唤醒，立刻执行上面的【模块A】！
        // 如果没指令，干等 20ms 唤醒，刚好给升降的按键做消抖！
        osEventFlagsWait(ArmBKEventHandle, EVENT_BRAKE_UPDATE, osFlagsWaitAny, 20);
    }
}















//消抖20ms 备用代码 源代码没有消抖功能暂时不加
// void StartArmTask(void *argument)
// {
//     uint16_t adc_current_height = 0; 
    
//     // 用于简单消抖的历史状态记录
//     uint8_t last_up_state = 0;
//     uint8_t last_down_state = 0;

//     for(;;)
//     {
//         // 1. 获取当前机械臂真实高度 (ADC拉绳传感器)
//         adc_current_height = BSP_ADC_GetArmHeight();

//         // 读一下当前的物理引脚状态
//         uint8_t current_up = ARM_UP_SIGN();
//         uint8_t current_down = ARM_DOWN_SIGN();

//         // 2. 核心控制逻辑
//         // 【优先级第一名】：只要上升键被按下了，管你下降键按没按，统统走上升逻辑
//         if (current_up == 1) 
//         {
//             // 极简消抖：这次读到是 1，且上一次（20ms前）读到也是 1，才算真按下
//             if (last_up_state == 1) {
//                 MOTOR_ARM_UP();
//             }
//         }
//         // 【优先级第二名】：没按上升键，且按了下降键
//         else if (current_down == 1) 
//         {
//             // 极简消抖
//             if (last_down_state == 1) {
//                 MOTOR_ARM_DOWN();
//             }
//         }
//         // 【优先级最低】：都没按，或者刚刚松手
//         else 
//         {
//             MOTOR_ARM_STOP();
//         }

//         // 3. 更新历史状态，留给下一次循环做消抖对比
//         last_up_state = current_up;
//         last_down_state = current_down;

//         // ==========================================
//         // 系统的 20ms 心跳延时，刚好作为消抖的时间间隔
//         // ==========================================
//         osDelay(20);
//     }
// }