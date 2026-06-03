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

#define RX_MAX_LEN 100
uint16_t rxlen = 0;            // 接收到的数据长度
uint8_t  dma_rxbuf[1024];      // DMA接收数据缓冲区


// 在 bsp_usart.c 中定义独立的缓冲区和长度
// static uint8_t rxbuf_u6[1024];
// static uint16_t rxlen_u6 = 0;

uint8_t rxbuf_u3[128];   // 接收到的力矩传感器数据暂定128
uint16_t rxlen_u3 = 0;

uint8_t rxbuf_u2[128];   // 接收到的台车轮子驱动1暂定128
uint16_t rxlen_u2 = 0;

uint8_t rxbuf_u4[128];    // 接收到的台车轮子驱动2暂定128
uint16_t rxlen_u4 = 0;




void BSP_UART_Start_Receive(void)
{
    // 1. 开启 USART6 的 DMA 空闲中断接收 (注意这里用 sizeof 更安全)
    HAL_UARTEx_ReceiveToIdle_IT(&huart6, dma_rxbuf, sizeof(dma_rxbuf)); 
    
    // 2. 开启 USART3 (力矩传感器) 的 IT 空闲中断接收
    HAL_UARTEx_ReceiveToIdle_IT(&huart3, rxbuf_u3, sizeof(rxbuf_u3)); 
    
    // 3. 开启 USART2 (驱动轮1) 的 IT 空闲中断接收
    HAL_UARTEx_ReceiveToIdle_IT(&huart2, rxbuf_u2, sizeof(rxbuf_u2)); 
    
    // 4. 开启 UART4 (驱动轮2) 的 IT 空闲中断接收
    HAL_UARTEx_ReceiveToIdle_IT(&huart4, rxbuf_u4, sizeof(rxbuf_u4)); 
}





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






static uint8_t u6_tx_buffer[64];
/*******************************************************
 Author: PAN        Version: V1.0       Date:2026/05/07
 Function:          Usart6_Send_Data
 Description:       串口6 中断非阻塞发送（考虑到U6包含功能比较多开启中断U234不属于高频暂未开启）
*******************************************************/
void Usart6_Send_Data(uint8_t *data, uint32_t size)
{
    // 长度保护防止溢出
    if (size > sizeof(u6_tx_buffer)) {
        size = sizeof(u6_tx_buffer); 
    }

    uint32_t timeout = 0;
    while (huart6.gState != HAL_UART_STATE_READY && timeout < 0xFFFFF) {
        timeout++; 
    }

    // 数据拷贝
    memcpy(u6_tx_buffer, data, size);

    HAL_UART_Transmit_IT(&huart6, u6_tx_buffer, size);
}



void Usart1_Send_Data(uint8_t *data, uint32_t size)
{
    HAL_UART_Transmit(&huart1, data, size, 0x200);
}


/*******************************************************
 Author: PAN        Version: V1.0       Date:2026/04/21
 Function:          Torque_RS232_Send 驱动力矩传感器通讯发送
 Description:       串口3发送
 Calls:             HAL_UART_Transmit
 Called By:         应用层
 Input:             data 待发送的数据帧/数组
                    size 长度
 Output:            无
 Return:            无
 Others:            无
*******************************************************/

void Torque_RS232_Send(uint8_t *data, uint32_t size)
{
   HAL_UART_Transmit(&huart3,data,size,100);
  // HAL_UART_Transmit_IT(&huart3,data,size);
}
//用不到
HAL_StatusTypeDef Usart3_Send_Data(uint8_t *data, uint32_t size)
{
   
    return HAL_UART_Transmit(&huart3, data, size, 0x200);
}


/*******************************************************
 Author: PAN        Version: V1.0       Date:2026/04/21
 Function:          Trolley_Drive_RS232_ComTX1 台车驱动轮RS232通讯发送口1 
 Description:       串口2发送
 Calls:             HAL_UART_Transmit
 Called By:         应用层
 Input:             data 待发送的数据帧/数组
                    size 长度
 Output:            无
 Return:            无
 Others:            无
*******************************************************/
    void Trolley_Drive_RS232_ComTX1(uint8_t *data, uint32_t size)
    {
       // HAL_UART_Transmit_IT(&huart2,data,size);
       HAL_UART_Transmit(&huart2,data,size,100);
    }


/*******************************************************
 Author: PAN        Version: V1.0       Date:2026/04/21
 Function:          Trolley_Drive_RS232_ComTX1 台车驱动轮RS232通讯发送口2
 Description:       串口4发送
 Calls:             HAL_UART_Transmit
 Called By:         应用层
 Input:             data 待发送的数据帧/数组
                    size 长度
 Output:            无
 Return:            无
 Others:            无
*******************************************************/
void Trolley_Drive_RS232_ComTX2(uint8_t *data, uint32_t size)
{
   // HAL_UART_Transmit_IT(&huart4,data,size);
   HAL_UART_Transmit(&huart4,data,size,100);
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
    
  //  if (comEventHandle == NULL) return; 
    if (huart->Instance == USART6) // 串口6接收完成
    {
        rxlen = Size;
        osEventFlagsSet(comEventHandle, EVENT_UART6_RX);
        //HAL_UARTEx_ReceiveToIdle_DMA(&huart6, dma_rxbuf, 1024); // 重新开启DMA接收
        HAL_UARTEx_ReceiveToIdle_IT(&huart6, dma_rxbuf, sizeof(dma_rxbuf));
    }
// ========  串口 3 接收完成 (力矩传感器) ========
    else if (huart->Instance == USART3) 
    {
        rxlen_u3 = Size;
        osEventFlagsSet(comEventHandle, EVENT_UART3_RX);                 
        // 最大允许装128
        HAL_UARTEx_ReceiveToIdle_IT(&huart3, rxbuf_u3, sizeof(rxbuf_u3)); 
    }
    // ======== 串口 2 接收完成 (台车驱动轮RS232通讯发送口1) ========
    else if (huart->Instance == USART2) 
    {
        rxlen_u2 = Size;
        osEventFlagsSet(comEventHandle, EVENT_UART2_RX);                 
        HAL_UARTEx_ReceiveToIdle_IT(&huart2, rxbuf_u2, sizeof(rxbuf_u2)); 
    }
    // ======== 4. 串口 4 接收完成 (台车驱动轮RS232通讯发送口2) ========
    else if (huart->Instance == UART4) 
    {
        rxlen_u4 = Size;
        osEventFlagsSet(comEventHandle, EVENT_UART4_RX);                 
        HAL_UARTEx_ReceiveToIdle_IT(&huart4, rxbuf_u4, sizeof(rxbuf_u4)); 
    }


}