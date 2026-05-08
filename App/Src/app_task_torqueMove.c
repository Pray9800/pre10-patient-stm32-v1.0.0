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
#include "usart.h"
#include "string.h"

// /**
//  * 旧版本 无平滑滤波和步进
//  */

// // 比例系数 (数值越小，需要的推力越大；数值越大，越敏感)
// #define KP_SPEED  5 
// // 限速
// #define MAX_RPM   500
// //死区 
// #define DEADZONE 10
// // 每次调节步进用于平滑移动数值越大 台车移动越激进
// #define MAX_STEP    10 
// // 滤波深度
// #define FILTER_NUM  5     

// void StartTorqueMove(void *argument)
// {
//     // 开机初始化：给力矩模块发一次清零指令 来源：官方手册 6 7位是CRC校验
//   uint8_t ch1_cmd[8] = {0x01, 0x05, 0x00, 0x64, 0xFF, 0X00,0xCD, 0xE5};
//   uint8_t ch2_cmd[8] = {0x01, 0x05, 0x00, 0x65, 0xFF, 0X00,0x9C, 0x25};  

//     Torque_RS232_Send(ch1_cmd, 8);
//     osDelay(20); // 留出发送时间
//     Torque_RS232_Send(ch2_cmd, 8);
//     osDelay(20); // 等待传感器清零生效

//     // 初始化电机驱动器
//     BSP_ServoMotor_Init();
//     uint8_t servo_is_enabled = 0;
//     int32_t raw_l, raw_r;           // 传感器原始值
//     int32_t speed_l, speed_r,speed_r_send; //右轮属于镜像安装需要取反

//     uint32_t flags;

//     for(;;)
//     {

//          // if (Trolley_Move() == 1) // 台车使能按钮按下
//       if (1) // 台车使能按钮按下
//         { 
           
//            osThreadFlagsClear(FLAG_TORQUE_READY);
//            BSP_Torque_RequestData(TORQUE_MODE_DOUBLE); // 请求双力矩AD数据
//            if (!servo_is_enabled) {
//                 BSP_ServoMotor_Start();
//                 App_Printf("Move_Start \r\n");
//                 servo_is_enabled = 1;
//             }
            
//             // 有task_com的U3 任务发来通知
//             flags = osThreadFlagsWait(FLAG_TORQUE_READY ,osFlagsWaitAny, 50);
            
//             if ((flags & FLAG_TORQUE_READY) == FLAG_TORQUE_READY)
//             {
//                 // 保存数据 并且将左轮
//                 raw_r = SysMsg.Torque[0];
//                 raw_l = SysMsg.Torque[1];

//                 App_Printf("speed_r is %ld \r\n" ,  raw_r );
//                 App_Printf("speed_l is %ld\r\n" ,   raw_l );         

//             //线性死区算法：(原始值 - 死区门槛) * 增益
//             // 右轮处理
//             if (raw_r > DEADZONE) {
//                 speed_r = (raw_r - DEADZONE) * KP_SPEED;
//             } else if (raw_r < -DEADZONE) {
//                 speed_r = (raw_r + DEADZONE) * KP_SPEED;
//             } else {
//                 speed_r = 0;
//             }

//             // 左轮处理
//             if (raw_l > DEADZONE) {
//                 speed_l = (raw_l - DEADZONE) * KP_SPEED;
//             } else if (raw_l < -DEADZONE) {
//                 speed_l = (raw_l + DEADZONE) * KP_SPEED;
//             } else {
//                 speed_l = 0;
//             }



//                // 阈值3000转租
//                 // -- 左轮限幅 --
//                 if (speed_l > MAX_RPM)       speed_l = MAX_RPM;
//                 else if (speed_l < -MAX_RPM) speed_l = -MAX_RPM;
                
//                 // -- 右轮限幅 --
//                 if (speed_r > MAX_RPM)       speed_r = MAX_RPM;
//                 else if (speed_r < -MAX_RPM) speed_r = -MAX_RPM;               
//                 // 调用底层发送指令 把右轮的倒转拨正
//                 speed_r_send=speed_r*-1; 
//                 BSP_ServoMotor_SetSpeed(speed_l, speed_r_send);

//             }
//             else
//             {
 
//                 BSP_ServoMotor_SetSpeed(0, 0);
//             }
           
//         }
//         else // 使能按钮松开
//         {
//             if (servo_is_enabled) {
//                 BSP_ServoMotor_SetSpeed(0, 0);
//                 BSP_ServoMotor_Stop();
//                 servo_is_enabled = 0;
//             }
//             // 清除可能残留的标志位，防止下次按按钮时误触发
//             osThreadFlagsClear(FLAG_TORQUE_READY );          
//         }
//          osDelay(50);        
//     } 
// }





//新版本 引入 步进和滤波平滑


// 比例系数 (数值越小，需要的推力越大；数值越大，越敏感)
#define KP_SPEED  5 
// 限速
#define MAX_RPM   500
//死区 
#define DEADZONE 10
// 每次调节步进用于平滑移动数值越大 台车移动越激进
#define MAX_STEP    10 
// 滤波深度深度越深越平滑
#define FILTER_NUM  5     
void StartTorqueMove(void *argument)

