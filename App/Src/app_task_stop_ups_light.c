/*******************************************************
 Copyright (C), HangZhou Jianjia Co.,Ltd.
 File name:         app_task_ups_light.c
 Author: PAN      Version: V1.0       Date:2026/04/21
 Description: 控制UPS和指示灯的状态
 Function List:
 History:
*******************************************************/

#include "app_task.h"
#include "bsp_gpio.h"
#include "string.h"
#include "bsp_spi.h"
#include "spi.h"



/*******************************************************
 Author: PAN       Version: V1.0       Date:2026/04/20
 Function:          UPS_Ctl
 Description:       控制UPS状态
 Calls:             HAL_GPIO_WritePin
 Called By:         应用层
 Input:             state UPS状态
 Output:            无
 Return:            无
 Others:            无
*******************************************************/
void UPS_Ctl(uint8_t state)
{
    switch(state)
    {
        case UPS_ON:
            // 控制UPS开启
            UPS_ON_SET();
            UPS_OFF_RESET();
            break;
        case UPS_OFF:
            // 控制UPS关闭
            UPS_ON_RESET();
            UPS_OFF_SET();         
            break;
        default:
            // 默认状态
            break;
    }
}








/**
 * @brief: 原来的灯带控制代码
 */
//0x03  0码
//3f    1码
#define	WS2812_0	0x03
#define	WS2812_1	0x3f
#define LED_NUM          50   //yushe
#define Rgb_bit          24


uint8_t color_array[20][3] = {
    {255,255,255},
    {255,0,0},
    {0,255,0},
    {0,0,255},
    {0,255,255},
    {255,0,255},
    {255,255,0},
    {255,128,255},
    {128,0,255},
    {255,128,0},
    {255,255,255},
    {255,0,0},
    {0,255,0},
    {0,0,255},
    {0,255,255},
    {255,0,255},
    {255,255,0},
    {255,128,255},
    {128,0,255},
    {255,128,0},
};


uint8_t W2812Buffer[LED_NUM * Rgb_bit] = {0};
uint8_t *w2812_creatData(uint8_t R,uint8_t G,uint8_t B)
{
    static uint8_t temp[24] = {0};
    for (uint8_t i=0;i<8;i++){
        temp[7-i] =  (G & 0x01) ? WS2812_1 : WS2812_0; 
        G = G >> 1;
    }
    for (uint8_t i=0;i<8;i++){
        temp[15-i] =  (R & 0x01) ? WS2812_1 : WS2812_0; 
        R = R >> 1;
    }
    for (uint8_t i=0;i<8;i++){
        temp[23-i] =  (B & 0x01) ? WS2812_1 : WS2812_0; 
        B = B >> 1;
    }
    return temp;
}
void ws2812_set_num(uint16_t led_num ,uint8_t r,uint8_t g,uint8_t b)
{
      
      for(uint8_t i = 0; i < led_num; i++)
        memcpy(W2812Buffer + i*Rgb_bit,w2812_creatData(r,g,b),Rgb_bit);
     HAL_SPI_Transmit(&hspi2,W2812Buffer,led_num*Rgb_bit,1000); 
}
/*
* 
*/
void ws2812_ledturnoff(uint16_t led_num)
{
  memset(W2812Buffer,WS2812_0,led_num * Rgb_bit);
}
void ws2812_update(uint16_t led_num)
{
  HAL_SPI_Transmit(&hspi2,W2812Buffer,led_num*Rgb_bit,1000);
}
void ws2812_point(uint16_t pos,uint16_t led_num,uint8_t r,uint8_t g,uint8_t b)
{
//  if(led_num > LED_NUM) return 1;
  
    if((pos < led_num) &&(pos > 0))
        memcpy(W2812Buffer + (pos-1)*Rgb_bit,w2812_creatData(r,g,b),Rgb_bit);
    else
      ws2812_ledturnoff(led_num);
 
}
void ws2812_siglepos(uint16_t pos,uint16_t led_num,uint8_t r,uint8_t g,uint8_t b)
{
  ws2812_ledturnoff(led_num);
  ws2812_point(pos,led_num,r,g,b);
  ws2812_update(led_num);
}
void ws2812_whell(uint16_t led_num)
{
  static uint16_t posit = 0;
  uint8_t r, g, b;
  float angle = posit * 2 * 3.14159 /10;
  r = sin(angle) * 127 + 128;
  g = sin(angle + 2*3.14159/3) * 127 + 128;
  b = sin(angle + 4*3.14159/3) * 127 + 128;
  
  
  ws2812_ledturnoff(led_num);
  ws2812_point(posit,led_num,color_array[posit][0],color_array[posit][1],color_array[posit][2]);
  ws2812_update(led_num);
  posit ++;
  if(posit > led_num)
  {
    posit = 0;
  }
}
void ws2812_dual(uint16_t led_num)
{
   static uint16_t posit = 0;
   
   ws2812_ledturnoff(led_num);
   ws2812_point(posit,led_num,color_array[posit][0],color_array[posit][1],color_array[posit][2]);
   ws2812_point(led_num - posit,led_num,color_array[posit][0],color_array[posit][1],color_array[posit][2]);
   ws2812_update(led_num);
   
   posit ++;
  if(posit > led_num)
  {
    posit = 0;
  }
}
void ws2812_bright(uint16_t led_num)
{
   static uint16_t posit = 0;
   
   ws2812_ledturnoff(led_num);
   if(posit > led_num-1)
   {
     ws2812_point(posit,led_num,255-220,0,0);
   }
   else if(posit > led_num-2)
   {
     ws2812_point(posit+1,led_num,255-170,0,0);
   ws2812_point(posit,led_num,255-220,0,0);
   }
   else if(posit > led_num-3)
   {
   ws2812_point(posit+2,led_num,255-100,0,0);
   ws2812_point(posit+1,led_num,255-170,0,0);
   ws2812_point(posit,led_num,255-220,0,0);
   }
//   else if(posit > led_num-4)
//   {
//   ws2812_point(posit+2,led_num,255-100,0,0);
//   ws2812_point(posit+1,led_num,255-170,0,0);
//   ws2812_point(posit,led_num,255-220,0,0);
//   }
   else{
   ws2812_point(posit+3,led_num,255-50,0,0);
   ws2812_point(posit+2,led_num,255-100,0,0);
   ws2812_point(posit+1,led_num,255-170,0,0);
   ws2812_point(posit,led_num,255-220,0,0);
   }
   ws2812_update(led_num);
   
   posit ++;
  if(posit > led_num)
  {
    posit = 0;
  }
}


 