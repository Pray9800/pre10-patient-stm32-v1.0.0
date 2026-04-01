/*******************************************************
 Copyright (C), HangZou Jianjia Co.,Ltd.
 File name:         bsp_usart.h
 Author: PENG       Version: V1.0        Date:2026/03/26
 Description:       串口配置头文件
                    对驱动层进行重新封装
 Function List:
 History:
*******************************************************/

#ifndef __BSP_USART_H_
#define __BSP_USART_H_

#include "main.h"
#include "usart.h"
#include "cmsis_os2.h"


#define uart_p huart1    // printf对应串口

// 串口接收缓冲区
extern  uint16_t rxlen;
extern  uint8_t  dma_rxbuf[1024];

// 串口接收中断
#define COMRX_EVENT        (1UL << 0)     // 串口收中断事件
extern 	osEventFlagsId_t comEventHandle;  // 串口事件标志句柄

extern  osMutexId_t printfMutexHandle;    // printf互斥信号量句柄


void App_Printf(const char *fmt, ...);
void Usart6_Send_Data(uint8_t *data, uint32_t size);


#endif /* __BSP_USART_H_ */
