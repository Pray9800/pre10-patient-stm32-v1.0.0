/*******************************************************
 Copyright (C), HangZhou Jianjia Co.,Ltd.
 File name:         app_task_break.c
 Author: PAN       Version: V1.0       Date:2026/04/23
 Description: 抱闸控制任务
 Function List: 
 * - GPIO输出控制抱闸电机
 History:
*******************************************************/

#include "app_task.h"
#include "bsp_gpio.h"


/* ====================================================================
 * 任务：抱闸控制任务 (事件驱动型)
 * 触发：由 UART6 收到网络指令后，通过 EVENT_BRAKE_UPDATE 唤醒
 * ==================================================================== */
void StartBrakeTask(void *argument)
{

    uint8_t last_brake_state = 0xFF; 
    
    // 初始化抱闸
    BSP_Brake_Control(last_brake_state);

    for(;;)
    {
        // 阻塞等待UART6传来的信号
        osEventFlagsWait(ArmBKEventHandle, EVENT_BRAKE_UPDATE, osFlagsWaitAny, osWaitForever);

        // 被唤醒后执行动作
        if (g_brake_state_ctrl != last_brake_state) 
        {
            BSP_Brake_Control(g_brake_state_ctrl);
            last_brake_state = g_brake_state_ctrl; // 更新记录
        }
    }
      
}