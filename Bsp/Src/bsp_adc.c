
/*******************************************************
 Copyright (C), HangZhou Jianjia Co.,Ltd.
 File name:         bsp_adc.c
 Author: PAN     Version: V1.0       Date:2026/04/23
 Description:       ADC采样文件

 Function List:     HAL_ADC_GetValue
                    HAL_ADC_Start
                    HAL_ADC_Stop
                    HAL_ADC_PollForConversion
                    HAL_ADC_GetError
 History:
*******************************************************/

#include "bsp_adc.h"
#include "bsp_usart.h"  //发送debug



void BSP_ADC_Init(void)
{
   //校准
    if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
    {
        App_Printf("ADC Calibration Error!\r\n");
    }
}


/*******************************************************
 Function:    BSP_ADC_GetArmHeight
 Description: 读取机械臂高度电位器的值
 Return:      0 ~ 65535 (因为你配置的是 16-bit 分辨率)
*******************************************************/
uint16_t BSP_ADC_GetArmHeight(void)
{
    uint16_t adc_value = 0; // 如果传感器没读到，默认返回0

    // 触发转换
    HAL_ADC_Start(&hadc1);

    // 2. 阻塞等待最多 1ms 
    if (HAL_ADC_PollForConversion(&hadc1, 1) == HAL_OK)
    {
        // 拿数据
        adc_value = HAL_ADC_GetValue(&hadc1);
    }
    else
    {
        App_Printf("ADC Arm Height Read Error: Timeout or Hardware Fault!\r\n");
    }
    
    return adc_value;
}