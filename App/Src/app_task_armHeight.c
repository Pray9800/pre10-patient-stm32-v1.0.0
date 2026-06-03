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

#define ARM_DEADZONE    1000  //  ADC 分辨率调整，相当于0.05V 65536/1000*3.3
#define ARM_MIN_HEIGHT  0x4100 //  暂时ADC读取情况相当于0.8v
#define ARM_MAX_HEIGHT  0xC000 //  2.475v
    #if 0
    void StartArmTask(void *argument)
    {
        for(;;)
        {
            
            // uint8_t last_key = 0;
            // uint8_t valid_key = 0;
            // 1. 获取高度 (ADC拉绳传感器实时读取)
            adc_current_height = BSP_ADC_GetArmHeight();
            
            // 2. 物理升降电机控制
            if ((ARM_UP_SIGN() == 1)) 
            {
                height_auto_mode = 0;
                MOTOR_ARM_UP();
            } 
            else if ((ARM_DOWN_SIGN() == 1)) 
            {
                MOTOR_ARM_DOWN();
                height_auto_mode = 0;
            } 
            //3.上位机升降控制
            else if (height_auto_mode == 1)  //上位机信号控制标志位包含升降信号或者高度信号
            {
                    
                    if (adc_targart_height < ARM_MIN_HEIGHT || adc_targart_height > ARM_MAX_HEIGHT || height_update_down==0)  //预设值超标直接退出 或者有网口升降停止指令
                {
                    MOTOR_ARM_STOP();
                    height_auto_mode = 0;
                    App_Printf("Error: out of range"); 
                }
                
                else //误差范围内 或者有上升下降的网口指令
                {
                    // 两个都没按停止
                    int32_t error = (int32_t)adc_targart_height - (int32_t)adc_current_height; //还有插值
                    

                    //先处升降信号
                    if (height_update_down==2 && ARM_MIN_HEIGHT<adc_current_height &&ARM_MAX_HEIGHT>adc_current_height) //实际高度不能超过阈值
                    {
                        MOTOR_ARM_DOWN();
                    }
                    else if (height_update_down==1 && ARM_MIN_HEIGHT<adc_current_height &&ARM_MAX_HEIGHT>adc_current_height) //实际高度不能超过阈值
                    {
                        MOTOR_ARM_UP();
                    }
                    //再处理高度信号
                    else if (error > ARM_DEADZONE )      // 目标在上方，且超过允许误差
                    {
                        MOTOR_ARM_UP();
                    }
                    else if (error < -ARM_DEADZONE) // 目标在下方，且超允许误差
                    {
                        MOTOR_ARM_DOWN();
                    }
                    // 
                    else                           // 进入允许误差，说明已经到达
                    {
                        MOTOR_ARM_STOP();
                        height_auto_mode = 0;       // 到达后可关闭
                        height_update_down=0;       //停止指令
                    }
                    
                }
                

            }

            else  //没有按键没有上位机
                {
                 MOTOR_ARM_STOP();
                }

            //  (50Hz) 
            osDelay(20);  //提供最快刷新频率
        }
    }
#endif

/*******************************************************
 Author: PAN       Version: V1.0       Date:2026/06/03
 Function:          StartArmTask
 Description:       机械臂高度控制任务1.物理按键控制 2.上位机连续升降指令控制 3.上位机目标定高控制
 Calls:             ARM_UP_SIGN() 、ARM_DOWN_SIGN() 
 Called By:         应用层
 Input:             height_auto_mode和height_update_down的值由上位机指令改变
 Output:            无
 Return:            无
 Others:            无
*******************************************************/
void StartArmTask(void *argument)
{
    for(;;)
    {
        // 1. 获取高度 (ADC拉绳传感器实时读取)
        adc_current_height = BSP_ADC_GetArmHeight();
        
        // ==================== 1. 物理按键优先控制 ====================
        if (ARM_UP_SIGN() == 1) 
        {
            height_auto_mode = 0;   // 退出上位机
            MOTOR_ARM_UP();
        } 
        else if (ARM_DOWN_SIGN() == 1) 
        {
            height_auto_mode = 0;   // 退出上位机
            MOTOR_ARM_DOWN();
        } 
        
        // ==================== 2. 上位机控制 ====================
        else if (height_auto_mode == 1) 
        {
            // --- 状态 A: 网络连续升降指令 (1=上升, 2=下降，0停止) ---
            if (height_update_down == 1) 
            {
                if (adc_current_height < ARM_MAX_HEIGHT) 
                {
                    MOTOR_ARM_UP(); // 限位
                } 
                else 
                {
                    MOTOR_ARM_STOP();
                }
            }
            else if (height_update_down == 2) 
            {
                if (adc_current_height > ARM_MIN_HEIGHT) 
                {
                    MOTOR_ARM_DOWN(); // 限位
                } 
                else 
                {
                    MOTOR_ARM_STOP();
                }
            }
            else if (height_update_down == 0) 
            {
                    MOTOR_ARM_STOP();
                    height_auto_mode = 0; 
            }
            
            // --- 状态 B: 网络目标定高模式   ---
            else if (height_update_down == 0xFF) // 高度控制模式
             {
                // 先校验范围
                if ((adc_targart_height < ARM_MIN_HEIGHT) || (adc_targart_height > ARM_MAX_HEIGHT)) 
                {
                    MOTOR_ARM_STOP();
                    height_auto_mode = 0; // 退出上位机
                    App_Printf("Error: Target out of range\r\n"); 
                }
                else 
                {
                    // 计算插值
                    int32_t error = (int32_t)adc_targart_height - (int32_t)adc_current_height;
                    
                    if (error > ARM_DEADZONE) 
                    {
                        MOTOR_ARM_UP();
                    }
                    else if (error < -ARM_DEADZONE) 
                    {
                        MOTOR_ARM_DOWN();
                    }
                    else 
                    {
                        // 误差进入死区内，说明到达了指定位置
                        MOTOR_ARM_STOP();      
                        height_auto_mode = 0;  
                    }
                }
            }
            
        }
        
        // ==================== 3. 待机状态 ====================
        else 
        {
            MOTOR_ARM_STOP(); // 没有按键，且网络没发指令 
        }

        // 50Hz 控制周期
        osDelay(20); 
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


