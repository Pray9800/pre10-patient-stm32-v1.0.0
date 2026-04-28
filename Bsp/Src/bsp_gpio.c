/*******************************************************
 Copyright (C), HangZhou Jianjia Co.,Ltd.
 File name:         bsp_gpio.c
 Author: PENG       Version: V1.0       Date:2026/03/26
 Description:       GPIO配置文件

 Function List:     HAL_GPIO_EXTI_Callback
                    Key_Scan_Direct
 History:
*******************************************************/

#include "bsp_gpio.h"







//封装M123的六个gpio

/*******************************************************
 * @brief  驻车腿顶起 (M1/M2/M3 同步伸出)
 * @note   逻辑：0相拉低，1相拉高
 *******************************************************/
void BSP_ParkMotors_Extend(void)
{
    // M1
    HAL_GPIO_WritePin(CTL_M1_0_GPIO_Port, CTL_M1_0_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(CTL_M1_1_GPIO_Port, CTL_M1_1_Pin, GPIO_PIN_SET);
    // M2
    HAL_GPIO_WritePin(CTL_M2_0_GPIO_Port, CTL_M2_0_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(CTL_M2_1_GPIO_Port, CTL_M2_1_Pin, GPIO_PIN_SET);
    // M3
    HAL_GPIO_WritePin(CTL_M3_0_GPIO_Port, CTL_M3_0_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(CTL_M3_1_GPIO_Port, CTL_M3_1_Pin, GPIO_PIN_SET);
}

/*******************************************************
 * @brief  驻车腿收回 (M1/M2/M3 同步收回)
 * @note   逻辑：0相拉高，1相拉低
 *******************************************************/
void BSP_ParkMotors_Retract(void)
{
    // M1
    HAL_GPIO_WritePin(CTL_M1_0_GPIO_Port, CTL_M1_0_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(CTL_M1_1_GPIO_Port, CTL_M1_1_Pin, GPIO_PIN_RESET);
    // M2
    HAL_GPIO_WritePin(CTL_M2_0_GPIO_Port, CTL_M2_0_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(CTL_M2_1_GPIO_Port, CTL_M2_1_Pin, GPIO_PIN_RESET);
    // M3
    HAL_GPIO_WritePin(CTL_M3_0_GPIO_Port, CTL_M3_0_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(CTL_M3_1_GPIO_Port, CTL_M3_1_Pin, GPIO_PIN_RESET);
}

/*******************************************************
 * @brief  驻车腿全部停止
 *******************************************************/
void BSP_ParkMotors_Stop(void)
{
    // M1
    HAL_GPIO_WritePin(CTL_M1_0_GPIO_Port, CTL_M1_0_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(CTL_M1_1_GPIO_Port, CTL_M1_1_Pin, GPIO_PIN_RESET);
    // M2
    HAL_GPIO_WritePin(CTL_M2_0_GPIO_Port, CTL_M2_0_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(CTL_M2_1_GPIO_Port, CTL_M2_1_Pin, GPIO_PIN_RESET);
    // M3
    HAL_GPIO_WritePin(CTL_M3_0_GPIO_Port, CTL_M3_0_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(CTL_M3_1_GPIO_Port, CTL_M3_1_Pin, GPIO_PIN_RESET);
}






/*******************************************************
 Author: PAN       Version: V1.0       Date:2026/4/23
 Function:          BSP_Brake_Control
 Description:       抱闸控制 00开启 FF关闭 其他值不变    
 Called By:         HAL库
 Input:             reg_val 0xFF开抱闸 0x00关抱闸 其他值不变
*******************************************************/
void BSP_Brake_Control(uint8_t reg_val)
{
     
    if (reg_val == 0xFF) 
    {
        // 开抱闸
        HAL_GPIO_WritePin(CTL_BK_0_GPIO_Port, CTL_BK_0_Pin, GPIO_PIN_SET);
    }
    else if (reg_val == 0x00) 
    {
        // 关抱闸
        HAL_GPIO_WritePin(CTL_BK_0_GPIO_Port, CTL_BK_0_Pin, GPIO_PIN_RESET);
    }
    else 
    {
        // 其他值不变，保持当前状态  
        //App_Printf("Warning: Invalid Brake Reg Value: 0x%02X\r\n", reg_val);
    }
}






