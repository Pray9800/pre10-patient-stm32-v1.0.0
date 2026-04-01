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
 Author: PENG       Version: V1.0       Date:2026/03/26
 Function:          StartKeyTask
 Description:       按键任务
 Calls:
 Called By:
 Input:
 Output:
 Return:
 Others:
*******************************************************/
void StartKeyTask(void *argument)
{
    uint32_t keyEvent;

    osDelay(5000);    // 等待其他任务初始化完成

    while(1)
    {
        // 等待事件标志 事件由按键中断设置
        keyEvent = osEventFlagsWait(keyEventHandle,
            K1_EVENT | K2_EVENT | K3_EVENT | K4_EVENT | K5_EVENT | K6_EVENT | K7_EVENT | K8_EVENT,
            osFlagsWaitAny,
            osWaitForever);

        if(keyEvent & K1_EVENT){       // 按键1事件
            App_Printf("KeyTask: Key1 Pressed\r\n");
        }

        else if(keyEvent & K2_EVENT){  // 按键2事件
            App_Printf("KeyTask: Key2 Pressed\r\n");
        }

        else if(keyEvent & K3_EVENT){  // 按键3事件
            App_Printf("KeyTask: Key3 Pressed\r\n");
        }

        else if(keyEvent & K4_EVENT){  // 按键4事件
            App_Printf("KeyTask: Key4 Pressed\r\n");
        }

        else if(keyEvent & K5_EVENT){  // 按键5事件
            App_Printf("KeyTask: Key5 Pressed\r\n");
        }

        else if(keyEvent & K6_EVENT){  // 按键6事件
            App_Printf("KeyTask: Key6 Pressed\r\n");
        }

        else if(keyEvent & K7_EVENT){  // 按键7事件
            App_Printf("KeyTask: Key7 Pressed\r\n");
        }

        else if(keyEvent & K8_EVENT){  // 按键8事件
            App_Printf("KeyTask: Key8 Pressed\r\n");
        }

        else {
            // 无按键按下
        }

		osDelay(100);
	}
}