{

    // --- 传感器清零 ---

    uint8_t ch1_cmd[8] = {0x01, 0x05, 0x00, 0x64, 0xFF, 0x00, 0xCD, 0xE5};
    uint8_t ch2_cmd[8] = {0x01, 0x05, 0x00, 0x65, 0xFF, 0x00, 0x9C, 0x25};  
    Torque_RS232_Send(ch1_cmd, 8);
    osDelay(20); 
    Torque_RS232_Send(ch2_cmd, 8);
    osDelay(20); 

    // --- 电机初始化 ---
    BSP_ServoMotor_Init(); //开启使能 设定加减速度
    uint8_t servo_is_enabled = 0; 

    // 滤波与控制变量
    int32_t raw_l, raw_r;  //记录力矩传来数值用于处理死区
    int32_t target_l = 0, target_r = 0; 
    int32_t current_l = 0, current_r = 0; // 当前正在执行的速度

    // 滤波器数组
    int32_t filter_l_buf[FILTER_NUM] = {0};
    int32_t filter_r_buf[FILTER_NUM] = {0};
    uint8_t filter_idx = 0;
    uint32_t flags;

    for(;;)

    {

        if (Trolley_Move() == 1) // 台车使能按钮按下

        { 

            osThreadFlagsClear(FLAG_TORQUE_READY);
            BSP_Torque_RequestData(TORQUE_MODE_DOUBLE); //请求双力矩数据
            if (!servo_is_enabled) {
                BSP_ServoMotor_Start();
                // 启动时清空历史速度
                current_l = 0; 
                current_r = 0; 
                servo_is_enabled = 1;
                App_Printf("Move_Start \r\n");
            }
       
            flags = osThreadFlagsWait(FLAG_TORQUE_READY, osFlagsWaitAny, 50);//等待传来的力矩         
            if ((flags & FLAG_TORQUE_READY) == FLAG_TORQUE_READY)

            {
                // ==========================================
                // 滑动平均滤波 (消除传感器突变噪点)取五次平均值
                // ==========================================
                filter_r_buf[filter_idx] = SysMsg.Torque[0];
                filter_l_buf[filter_idx] = SysMsg.Torque[1];
                filter_idx = (filter_idx + 1) % FILTER_NUM;

                int32_t sum_r = 0, sum_l = 0;
                for(int i = 0; i < FILTER_NUM; i++) {
                    sum_r += filter_r_buf[i];
                    sum_l += filter_l_buf[i];
                }
                raw_r = sum_r / FILTER_NUM;
                raw_l = sum_l / FILTER_NUM;

                App_Printf("speed_r is %ld \r\n" ,  raw_r );
                App_Printf("speed_l is %ld\r\n" ,   raw_l );    
                // ==========================================
                // 线性死区与增益计算 (计算目标速度)
                // ==========================================

                // 右轮目标速度
                if (raw_r > DEADZONE)       target_r = (raw_r - DEADZONE) * KP_SPEED;
                else if (raw_r < -DEADZONE) target_r = (raw_r + DEADZONE) * KP_SPEED;
                else                        target_r = 0;

                // 左轮目标速度
                if (raw_l > DEADZONE)       target_l = (raw_l - DEADZONE) * KP_SPEED;
                else if (raw_l < -DEADZONE) target_l = (raw_l + DEADZONE) * KP_SPEED;
                else                        target_l = 0;

                // 安全限幅
                if (target_l > MAX_RPM) target_l = MAX_RPM; else if (target_l < -MAX_RPM) target_l = -MAX_RPM;
                if (target_r > MAX_RPM) target_r = MAX_RPM; else if (target_r < -MAX_RPM) target_r = -MAX_RPM;               

                // ==========================================
                // 软件斜坡限幅 
                // ==========================================
                // 限制左轮每次速度的变化量不超过 MAX_STEP
                if (target_l - current_l > MAX_STEP)       current_l += MAX_STEP;
                else if (target_l - current_l < -MAX_STEP) current_l -= MAX_STEP;
                else                                       current_l = target_l;
                // 限制右轮每次速度的变化量不超过 MAX_STEP
                if (target_r - current_r > MAX_STEP)       current_r += MAX_STEP;
                else if (target_r - current_r < -MAX_STEP) current_r -= MAX_STEP;
                else                                       current_r = target_r;
                // ==========================================
                // 最终输出给底层 (右轮镜像取反)
                // ==========================================
                int32_t speed_r_send = current_r * -1; 
                BSP_ServoMotor_SetSpeed(current_l, speed_r_send);
                // 去掉频繁打印，保持 RTOS 调度流畅
            }
        }
        else // 使能按钮松开
        {
            if (servo_is_enabled) {
                // 刹车时也清空状态
                current_l = 0; 
                current_r = 0;
                memset(filter_l_buf, 0, sizeof(filter_l_buf));
                memset(filter_r_buf, 0, sizeof(filter_r_buf));
                BSP_ServoMotor_SetSpeed(0, 0);
                BSP_ServoMotor_Stop();
                servo_is_enabled = 0;
            }
            osThreadFlagsClear(FLAG_TORQUE_READY); 
        }
        osDelay(50); 
    }
}