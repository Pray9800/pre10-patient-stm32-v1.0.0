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
#include "bsp_usart.h"

void App_UPS_Update(void);





void StartUpsTask(void *argument)
{
 
    App_UPS_Update();
    for(;;)
    {
        // 死等唤醒信号：获得物理按键动作，或收到串口指令时 去执行
        osThreadFlagsWait(FLAG_UPS_UPDATE, osFlagsWaitAny, osWaitForever);     
        // 醒来后，去执行状态机
        App_UPS_Update();
    }
      

}





/*******************************************************
 Author: PAN       Version: V1.0       Date:2026/04/20
 Function:          UPS_Ctl
 Description:       控制UPS状态最终执行函数
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




/*******************************************************
 Author: PAN       Version: V1.0       Date:2026/04/23
 Function:          App_UPS_Update
 Description:       控制UPS状态（包含按键和UART6控制）
 Calls:             HAL_GPIO_WritePin
 Called By:         应用层
 Input:             state UPS状态
 Output:            无
 Return:            无
 Others:            无
*******************************************************/
void App_UPS_Update(void)
{
    // 读取急停的实时电平
    // SET (高电平) = 按下(危险)； RESET (低电平) = 松开(安全)
    if (STOP_READ() == GPIO_PIN_SET) 
    {

        // 物理按键优先 物理急停拦截模式 如果检测到急停按钮被按下，忽略软件指令
        UPS_Ctl(UPS_OFF);         //  强制切断 UPS 
        g_ups_state_ctrl = 0x00;  // 清除标志位 用于U6的读取
        // // 或者将其替换为一个标志位在主循环打印，防止卡死中断。
        App_Printf(">>> ALARM: E-STOP ACTIVE! UPS Force OFF! <<<\r\n");
    }
    else 
    {

        // 软件指令放行模式
        // 只有在急停按钮“安全松开”的前提下 才去U6的指令
        if (g_ups_state_ctrl == 0xFF) 
        {
            UPS_Ctl(UPS_ON);
        }
        else if (g_ups_state_ctrl == 0x00) 
        {
            UPS_Ctl(UPS_OFF);
        }
        else 
        {
            // 其他值不变，保持当前状态  
        }
    }
}



/*******************************************************
 Author: PAN       Version: V1.0       Date:2026/04/23
 Function:          App_UPS_Request
 Description:       控制UPS串口通讯请求函数（在STOP的GPIO按键是0的情况下运行）
 Calls:             HAL_GPIO_WritePin
 Called By:         应用层
 Input:             state UPS状态
 Output:            无
 Return:            无
 Others:            无
*******************************************************/
void App_UPS_Request(uint8_t req_state)
{
    //赋值  来自U6的指令 
    g_ups_state_ctrl = req_state;
    //发送通知

    if (upsTaskHandle != NULL) {
            osThreadFlagsSet(upsTaskHandle, FLAG_UPS_UPDATE); 
        }
}








//中断回调函数
/*******************************************************
 Author: PENG       Version: V1.0       Date:2026/03/26
 Function:          HAL_GPIO_EXTI_Callback
 Description:       外部中断回调函数  按键中断处理
 Calls:             osEventFlagsSet
 Called By:         HAL库
 Input:             GPIO_Pin 中断引脚
 Output:            无
 Return:            无
 Others:            无
*******************************************************/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    static uint32_t LastTime[8] = {0};  // 记录每个按键上次触发时间
    uint32_t curTime = HAL_GetTick();
    uint32_t diffTime;

 
    //      if (GPIO_Pin == KEY3_Pin)
    // {
    //     diffTime = curTime - LastTime[2];
    //     if (diffTime >= KEY_TIME_MS) {
    //         LastTime[2] = curTime;
    //         if (keyEventHandle != NULL) {
    //             osEventFlagsSet(keyEventHandle, K3_EVENT);
    //         }
    //     }   // 设置按键3事件标志
    // }
    

      if (GPIO_Pin == STOP_Pin)
    {
        // 处理急停按钮STOP按键中断
        if (upsTaskHandle != NULL) {
            osThreadFlagsSet(upsTaskHandle, FLAG_UPS_UPDATE);
        }
    
    }
    else
    {
        // 其他中断引脚处理
    }
}












// /**
//  * @brief: 原来的灯带控制代码
//  */




// //0x03  0码
// //3f    1码
// #define	WS2812_0	0x03
// #define	WS2812_1	0x3f
// #define LED_NUM          50   //yushe
// #define Rgb_bit          24


// uint8_t color_array[20][3] = {
//     {255,255,255},
//     {255,0,0},
//     {0,255,0},
//     {0,0,255},
//     {0,255,255},
//     {255,0,255},
//     {255,255,0},
//     {255,128,255},
//     {128,0,255},
//     {255,128,0},
//     {255,255,255},
//     {255,0,0},
//     {0,255,0},
//     {0,0,255},
//     {0,255,255},
//     {255,0,255},
//     {255,255,0},
//     {255,128,255},
//     {128,0,255},
//     {255,128,0},
// };


