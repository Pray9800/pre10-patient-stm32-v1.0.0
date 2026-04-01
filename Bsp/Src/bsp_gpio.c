/*******************************************************
 Copyright (C), HangZhou Jianjia Co.,Ltd.
 File name:         bsp_gpio.c
 Author: PENG       Version: V1.0       Date:2026/03/26
 Description:       GPIO配置文件

 Function List:     HAL_GPIO_EXTI_Callback
                    Key_Scan_Direct
 History:
*******************************************************/

#include "bsp_gpio.h"


/*******************************************************
 Author: PENG       Version: V1.0       Date:2026/03/26
 Function:          HAL_GPIO_EXTI_Callback
 Description:       外部中断回调函数  按键中断处理
 Calls:             osEventFlagsSet
 Called By:         HAL库
 Input:             GPIO_Pin 中断引脚
 Output:            无
 Return:            无
 Others:            无
*******************************************************/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    static uint32_t LastTime[8] = {0};  // 记录每个按键上次触发时间
    uint32_t curTime = HAL_GetTick();
    uint32_t diffTime;

    if (GPIO_Pin == KEY1_Pin)
    {
        diffTime = curTime - LastTime[0];
        if (diffTime >= KEY_TIME_MS) {
            LastTime[0] = curTime;
            if (keyEventHandle != NULL) {
                osEventFlagsSet(keyEventHandle, K1_EVENT);
            }
        }   // 设置按键1事件标志
    }
    else if (GPIO_Pin == KEY2_Pin)
    {
        diffTime = curTime - LastTime[1];
        if (diffTime >= KEY_TIME_MS) {
            LastTime[1] = curTime;
            if (keyEventHandle != NULL) {
                osEventFlagsSet(keyEventHandle, K2_EVENT);
            }
        }   // 设置按键2事件标志
    }
    else if (GPIO_Pin == KEY3_Pin)
    {
        diffTime = curTime - LastTime[2];
        if (diffTime >= KEY_TIME_MS) {
            LastTime[2] = curTime;
            if (keyEventHandle != NULL) {
                osEventFlagsSet(keyEventHandle, K3_EVENT);
            }
        }   // 设置按键3事件标志
    }
    else if (GPIO_Pin == KEY4_Pin)
    {
        diffTime = curTime - LastTime[3];
        if (diffTime >= KEY_TIME_MS) {
            LastTime[3] = curTime;
            if (keyEventHandle != NULL) {
                osEventFlagsSet(keyEventHandle, K4_EVENT);
            }
        }   // 设置按键4事件标志
    }
    else if (GPIO_Pin == KEY5_Pin)
    {
        diffTime = curTime - LastTime[4];
        if (diffTime >= KEY_TIME_MS) {
            LastTime[4] = curTime;
            if (keyEventHandle != NULL) {
                osEventFlagsSet(keyEventHandle, K5_EVENT);
            }
        }   // 设置按键5事件标志
    }
    else if (GPIO_Pin == KEY6_Pin)
    {
        diffTime = curTime - LastTime[5];
        if (diffTime >= KEY_TIME_MS) {
            LastTime[5] = curTime;
            if (keyEventHandle != NULL) {
                osEventFlagsSet(keyEventHandle, K6_EVENT);
            }
        }   // 设置按键6事件标志
    }
    else if (GPIO_Pin == KEY7_Pin)
    {
        diffTime = curTime - LastTime[6];
        if (diffTime >= KEY_TIME_MS) {
            LastTime[6] = curTime;
            if (keyEventHandle != NULL) {
                osEventFlagsSet(keyEventHandle, K7_EVENT);
            }
        }   // 设置按键7事件标志
    }
    else if (GPIO_Pin == KEY8_Pin)
    {
        diffTime = curTime - LastTime[7];
        if (diffTime >= KEY_TIME_MS) {
            LastTime[7] = curTime;
            if (keyEventHandle != NULL) {
                osEventFlagsSet(keyEventHandle, K8_EVENT);
            }
        }   // 设置按键8事件标志
    }

    else
    {
        // 其他中断引脚处理
    }
}
