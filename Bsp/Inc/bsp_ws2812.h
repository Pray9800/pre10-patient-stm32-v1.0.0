#ifndef __BSP_WS2812_H
#define __BSP_WS2812_H

#include "stdint.h"

// 最大允许的灯带数量
#define MAX_LED_NUM      100  

// 1个灯珠需要 24 个 bit（也就是 24 个 SPI 字节）
#define RGB_BIT          24   

// 供外部业务层调用的 API 接口
void ws2812_set_num(uint16_t led_num, uint8_t r, uint8_t g, uint8_t b);
void ws2812_turnoff(uint16_t led_num);

#endif /* __BSP_WS2812_H */