#include "bsp_servomotor_com.h"
#include "bsp_usart.h"  // 引入底层的串口发送函数
#include "cmsis_os2.h"  // 引入 RTOS 的 osDelay




/* * 宏定义映射：根据硬件图纸与 bsp_usart.c 
 * CH2(huart2) 对应 台车驱动1(左轮)
 * CH4(huart4) 对应 台车驱动2(右轮)
 */







/*******************************************************
 Author: PAN        Version: V1.0       Date:2026/04/22
 Function:          BSP_ServoMotor_Init  
 Description:       台车驱动轮电机初始化(速度模式, 加减速时间)
*******************************************************/
void BSP_ServoMotor_Init(void)
{
    uint8_t speedmodebuf[4] = {0x02, 0x00, 0xc4, 0xc6}; // 选择速度模式
   // uint8_t speedtimebuf[4] = {0x0a, 0x1E, 0x28, 0x50}; // 电机加减速时间 0到3000用时3s 以及4S
    uint8_t speedtimebuf[4] = {0x0a, 0x03, 0x03, 0x10}; //300ms 实际由于step的情况发挥作用不大
    uint8_t motostart[4]    = {0x00, 0x00, 0x01, 0x01}; // 启动电机

    // 1. 设置速度模式
    osDelay(10);
    Servo_Left_Send(speedmodebuf, 4);
    Servo_Right_Send(speedmodebuf, 4);
   
    // 2. 加减速时间设置
    osDelay(10);
    Servo_Left_Send(speedtimebuf, 4);
    Servo_Right_Send(speedtimebuf, 4);

    // 3. 启动电机 (使能)
    osDelay(10);
    Servo_Left_Send(motostart, 4);
    Servo_Right_Send(motostart, 4);
}

/*******************************************************
 Author: PAN        Version: V1.0       Date:2026/04/22
 Function:          BSP_ServoMotor_SetSpeed
 Description:       设置台车左右轮的转速
 Input:             speed_l 左轮速度 (rpm)
                    speed_r 右轮速度 (rpm)
                    (由正负号决定正反转)
*******************************************************/
void BSP_ServoMotor_SetSpeed(int speed_l, int speed_r)
{
    uint8_t speedbuf[4] = {0};
    int speed_l_tmp, speed_r_tmp;

    // 协议算法: 设定值 = (目标限幅转速 * 8192) / 3000
    speed_l_tmp = (speed_l * 8192) / 3000;
    speed_r_tmp = (speed_r * 8192) / 3000;

    // 将有符号的 int 强转为无符号的 uint16_t，
    // 杜绝负数在进行右移(>>)操作时引发的符号位扩展灾难，保证提取到的 16 进制绝对纯净。
    uint16_t hex_speed_l = (uint16_t)speed_l_tmp;
    uint16_t hex_speed_r = (uint16_t)speed_r_tmp;


    // -------- 组装并发送左轮数据 --------
    speedbuf[0] = 0x06;
    speedbuf[1] = (hex_speed_l >> 8) & 0xFF;
    speedbuf[2] = hex_speed_l & 0xFF;
    speedbuf[3] = (speedbuf[0] + speedbuf[1] + speedbuf[2]) & 0xFF; // 校验和
    Servo_Left_Send(speedbuf, 4);

    // -------- 组装并发送右轮数据 --------
    speedbuf[0] = 0x06;
    speedbuf[1] = (hex_speed_r >> 8) & 0xFF;
    speedbuf[2] = hex_speed_r & 0xFF;
    speedbuf[3] = (speedbuf[0] + speedbuf[1] + speedbuf[2]) & 0xFF; // 校验和
    Servo_Right_Send(speedbuf, 4);
}

/*******************************************************
 Author: PAN        Version: V1.0       Date:2026/04/22
 Function:          BSP_ServoMotor_Start
 Description:       启动电机
*******************************************************/
void BSP_ServoMotor_Start(void)
{
    uint8_t buf[4] = {0x00, 0x00, 0x01, 0x01}; 
    
    osDelay(2);
    Servo_Left_Send(buf, 4);
    Servo_Right_Send(buf, 4);
}

/*******************************************************
 Author: PAN        Version: V1.0       Date:2026/04/22
 Function:          BSP_ServoMotor_Stop
 Description:       停止电机
*******************************************************/
void BSP_ServoMotor_Stop(void)
{
    uint8_t buf[4] = {0x00, 0x00, 0x00, 0x00};
    
    osDelay(2);
    Servo_Left_Send(buf, 4);
    Servo_Right_Send(buf, 4);
}