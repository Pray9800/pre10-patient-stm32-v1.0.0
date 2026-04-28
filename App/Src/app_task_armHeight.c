/*******************************************************
 Copyright (C), HangZou Jianjia Co.,Ltd.
 File name:         app_task_arm.c
 Author: PAN       Version: V1.0       Date:2026/04/27
 Description:       机械臂控制任务
 Function List:
  * * 控制物理高度的获取（ADC拉绳传感器）。
  * * 根据UART6指令控制抱闸状态（GPIO输出）和升降电机（GPIO输出）。
 History:
*******************************************************/



#include "app_task.h"
#include "bsp_gpio.h"
#include "bsp_adc.h"
#include "bsp_usart.h"



void StartArmTask(void *argument)
{
    for(;;)
    {
        
        uint8_t last_key = 0;
        uint8_t valid_key = 0;
        // 1. 获取高度 (ADC拉绳传感器实时读取)
        adc_current_height = BSP_ADC_GetArmHeight();
       
        // 2. 升降电机控制
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

        //  (50Hz) 
        osDelay(20);  //提供最快刷新频率
    }
}







/* * 消抖 后续使用
 * current_state : 当前瞬间读到的电平 (0 或 1)
 * p_last_state  : 存放上一次电平的变量地址
 * p_valid_state : 存放最终有效电平的变量地址
 */
uint8_t Key_Debounce(uint8_t current_state, uint8_t *p_last_state, uint8_t *p_valid_state)
{
    // 如果当前的电平，跟 20ms 前的电平一模一样，说明没抖动，是有效按下/松开
    if (current_state == *p_last_state) 
    {
        *p_valid_state = current_state;
    }
    
    // 把现在的电平记住，留给下个 20ms 去对比
    *p_last_state = current_state;
    
    return *p_valid_state;
}


