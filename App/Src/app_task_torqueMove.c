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




#define KP_FORWARD     8     // 直行敏感度（越大推得越轻松）
#define KP_TURN        4     // 转向敏感度（略小于直行，防止转弯太猛）
#define KP_SPEED       5     //力矩限速
#define DEADZONE_FWD   10    // 直行死区：双手平均推力超过这个值才走
#define DEADZONE_TURN  22    // 转向死区 左右手受力差值小于20时，直线行驶

#define DEADZONE       10     // 死区 
#define MAX_RPM        500   // 极限速度
#define MAX_STEP       15    // 斜坡加速度，决定起步是否丝滑
#define FILTER_NUM     5     // 滤波深度



#if 0
// =====================================
// 旧版本 无平滑滤波和步进 (独立驱动)
// =====================================

void StartTorqueMove(void *argument)
{
    // --- 传感器清零 ---
    uint8_t ch1_cmd[8] = {0x01, 0x05, 0x00, 0x64, 0xFF, 0x00, 0xCD, 0xE5};
    uint8_t ch2_cmd[8] = {0x01, 0x05, 0x00, 0x65, 0xFF, 0x00, 0x9C, 0x25};  
    Torque_RS232_Send(ch1_cmd, 8);
    osDelay(20); 
    Torque_RS232_Send(ch2_cmd, 8);
    osDelay(20); 

    BSP_ServoMotor_Init();
    uint8_t servo_is_enabled = 0; 

    int32_t current_l = 0, current_r = 0; 
    uint32_t tick_start; // 用于精准控制 20ms 循环
    
    for(;;)
    {
        tick_start = osKernelGetTickCount(); 

        if (Trolley_Move() == 1) // 台车使能按钮按下
        { 
            osThreadFlagsClear(FLAG_U2_ACK_READY | FLAG_U4_ACK_READY);
            osThreadFlagsClear(FLAG_TORQUE_READY);
            BSP_Torque_RequestData(TORQUE_MODE_DOUBLE); 

            if (!servo_is_enabled) {
                BSP_ServoMotor_Start();
                osDelay(10);
                current_l = 0; current_r = 0; 
                servo_is_enabled = 1;
                App_Printf("START \r\n");
            }
            
            uint32_t wait_flags = osThreadFlagsWait(FLAG_TORQUE_READY, osFlagsWaitAny, 20);
            if ((wait_flags & FLAG_TORQUE_READY) == FLAG_TORQUE_READY)
            {
                // ==========================================
                // 原始数据直接计算 (无滤波、无解耦)
                // ==========================================
                int32_t raw_r = SysMsg.Torque[0];
                int32_t raw_l = SysMsg.Torque[1];

                // 右轮处理
                if (raw_r > DEADZONE)        current_r = (raw_r - DEADZONE) * KP_SPEED;
                else if (raw_r < -DEADZONE)  current_r = (raw_r + DEADZONE) * KP_SPEED;
                else                         current_r = 0;

                // 左轮处理
                if (raw_l > DEADZONE)        current_l = (raw_l - DEADZONE) * KP_SPEED;
                else if (raw_l < -DEADZONE)  current_l = (raw_l + DEADZONE) * KP_SPEED;
                else                         current_l = 0;

                // 限幅处理
                if (current_l > MAX_RPM)       current_l = MAX_RPM;
                else if (current_l < -MAX_RPM) current_l = -MAX_RPM;
                
                if (current_r > MAX_RPM)       current_r = MAX_RPM;
                else if (current_r < -MAX_RPM) current_r = -MAX_RPM;               

                // 最终输出 (右轮镜像取反)
                BSP_ServoMotor_SetSpeed(current_l, current_r * -1);
            }
            else  // 假如力矩没有传输了 也先暂停
            {  
                current_l = 0; current_r = 0;
                BSP_ServoMotor_SetSpeed(0, 0);
            }
        }
        else // 使能按钮松开
        {
            if (servo_is_enabled) {
                // 刹车/清空状态
                current_l = 0; 
                current_r = 0;

                // 连续发 10 次 0 速指令 (共 200ms) 保证不丢包
                for(int i = 0; i < 10; i++) {
                    BSP_ServoMotor_SetSpeed(0, 0);
                    osDelay(20); 
                }

                osThreadFlagsClear(FLAG_U2_ACK_READY | FLAG_U4_ACK_READY);
                BSP_ServoMotor_Stop();
                
                uint32_t ack_flags = osThreadFlagsWait(FLAG_U2_ACK_READY | FLAG_U4_ACK_READY, osFlagsWaitAny, 30);
                
                if (ack_flags != osFlagsErrorTimeout) {
                    App_Printf("Motor Disabled! ACK OK!  servo_is_enabled is %d\r\n", servo_is_enabled );
                } else {
                    App_Printf("Motor Disabled! ACK Timeout!  servo_is_enabled is %d \r\n", servo_is_enabled);
                }
                servo_is_enabled = 0;
            }   
            osThreadFlagsClear(FLAG_TORQUE_READY); 
        }

        // 强制保证整个大循环严格按照 20ms (50Hz) 运行
        osDelayUntil(tick_start + 20); 
    }
}
#endif


