/*******************************************************
 Copyright (C), HangZou Jianjia Co.,Ltd.
 File name:         bsp_usart.c
 Author: PENG       Version: V1.0       Date:2026/03/26
 Description:       串口配置文件
                    对驱动层进行重新封装
 Function List:     __io_putchar
                    App_Printf
                    Usart1_Send_Data
 History:
*******************************************************/

#include "bsp_usart.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>


uint16_t rxlen = 0;            // 接收到的数据长度
uint8_t  dma_rxbuf[1024];      // DMA接收数据缓冲区


/*******************************************************
 Author: PENG       Version: V1.0       Date:2025/06/26
 Function:          __io_putchar
 Description:       重映射printf函数
 Calls:             HAL_UART_Transmit
 Called By:
 Input:             ch 字符
 Output:            无
 Return:            ch 字符
 Others:            无
*******************************************************/
#ifdef __GNUC__
int __io_putchar(int ch)
{
    HAL_UART_Transmit(&uart_p, (uint8_t *)&ch, 1, 0x200);
    return ch;
}
#else
int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(&uart_p, (uint8_t *)&ch, 1, 0x200);
    return ch;
}
#endif

/*******************************************************
 Author: PENG       Version: V1.0       Date:2025/06/26
 Function:          App_Printf
 Description:       线程安全的printf函数
                    当无法列出传递函数的所有实参的类型和数目时,可以用省略号指定参数表
 Calls:             vsnprintf
 Called By:         应用层
 Input:             fmt 格式字符串
 Output:            无
 Return:            无
 Others:            无
*******************************************************/
void App_Printf(const char *fmt, ...)
{
    static char buf_str[256];
    va_list v_args;
    int len;

    if (osKernelGetState() != osKernelRunning) {
        /* 调度器未启动，直接用低级输出（或忽略）*/
        return;
    }

    if (osMutexAcquire(printfMutexHandle, 50) != osOK) {
        return; /* 获取失败就放弃，避免阻塞 */
    }

    va_start(v_args, fmt);
    len = vsnprintf(buf_str, sizeof(buf_str), fmt, v_args);
    va_end(v_args);

    if (len <= 0) {
        osMutexRelease(printfMutexHandle);
        return;
    }
    if (len >= (int)sizeof(buf_str)) {
        len = sizeof(buf_str) - 1;
        buf_str[len] = '\0';
    }

    /* 直接调用 HAL 发送（阻塞），或将 buf 交给 DMA/队列以异步发送 */
    HAL_UART_Transmit(&uart_p, (uint8_t *)buf_str, (uint16_t)len, 200);

    osMutexRelease(printfMutexHandle);
}

/*******************************************************
 Author: PENG       Version: V1.0       Date:2026/03/26
 Function:          Usart6_Send_Data
 Description:       串口6发送
 Calls:             HAL_UART_Transmit_DMA
 Called By:         应用层
 Input:             data 待发送的数据帧/数组
                    size 长度
 Output:            无
 Return:            无
 Others:            无
*******************************************************/
void Usart6_Send_Data(uint8_t *data, uint32_t size)
{
    HAL_UART_Transmit_DMA(&huart6, data, size);
    // HAL_UART_Transmit(&huart6, data, size, 0x200);
}

/*******************************************************
 Author: PENG       Version: V1.0       Date:2026/03/26
 Function:          HAL_UARTEx_RxEventCallback
 Description:       串口接收事件回调函数
 Calls:
 Called By:         串口接受中断服务函数
 Input:             huart 串口句柄
                    Size 接收数据长度
 Output:            无
 Return:            无
 Others:            串口初始化时调用
                    HAL_UARTEx_ReceiveToIdle_DMA 
                    开启DMA空闲中断接收
*******************************************************/
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART6) // 串口6接收完成
    {
        rxlen = Size;

        if (comEventHandle != NULL) {
            osEventFlagsSet(comEventHandle, COMRX_EVENT);
        }

        HAL_UARTEx_ReceiveToIdle_DMA(&huart6, dma_rxbuf, 1024); // 重新开启DMA接收
    }
}
