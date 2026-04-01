/*******************************************************
 Copyright (C), HangZhou Jianjia Co.,Ltd.
 File name:         app_task_default.c
 Author: PENG       Version: V1.0       Date:2026/03/26
 Description:
 Function List:
 History:
*******************************************************/
#include "app_task.h"
#include "bsp_gpio.h"
#include "bsp_usart.h"

// #include "FreeRTOS.h"
// #include "task.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>


#ifdef DEBUG_STACK
void TaskStackInfo(void)
{
    TaskHandle_t tasks[] = {
        defaultTaskHandle,
        NULL  // 结束标记
    };

    App_Printf("Task Name\tStack Remain(Words)\r\n");
    for(int i = 0; tasks[i] != NULL; i++)
    {
        // 获取任务名称
        const char *taskName = pcTaskGetName(tasks[i]);
        // 获取堆栈高水位线（返回从创建以来最小的剩余堆栈空间）
        UBaseType_t highWaterMark = uxTaskGetStackHighWaterMark(tasks[i]);
        App_Printf("%-15s\t%lu\r\n", taskName, highWaterMark);
    }
    App_Printf("-----------------------------------------\r\n");
}

void TaskStateInfo(void)
{
    TaskStatus_t *pxTaskStatusArray;
    volatile UBaseType_t uxArraySize, x;
    uint32_t ulTotalRunTime;
    
    // 获取任务数量
    uxArraySize = uxTaskGetNumberOfTasks();
    
    // 分配内存
    pxTaskStatusArray = pvPortMalloc(uxArraySize * sizeof(TaskStatus_t));
    
    if(pxTaskStatusArray != NULL)
    {
        // 获取任务状态
        uxArraySize = uxTaskGetSystemState(pxTaskStatusArray, uxArraySize, &ulTotalRunTime);
        
        // 打印每个任务的信息
        App_Printf("----------------Task State----------------\r\n");
        App_Printf("Task\t\tState\tPrio\tReStack\tRuntime\r\n");
        for(x = 0; x < uxArraySize; x++)
        {
            App_Printf("%-15s\t", pxTaskStatusArray[x].pcTaskName);
            App_Printf("%d\t", pxTaskStatusArray[x].eCurrentState);
            App_Printf("%lu\t", pxTaskStatusArray[x].uxCurrentPriority);
            App_Printf("%d\t", pxTaskStatusArray[x].usStackHighWaterMark);
            App_Printf("%lu\r\n", pxTaskStatusArray[x].ulRunTimeCounter);
        }
        App_Printf("\r\nState 0:Running 1:Ready 2:Blocked 3:Suspended 4:Deleted\r\n");

        vPortFree(pxTaskStatusArray);
    }
}
#endif

/*******************************************************
 Author: PENG       Version: V1.0       Date:2026/01/21
 Function:          Led_Ctl
 Description:
 Calls:
 Called By:
 Input:
 Output:
 Return:
 Others:
*******************************************************/
void Led_Ctl(uint8_t state)
{
    switch(state)
    {
        case LED_ON:
            LED0_ON();
            break;
        case LED_OFF:
            LED0_OFF();
            break;
        case LED_BLINK:
            LED0_TOGGLE();
            break;
        default:
            LED0_OFF();
            break;
    }
}

/*******************************************************
 Author: PENG       Version: V1.0       Date:2026/01/21
 Function:          StartDefaultTask
 Description:       默认任务
 Calls:
 Called By:
 Input:
 Output:
 Return:
 Others:
*******************************************************/
void StartDefaultTask(void *argument)
{
    App_Printf("PRE10-DOCTOR-STM32 V%d.%d\r\n", VER_X, VER_Y);

	while(1)
	{
        #ifdef DEBUG_TASK
        // 获取任务状态统计信息
        char TaskInfo[512]={0};
        memset(TaskInfo, 0, sizeof(TaskInfo));
        vTaskList(TaskInfo);
        App_Printf("----------------Task List----------------\r\n");
        App_Printf("Task\t\tState\tPrio\tReStack\tNum\r\n");
        App_Printf("%s\r\n", TaskInfo);
        App_Printf("B:Blocked R:Ready X:Running S:Suspended D:Deleted\r\n");

        // 获取任务运行时间统计信息
        memset(TaskInfo, 0, sizeof(TaskInfo));
        vTaskGetRunTimeStats(TaskInfo);
        App_Printf("--------------CPU Percentage-------------\r\n");
        App_Printf("Task\t\tRunTime\tPercent\r\n");
        App_Printf("%s\r\n", TaskInfo);
        App_Printf("-----------------------------------------\r\n");

		// 芯片内部温度
        short temp = Get_TempInter_Data();
		App_Printf("CPU Temp: %02d.%2d\r\n", temp/100, temp%100);
        App_Printf("-----------------------------------------\r\n");
        osDelay(500);
        #endif

        #ifdef DEBUG_STACK
        TaskStackInfo();            // 打印任务堆栈信息
        // TaskStateInfo();            // 打印任务状态信息
        #endif

        Led_Ctl(LED_BLINK);

        // 独立看门狗复位时间 256*1000/40000 = 6.4s
        HAL_IWDG_Refresh(&hiwdg1);   // 喂狗

		osDelay(500);
	}
}