#if 0
//  新版本V1.1 引入 步进和滤波平滑
//  比例系数 (数值越小，需要的推力越大；数值越大，越敏感)

void StartTorqueMove(void *argument)
{
    // --- 传感器清零 ---
    uint8_t ch1_cmd[8] = {0x01, 0x05, 0x00, 0x64, 0xFF, 0x00, 0xCD, 0xE5};
    uint8_t ch2_cmd[8] = {0x01, 0x05, 0x00, 0x65, 0xFF, 0x00, 0x9C, 0x25};  
    Torque_RS232_Send(ch1_cmd, 8);
    osDelay(20); 
    Torque_RS232_Send(ch2_cmd, 8);
    osDelay(20); 

    BSP_ServoMotor_Init();
    uint8_t servo_is_enabled = 0; 

    int32_t current_l = 0, current_r = 0; 
    int32_t filter_l_buf[FILTER_NUM] = {0}, filter_r_buf[FILTER_NUM] = {0};
    uint8_t filter_idx = 0;
    
    uint32_t tick_start; // 用于精准控制 20ms 循环
    
    for(;;)
    {
        // 记录循环开始的精确时间
        tick_start = osKernelGetTickCount(); 

        if (Trolley_Move() == 1) // 台车使能按钮按下
        { 
            osThreadFlagsClear(FLAG_U2_ACK_READY | FLAG_U4_ACK_READY);
            osThreadFlagsClear(FLAG_TORQUE_READY);
            BSP_Torque_RequestData(TORQUE_MODE_DOUBLE); 

            if (!servo_is_enabled) {
                BSP_ServoMotor_Start();
                osDelay(10);
                current_l = 0; current_r = 0; 
                servo_is_enabled = 1;
                App_Printf("START \r\n");
            }
            
            uint32_t wait_flags = osThreadFlagsWait(FLAG_TORQUE_READY, osFlagsWaitAny, 20);
            if ((wait_flags & FLAG_TORQUE_READY) == FLAG_TORQUE_READY)
            {
                // ==========================================
                // 1. 滑动平均滤波
                // ==========================================
                filter_r_buf[filter_idx] = SysMsg.Torque[0];
                filter_l_buf[filter_idx] = SysMsg.Torque[1];
                filter_idx = (filter_idx + 1) % FILTER_NUM;

                int32_t sum_r = 0, sum_l = 0;
                for(int i = 0; i < FILTER_NUM; i++) {
                    sum_r += filter_r_buf[i];  
                    sum_l += filter_l_buf[i];
                }
                int32_t raw_r = sum_r / FILTER_NUM;
                int32_t raw_l = sum_l / FILTER_NUM;

                // ==========================================
                // 2. 线性死区与增益计算
                // ==========================================
                int32_t target_l = 0, target_r = 0;
                
                if (raw_r > DEADZONE)       target_r = (raw_r - DEADZONE) * KP_SPEED;
                else if (raw_r < -DEADZONE) target_r = (raw_r + DEADZONE) * KP_SPEED;

                if (raw_l > DEADZONE)       target_l = (raw_l - DEADZONE) * KP_SPEED;
                else if (raw_l < -DEADZONE) target_l = (raw_l + DEADZONE) * KP_SPEED;

                // ==========================================
                // 3. 安全限幅
                // ==========================================
                if (target_l > MAX_RPM) target_l = MAX_RPM; else if (target_l < -MAX_RPM) target_l = -MAX_RPM;
                if (target_r > MAX_RPM) target_r = MAX_RPM; else if (target_r < -MAX_RPM) target_r = -MAX_RPM;               

                // ==========================================
                // 4. 软件斜坡步进
                // ==========================================
                if (target_l - current_l > MAX_STEP)       current_l += MAX_STEP;
                else if (target_l - current_l < -MAX_STEP) current_l -= MAX_STEP;
                else                                       current_l = target_l;
                
                if (target_r - current_r > MAX_STEP)       current_r += MAX_STEP;
                else if (target_r - current_r < -MAX_STEP) current_r -= MAX_STEP;
                else                                       current_r = target_r;

                // 最终输出 (右轮镜像取反)
                BSP_ServoMotor_SetSpeed(current_l, current_r * -1);
            }
            else  // 假如力矩没有传输了 也先暂停 
            {  
                current_l = 0; current_r = 0;
                BSP_ServoMotor_SetSpeed(0, 0);
            }
        }
        else // 使能按钮松开
        {
            if (servo_is_enabled) {
                
                // 刹车/清空状态
                current_l = 0; 
                current_r = 0;
                memset(filter_l_buf, 0, sizeof(filter_l_buf));
                memset(filter_r_buf, 0, sizeof(filter_r_buf));
                
                // 连续发 10 次 0 速指令 (共 200ms)保证不丢包
                for(int i = 0; i < 10; i++) {
                    BSP_ServoMotor_SetSpeed(0, 0);
                    osDelay(20); 
                }

                osThreadFlagsClear(FLAG_U2_ACK_READY | FLAG_U4_ACK_READY);
                BSP_ServoMotor_Stop();
                
                uint32_t ack_flags = osThreadFlagsWait(FLAG_U2_ACK_READY | FLAG_U4_ACK_READY, osFlagsWaitAny, 30);
                
                if (ack_flags != osFlagsErrorTimeout) {
                    App_Printf("Motor Disabled! ACK OK!  servo_is_enabled is %d\r\n", servo_is_enabled );
                } else {
                    App_Printf("Motor Disabled! ACK Timeout!  servo_is_enabled is %d \r\n", servo_is_enabled);
                }
                servo_is_enabled = 0;
            }   
            osThreadFlagsClear(FLAG_TORQUE_READY); 
        }

        // 强制保证整个大循环严格按照 20ms (50Hz) 运行
        osDelayUntil(tick_start + 20); 
    }
}
#endif