// uint8_t W2812Buffer[LED_NUM * Rgb_bit] = {0};
// uint8_t *w2812_creatData(uint8_t R,uint8_t G,uint8_t B)
// {
//     static uint8_t temp[24] = {0};
//     for (uint8_t i=0;i<8;i++){
//         temp[7-i] =  (G & 0x01) ? WS2812_1 : WS2812_0; 
//         G = G >> 1;
//     }
//     for (uint8_t i=0;i<8;i++){
//         temp[15-i] =  (R & 0x01) ? WS2812_1 : WS2812_0; 
//         R = R >> 1;
//     }
//     for (uint8_t i=0;i<8;i++){
//         temp[23-i] =  (B & 0x01) ? WS2812_1 : WS2812_0; 
//         B = B >> 1;
//     }
//     return temp;
// }
// void ws2812_set_num(uint16_t led_num ,uint8_t r,uint8_t g,uint8_t b)
// {
      
//       for(uint8_t i = 0; i < led_num; i++)
//         memcpy(W2812Buffer + i*Rgb_bit,w2812_creatData(r,g,b),Rgb_bit);
//      HAL_SPI_Transmit(&hspi2,W2812Buffer,led_num*Rgb_bit,1000); 
// }
// /*
// * 
// */
// void ws2812_ledturnoff(uint16_t led_num)
// {
//   memset(W2812Buffer,WS2812_0,led_num * Rgb_bit);
// }
// void ws2812_update(uint16_t led_num)
// {
//   HAL_SPI_Transmit(&hspi2,W2812Buffer,led_num*Rgb_bit,1000);
// }
// void ws2812_point(uint16_t pos,uint16_t led_num,uint8_t r,uint8_t g,uint8_t b)
// {
// //  if(led_num > LED_NUM) return 1;
  
//     if((pos < led_num) &&(pos > 0))
//         memcpy(W2812Buffer + (pos-1)*Rgb_bit,w2812_creatData(r,g,b),Rgb_bit);
//     else
//       ws2812_ledturnoff(led_num);
 
// }
// void ws2812_siglepos(uint16_t pos,uint16_t led_num,uint8_t r,uint8_t g,uint8_t b)
// {
//   ws2812_ledturnoff(led_num);
//   ws2812_point(pos,led_num,r,g,b);
//   ws2812_update(led_num);
// }
// void ws2812_whell(uint16_t led_num)
// {
//   static uint16_t posit = 0;
//   uint8_t r, g, b;
//   float angle = posit * 2 * 3.14159 /10;
//   r = sin(angle) * 127 + 128;
//   g = sin(angle + 2*3.14159/3) * 127 + 128;
//   b = sin(angle + 4*3.14159/3) * 127 + 128;
  
  
//   ws2812_ledturnoff(led_num);
//   ws2812_point(posit,led_num,color_array[posit][0],color_array[posit][1],color_array[posit][2]);
//   ws2812_update(led_num);
//   posit ++;
//   if(posit > led_num)
//   {
//     posit = 0;
//   }
// }
// void ws2812_dual(uint16_t led_num)
// {
//    static uint16_t posit = 0;
   
//    ws2812_ledturnoff(led_num);
//    ws2812_point(posit,led_num,color_array[posit][0],color_array[posit][1],color_array[posit][2]);
//    ws2812_point(led_num - posit,led_num,color_array[posit][0],color_array[posit][1],color_array[posit][2]);
//    ws2812_update(led_num);
   
//    posit ++;
//   if(posit > led_num)
//   {
//     posit = 0;
//   }
// }
// void ws2812_bright(uint16_t led_num)
// {
//    static uint16_t posit = 0;
   
//    ws2812_ledturnoff(led_num);
//    if(posit > led_num-1)
//    {
//      ws2812_point(posit,led_num,255-220,0,0);
//    }
//    else if(posit > led_num-2)
//    {
//      ws2812_point(posit+1,led_num,255-170,0,0);
//    ws2812_point(posit,led_num,255-220,0,0);
//    }
//    else if(posit > led_num-3)
//    {
//    ws2812_point(posit+2,led_num,255-100,0,0);
//    ws2812_point(posit+1,led_num,255-170,0,0);
//    ws2812_point(posit,led_num,255-220,0,0);
//    }
// //   else if(posit > led_num-4)
// //   {
// //   ws2812_point(posit+2,led_num,255-100,0,0);
// //   ws2812_point(posit+1,led_num,255-170,0,0);
// //   ws2812_point(posit,led_num,255-220,0,0);
// //   }
//    else{
//    ws2812_point(posit+3,led_num,255-50,0,0);
//    ws2812_point(posit+2,led_num,255-100,0,0);
//    ws2812_point(posit+1,led_num,255-170,0,0);
//    ws2812_point(posit,led_num,255-220,0,0);
//    }
//    ws2812_update(led_num);
   
//    posit ++;
//   if(posit > led_num)
//   {
//     posit = 0;
//   }
// }


 