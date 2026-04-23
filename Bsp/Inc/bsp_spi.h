/*******************************************************
 Copyright (C), HangZou Jianjia Co.,Ltd.
 File name:         bsp_spi.h
 Author: PENG       Version: V1.0        Date:2026/04/21
 Description:       SPI配置头文件
                    对驱动层进行重新封装
 Function List:
 History:
*******************************************************/


#ifndef __BSP_SPI_H_
#define __BSP_SPI_H_


#include "main.h"
#include "usart.h"
#include "cmsis_os2.h"

void SPI2_Send_Data(uint8_t *data, uint32_t size);

#endif