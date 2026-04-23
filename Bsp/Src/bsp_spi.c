/*******************************************************
 Copyright (C), HangZou Jianjia Co.,Ltd.
 File name:         bsp_spi.c
 Author: PENG       Version: V1.0       Date:2026/04/21
 Description:       SPI配置文件
                    对驱动层进行重新封装
 Function List:     __io_putchar
                    App_Printf
                    Usart1_Send_Data
 History:
*******************************************************/

#include "bsp_spi.h"
#include "stm32h7xx_hal_spi.h"
#include "spi.h"
#include "core_cm7.h"

void SPI2_Send_Data(uint8_t *data, uint32_t size) 
{
    // 清理 D-Cache，强制把 CPU 算好的数据刷入物理内存，  DMA 能看到最新数据
    SCB_CleanDCache_by_Addr((uint32_t *)data, size);
    
    // 开启 DMA 非阻塞发送
    HAL_SPI_Transmit_DMA(&hspi2, data, size);
    
    // HAL_SPI_Transmit(&hspi2, data, size, HAL_MAX_DELAY);
}

