/*******************************************************
 Copyright (C), HangZou Jianjia Co.,Ltd.
 File name:         app_task_ctrl.c
 Author: PAN       Version: V1.0       Date:2026/04/26
 Description:       控制任务台车的辅助推进
 Function List:
 * - KEY9按下时，GPIO信号拉高，进入控制逻辑
 * - 主动触发一次力矩读取
 * - 等待 comTask 解析完成的信号（线程标志位 0x01）
 * - 解析完成后计算速度并输出到电机
 History:
*******************************************************/

#include "app_task.h"
#include "bsp_gpio.h"
#include "bsp_usart.h"
#include "bsp_servomotor_com.h"
#include "bsp_torque_com.h"




// 比例系数 (数值越小，需要的推力越大；数值越大，越敏感)
#define KP_SPEED  5 
// 限速
#define MAX_RPM   3000

void StartTorqueMove(void *argument)
{
    // 开机初始化：给力矩模块发一次清零指令 来源：官方手册 6 7位是CRC校验
  uint8_t ch1_cmd[8] = {0x01, 0x05, 0x00, 0x64, 0xFF, 0X00,0xCD, 0xE5};
  uint8_t ch2_cmd[8] = {0x01, 0x05, 0x00, 0x65, 0xFF, 0X00,0x9C, 0x25};  

Torque_RS232_Send(ch1_cmd, 8);
    osDelay(10); // 留出发送时间
    Torque_RS232_Send(ch2_cmd, 8);
    osDelay(10); // 等待传感器清零生效

    // 初始化电机驱动器
    BSP_ServoMotor_Init();
    uint8_t servo_is_enabled = 0;
    int32_t speed_l, speed_r,speed_r_send; //右轮属于镜像安装需要取反
    uint32_t flags;

    for(;;)
    {
       if (Trolley_Move() == 1) // 台车使能按钮按下
        // if (1) // 台车使能按钮按下
        { 
           osThreadFlagsClear(FLAG_TORQUE_READY);
           BSP_Torque_RequestData(TORQUE_MODE_DOUBLE); // 请求双力矩AD数据
           if (!servo_is_enabled) {
                BSP_ServoMotor_Start();
                App_Printf("Move_Start \r\n");
                servo_is_enabled = 1;
            }
            
            // 有task_com的U3 任务发来通知
            flags = osThreadFlagsWait(FLAG_TORQUE_READY ,osFlagsWaitAny, 50);
            
            if ((flags & FLAG_TORQUE_READY) == FLAG_TORQUE_READY)
            {
                // 保存数据 并且将左轮
                speed_r = SysMsg.Torque[0]  *  KP_SPEED ;
                speed_l = SysMsg.Torque[1]  *  KP_SPEED ;

                App_Printf("speed_r is %ld \r\n" ,  speed_r);
                App_Printf("speed_l is %ld\r\n" ,   speed_l);         

               // 阈值3000转租
                // -- 左轮限幅 --
                if (speed_l > MAX_RPM)       speed_l = MAX_RPM;
                else if (speed_l < -MAX_RPM) speed_l = -MAX_RPM;
                
                // -- 右轮限幅 --
                if (speed_r > MAX_RPM)       speed_r = MAX_RPM;
                else if (speed_r < -MAX_RPM
                
                ) speed_r = -MAX_RPM;               
                // 调用底层发送指令 把右轮的倒转拨正
                speed_r_send=speed_r*-1; 
                BSP_ServoMotor_SetSpeed(speed_l, speed_r_send);

            }
            else
            {
 
                BSP_ServoMotor_SetSpeed(0, 0);
            }
            osDelay(50);
        }
        else // 使能按钮松开
        {
            if (servo_is_enabled) {
                BSP_ServoMotor_SetSpeed(0, 0);
                BSP_ServoMotor_Stop();
                servo_is_enabled = 0;
            }
            // 清除可能残留的标志位，防止下次按按钮时误触发
            osThreadFlagsClear(FLAG_TORQUE_READY ); 
           osDelay(50); 
        }
          
    }
  
}





