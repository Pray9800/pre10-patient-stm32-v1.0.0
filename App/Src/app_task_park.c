/*******************************************************
 Copyright (C), HangZhou Jianjia Co.,Ltd.
 File name:         app_task_park.c
 Author: PAN       Version: V1.0       Date:2026/04/23
 Description: 光电限位开关执行以及UART6控制FOOTP驻车
 Function List: 
 * - UART6接收指令控制FOOTP驻车状态（收回/顶起)
 * - 光电开关接收信号控制3个推杆电机的独立收回与限（但是源代码暂未用上）
 * - 按键控制三个电机的统一收回与顶起（对应老代码 case 4 和 case 2）
 History:
*******************************************************/





#include "app_task.h"
#include "bsp_gpio.h"



/*******************************************************
 Author: PAN       Version: V1.0       Date:2026/04/26
 Function:          FOOTP_Ctrl
 Description:       控制驻车状态（UART6控制）只有指令时候控制
 Calls:             HAL_GPIO_WritePin
 Called By:         应用层
 Input:             state FOOTP状态设定
 Output:            无
 Return:            无
 Others:            无
*******************************************************/
void FOOTP_Ctrl(uint8_t state)  
{



        // U6的指令0x13
        if (state == 0XFF) 
        {
            // // 可以移动
            // FOOTP_0_ON() ;
            // FOOTP_1_ON() ;
            // FOOTP_2_ON() ;
            // FOOTP_3_ON() ;
            //  BSP_ParkMotors_Extend();//顶起推杆 就是驻车
        }
        else if(state == 0x00) 
        {
            //停止移动
            // FOOTP_0_OFF() ;
            // FOOTP_1_OFF() ;
            // FOOTP_2_OFF() ;
            // FOOTP_3_OFF() ;
            // BSP_ParkMotors_Retract(); //回收推杆 就是不驻车 
        }

  
  
    
}




/*******************************************************
 Author: PAN        Version: V1.0       Date:2026/04/26
 Function:          FOOTP_Ctrl
 Description:       KEY按键采用PS4控制驻车状态
 Calls:             HAL_GPIO_WritePin
 Called By:         应用层
 Input:             
 Output:            无
 Return:            无
 Others:            无
*******************************************************/
 
void StartParkTask(void *argument)
{

 
    for(;;)
        {       
            // 判断推杆按键状态
            if (GET_KEY_PARK() == GPIO_PIN_SET) 
            {
                // 按下：收回推杆
                BSP_ParkMotors_Retract();
                g_park_state_ctrl=0x00;   //全局变量提供U6读取台车升起还是降落
            }
            else if(GET_KEY_PARK() == GPIO_PIN_RESET) 
            {           
                // 松开：顶起推杆
                BSP_ParkMotors_Extend();
                g_park_state_ctrl=0xFF; 
            }
            else
            {
                // BSP_ParkMotors_Stop();
            }

            osDelay(100); 
        }
        
}




#if 0 // 设为 1 即可开启编译
void PSWITCHDetect()
{
  if(PS1_READ()== GPIO_PIN_SET)//第1#
  {
     M1_0_OFF(); 
     M1_1_OFF() ;     // 停止 M1
    //  PEout(M1_A_Pin,0);
    //  PEout(M1_B_Pin,0);     // 停止 M1
  }
  else
  {
     M1_0_ON() ;
     M1_1_OFF();      // 启动 M1
    // PEout(M1_A_Pin,1);
    // PEout(M1_B_Pin,0);      // 启动 M1
  }
  if(PS2_READ()== GPIO_PIN_SET)
  {
     M2_0_OFF(); 
     M2_1_OFF();     // 停止 M2
    //  PEout(M2_A_Pin,0);
    //  PEout(M2_B_Pin,0);     // 停止 M2
  }
  else
  {
     M2_0_ON() ;
     M2_1_OFF();      // 启动 M2
    // PEout(M2_A_Pin,1);
    // PEout(M2_B_Pin,0);      // 启动 M2
  }
  if(PS3_READ()== GPIO_PIN_SET)
  {
     M3_0_OFF(); 
     M3_1_OFF() ;     // 停止 M3
    // PEout(M3_A_Pin,0);
    // PBout(M3_B_Pin,0);       // 停止 M3
  }
  else
  {
     M3_0_ON() ;
     M3_1_OFF();      // 启动 M3
    // PEout(M3_A_Pin,1);
    // PBout(M3_B_Pin,0);        // 启动 M3
  }
}

#endif






/* =========================================================================
 * 描      述: 配合光电开关，实现3个推杆电机的独立收回与限位。光电开关备用控制原来代码复现
 * 目前推杆电机自带物理限位，此函数暂不调用。留存以备后用。
 * ========================================================================= */
#if 0 // 设为 1 即可开启编译

void BSP_ParkMotors_PSWITCH_Detect(void)
{
    // ----------------------------------------------------
    // 1号推杆电机：独立限位
    // ----------------------------------------------------
    if(PS1_READ() == GPIO_PIN_SET) 
    {
        // 碰到光电开关(高电平)，1号电机单独刹车停转
        M1_0_OFF();
        M1_1_OFF();
    }
    else 
    {
        // 没碰到，1号电机继续收回 (0相拉高，1相拉低)
        M1_0_ON();
        M1_1_OFF();
    }

    // ----------------------------------------------------
    // 2号推杆电机：独立限位
    // ----------------------------------------------------
    if(PS2_READ() == GPIO_PIN_SET)
    {
        // 碰到光电开关，2号电机单独刹车停转
        M2_0_OFF();
        M2_1_OFF();
    }
    else
    {
        // 没碰到，2号电机继续收回
        M2_0_ON();
        M2_1_OFF();
    }

    // ----------------------------------------------------
    // 3号推杆电机：独立限位
    // ----------------------------------------------------
    if(PS3_READ() == GPIO_PIN_SET)
    {
        // 碰到光电开关，3号电机单独刹车停转
        M3_0_OFF();
        M3_1_OFF();
    }
    else
    {
        // 没碰到，3号电机继续收回
        M3_0_ON();
        M3_1_OFF();
    }
}

#endif