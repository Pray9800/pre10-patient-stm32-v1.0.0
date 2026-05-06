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

// 串口接收缓冲区 extern到外部提供task_com.c使用
extern  uint16_t rxlen;
extern  uint8_t  dma_rxbuf[1024];

// extern  uint16_t rxlen_u3;
// extern  uint8_t  rxbuf_u6[1024];


extern uint8_t rxbuf_u3[128];   // 接收到的力矩传感器数据暂定128
extern uint16_t rxlen_u3;

extern uint8_t rxbuf_u2[128];   // 接收到的台车轮子驱动1暂定128
extern uint16_t rxlen_u2;

extern uint8_t rxbuf_u4[128];    // 接收到的台车轮子驱动2暂定128
extern uint16_t rxlen_u4;






// 串口接收中断
// #define EVENT_UART6_RX       (1UL << 0)     // 串口收中断事件
#define EVENT_UART6_RX   (1 << 0)   // 0x01 (第 0 位)  主要控制
#define EVENT_UART3_RX   (1 << 1)   // 0x02 (第 1 位)  控制驱动轮
#define EVENT_UART2_RX   (1 << 2)   // 0x04 (第 2 位)  接收驱动轮指令
#define EVENT_UART4_RX   (1 << 3)   // 0x08 (第 3 位)  控制驱动轮
// 为了任务里等待方便，定义一个包含所有串口事件的掩码
#define EVENT_ALL_UART_RX (EVENT_UART6_RX | EVENT_UART3_RX | EVENT_UART2_RX | EVENT_UART4_RX)




extern 	osEventFlagsId_t comEventHandle;  // 串口事件标志句柄

extern  osMutexId_t printfMutexHandle;    // printf互斥信号量句柄


void App_Printf(const char *fmt, ...);
void Usart6_Send_Data(uint8_t *data, uint32_t size);
void Usart1_Send_Data(uint8_t *data, uint32_t size);
void Torque_RS232_Send(uint8_t *data, uint32_t size);
void Trolley_Drive_RS232_ComTX1(uint8_t *data, uint32_t size);
void Trolley_Drive_RS232_ComTX2(uint8_t *data, uint32_t size);
void BSP_UART_Start_Receive(void);
HAL_StatusTypeDef Usart3_Send_Data(uint8_t *data, uint32_t size);

#endif /* __BSP_USART_H_ */