/*******************************************************
 Author: PAN       Version: V1.0       Date:2026/05/08
 Function:          StartTorqueMove
 Description:       新版本V1.1 加入解耦
 Called By:         应用层
*******************************************************/



void StartTorqueMove(void *argument)
{
    // --- 传感器清零 --- 双通道清零 发送两次 
    uint8_t ch1_cmd[8] = {0x01, 0x05, 0x00, 0x64, 0xFF, 0x00, 0xCD, 0xE5};
    uint8_t ch2_cmd[8] = {0x01, 0x05, 0x00, 0x65, 0xFF, 0x00, 0x9C, 0x25};  
    Torque_RS232_Send(ch1_cmd, 8);
    osDelay(20); 
    Torque_RS232_Send(ch2_cmd, 8);
    osDelay(20); 

    BSP_ServoMotor_Init();
    uint8_t servo_is_enabled = 0; 

    int32_t current_l = 0, current_r = 0; 
    int32_t filter_l_buf[FILTER_NUM] = {0}, filter_r_buf[FILTER_NUM] = {0};
    uint8_t filter_idx = 0;
    
    uint32_t tick_start; // 用于精准控制 20ms 循环
    
    
    for(;;)
    {
        // 记录循环开始的精确时间
        tick_start = osKernelGetTickCount(); 

        if (Trolley_Move() == 1) // 台车使能按钮按下 key3
        { 
            osThreadFlagsClear(FLAG_U2_ACK_READY | FLAG_U4_ACK_READY);
            osThreadFlagsClear(FLAG_TORQUE_READY);
            BSP_Torque_RequestData(TORQUE_MODE_DOUBLE); 

            if (!servo_is_enabled) {
                BSP_ServoMotor_Start();
                //BSP_ServoMotor_Init();
                osDelay(10);
                current_l = 0; current_r = 0; 
                servo_is_enabled = 1;
                App_Printf("START \r\n");
            }
            uint32_t wait_flags = osThreadFlagsWait(FLAG_TORQUE_READY, osFlagsWaitAny, 20);
            if ((wait_flags & FLAG_TORQUE_READY) == FLAG_TORQUE_READY)
            {
                // 1. 滑动平均滤波
                filter_r_buf[filter_idx] = SysMsg.Torque[0];
                filter_l_buf[filter_idx] = SysMsg.Torque[1];
                filter_idx = (filter_idx + 1) % FILTER_NUM;

                int32_t sum_r = 0, sum_l = 0;
                for(int i = 0; i < FILTER_NUM; i++) {
                    sum_r += filter_r_buf[i];  sum_l += filter_l_buf[i];
                }
                int32_t raw_r = sum_r / FILTER_NUM;
                int32_t raw_l = sum_l / FILTER_NUM;

                // ==========================================
                // 运动学解耦控制 (分离直行与转向)
                // ==========================================
                // 计算共模(直行)和差模(转向)力矩
                int32_t force_fwd  = (raw_l + raw_r) / 2; 
                int32_t force_turn = (raw_l - raw_r) / 2; 

                int32_t v_fwd = 0, v_turn = 0;

                // 处理直行死区和增益
                if (force_fwd > DEADZONE_FWD)       v_fwd = (force_fwd - DEADZONE_FWD) * KP_FORWARD;
                else if (force_fwd < -DEADZONE_FWD) v_fwd = (force_fwd + DEADZONE_FWD) * KP_FORWARD;

                // 处理转向死区和增益 (克服双手力量不均引起的蛇行)
                if (force_turn > DEADZONE_TURN)       v_turn = (force_turn - DEADZONE_TURN) * KP_TURN;
                else if (force_turn < -DEADZONE_TURN) v_turn = (force_turn + DEADZONE_TURN) * KP_TURN;

                // 重新合成左右轮目标速度 (左轮=直行+转向，右轮=直行-转向)
                int32_t target_l = v_fwd + v_turn;
                int32_t target_r = v_fwd - v_turn;

                // 速度限幅
                if (target_l > MAX_RPM) target_l = MAX_RPM; else if (target_l < -MAX_RPM) target_l = -MAX_RPM;
                if (target_r > MAX_RPM) target_r = MAX_RPM; else if (target_r < -MAX_RPM) target_r = -MAX_RPM;               

                // 软件斜坡限幅 (MAX_STEP 步进)
                if (target_l - current_l > MAX_STEP)       current_l += MAX_STEP;
                else if (target_l - current_l < -MAX_STEP) current_l -= MAX_STEP;
                else                                       current_l = target_l;
                
                if (target_r - current_r > MAX_STEP)       current_r += MAX_STEP;
                else if (target_r - current_r < -MAX_STEP) current_r -= MAX_STEP;
                else                                       current_r = target_r;
                //  App_Printf("current_l is %ld  ,current_r is :%ld \n\r ",current_l,current_r );
                // 最终输出 (右轮镜像取反)
                BSP_ServoMotor_SetSpeed(current_l, current_r * -1);
            }
            else  //假如力矩没有传输了 也先暂停  也可以改成原速度推进 
            {  
                current_l = 0; current_r = 0;
                BSP_ServoMotor_SetSpeed(0, 0);
             }
        }
        else // 使能按钮松开
        {
            if (servo_is_enabled) {
                 

                // 刹车/清空状态
                current_l = 0; 
                current_r = 0;
                memset(filter_l_buf, 0, sizeof(filter_l_buf));
                memset(filter_r_buf, 0, sizeof(filter_r_buf));
                // 连续发 10 次 0 速指令 (共 200ms)保证不丢包
                for(int i = 0; i < 10; i++) {
                    BSP_ServoMotor_SetSpeed(0, 0);
                    osDelay(20); 
                }

                osThreadFlagsClear(FLAG_U2_ACK_READY | FLAG_U4_ACK_READY);
                //  (底层会发出 00 00 00 00)
                BSP_ServoMotor_Stop();
                uint32_t ack_flags = osThreadFlagsWait(FLAG_U2_ACK_READY | FLAG_U4_ACK_READY, osFlagsWaitAny, 30);
                // 至少收到了一个轮子的ACK，或者全收到)
                if (ack_flags != osFlagsErrorTimeout) {
                    App_Printf("Motor Disabled! ACK OK!  servo_is_enabled is %d\r\n",servo_is_enabled );
                } else {
                    // 两个轮子都丢包了
                    App_Printf("Motor Disabled! ACK Timeout!  servo_is_enabled is %d \r\n",servo_is_enabled);
                }
                servo_is_enabled = 0;
                        
            }   
                osThreadFlagsClear(FLAG_TORQUE_READY); 
        }


        // 强制保证整个大循环严格按照 20ms (50Hz) 运行
        osDelayUntil(tick_start + 20); 
    }
}