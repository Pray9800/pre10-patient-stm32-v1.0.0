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
#include "bsp_servomotor_com.h"
#include "bsp_torque_com.h"



void StartTorqueMove(void *argument)
{
  static uint8_t servo_is_enabled = 0;
  uint32_t thread_flags;
  int32_t speed_l = 0, speed_r = 0;

  // 初始化电机硬件配置
  BSP_ServoMotor_Init();

  for(;;)
  {
    // 
    //  台车驱动使能KEY9按下时，GPIO信号拉高，进入控制逻辑
    if (Trolley_Move()==1) 
    {
      if (!servo_is_enabled) {
        BSP_ServoMotor_Start(); // 电机使能
        servo_is_enabled = 1;
      }
        osThreadFlagsSet(torqueMoveTaskHandle, FLAG_TORQUE_READY); // 先设置标志位，清除原先的状态， from:gemini
      // 主动触发一次力矩读取
      BSP_Torque_RequestData(TORQUE_MODE_DOUBLE); // 请求双AD数据

      // --- 等待 comTask 解析完成的信号 ---
      // 等待 20ms，如果在周期内有新的力矩数据解析完成 通过uart3传入，然后在uart2和uart4发送给台车，解析完成后会设置线程标志位 0x01
      thread_flags = osThreadFlagsWait(FLAG_TORQUE_READY, osFlagsWaitAny, 20); 
      
      if (thread_flags == FLAG_TORQUE_READY) {
         speed_l = (int32_t)SysMsg.Torque[0] * 5;
         speed_r = (int32_t)SysMsg.Torque[1] * 5;        
        // 执行输出
        BSP_ServoMotor_SetSpeed(speed_l, speed_r);
      }
      else
      {
        // 异常：按着按钮却没等到数据，假设传感器坏了 之前运行太快
        // 安全起见，下发 0 速度，防止台车按最后一次速度“飞车”
        BSP_ServoMotor_SetSpeed(0, 0);
      }
    }
    else 
    {
      // GPIO 信号消失，立即关停
      if (servo_is_enabled) {
        BSP_ServoMotor_Stop();
        servo_is_enabled = 0;
      }
      speed_l = 0;
      speed_r = 0;
      BSP_ServoMotor_SetSpeed(0, 0);
      osThreadFlagsWait(FLAG_TORQUE_READY, osFlagsWaitAny, 0); // 清除之前的状态，防止下次误触发 from:gemini
    }

    // 维持控制频率100ms
    osDelay(2); 
  }
}




