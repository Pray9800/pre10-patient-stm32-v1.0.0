#ifndef __BSP_SERVOMOTOR_COM_H__
#define __BSP_SERVOMOTOR_COM_H__

#include "main.h"
#include "bsp_usart.h"



#define Servo_Left_Send(data, len)  Trolley_Drive_RS232_ComTX1(data, len)
#define Servo_Right_Send(data, len) Trolley_Drive_RS232_ComTX2(data, len)


// 伺服电机初始化
void BSP_ServoMotor_Init(void);

// 伺服电机启动 (使能)
void BSP_ServoMotor_Start(void);

// 伺服电机停止 (失能/急停)
void BSP_ServoMotor_Stop(void);

// 设置伺服电机转速
// speed_l: 左轮转速 (rpm)，正负决定正反转
// speed_r: 右轮转速 (rpm)，正负决定正反转
void BSP_ServoMotor_SetSpeed(int speed_l, int speed_r);

#endif /* __BSP_SERVOMOTOR_COM_H__ */