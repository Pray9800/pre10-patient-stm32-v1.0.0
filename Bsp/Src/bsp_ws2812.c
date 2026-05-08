#include "bsp_ws2812.h"
#include "string.h"
#include "bsp_spi.h"
extern void SPI2_Send_Data(uint8_t *data, uint32_t size); 

// SPI 模拟 WS2812 占空比的魔法数值
#define WS2812_0  0x03
#define WS2812_1  0x3F

// 物理内存分配：整个灯带的 DMA 发送缓冲区 (100 * 24 = 2400 字节)
uint8_t W2812Buffer[MAX_LED_NUM * RGB_BIT] = {0};

/**
 * @brief  [内部核心函数] 将 8位的 RGB 数据翻译成 24字节的 SPI 波形数据
 * @note   直接操作目标内存，避免了老代码的 static 数组，支持 RTOS 线程安全
 * @param  dest 目标内存指针
 * @param  r, g, b 颜色值
 */
static void w2812_fill_buffer(uint8_t *dest, uint8_t r, uint8_t g, uint8_t b)
{
    // WS2812 协议规定数据发送顺序为 G -> R -> B，并且是高位先发 (MSB First)
    for (int i = 0; i < 8; i++) 
    {
        dest[7 - i]  = (g & 0x01) ? WS2812_1 : WS2812_0; 
        g >>= 1;
        
        dest[15 - i] = (r & 0x01) ? WS2812_1 : WS2812_0; 
        r >>= 1;
        
        dest[23 - i] = (b & 0x01) ? WS2812_1 : WS2812_0; 
        b >>= 1;
    }
}

/**
 * @brief  点亮指定数量的灯带，并统一设置为相同颜色
 * @param  led_num 实际灯的数量 (来自上位机)
 * @param  r, g, b 红色，绿色，蓝色 (0-255)
 */
void ws2812_set_num(uint16_t led_num, uint8_t r, uint8_t g, uint8_t b)
{
    // 安全防线：防止上位机瞎发一个很大的数量，导致数组越界引发 HardFault
    if (led_num > MAX_LED_NUM) 
    {
        led_num = MAX_LED_NUM;  //只负责100个灯 
    }
      
    // 1. 将颜色数据装填进 DMA 缓冲区
    for (uint16_t i = 0; i < led_num; i++)
    {
        w2812_fill_buffer(&W2812Buffer[i * RGB_BIT], r, g, b);
    }
    
    // 2. 调用带有 D-Cache 清洗机制的高级 DMA 发送接口 
    SPI2_Send_Data(W2812Buffer, led_num * RGB_BIT); 
}

/**
 * @brief  关闭指定数量的灯带
 * @param  led_num 实际灯的数量
 */
void ws2812_turnoff(uint16_t led_num)
{
    if (led_num > MAX_LED_NUM) led_num = MAX_LED_NUM;
    
    // 0码就是熄灭
    memset(W2812Buffer, WS2812_0, led_num * RGB_BIT);
    
    SPI2_Send_Data(W2812Buffer, led_num * RGB_BIT);
}