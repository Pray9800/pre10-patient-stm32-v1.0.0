#include "bsp_torque_com.h"
#include "bsp_usart.h" // 包含底层的串口发送函数


/*******************************************************
 Author: PAN       Version: V1.0       Date:2026/04/22
 Function:          BSP_Torque_RequestData
 Description:       通过uart3发送RS232指令请求力矩传感器数据
 Input:             mode 读取模式，单AD或双AD
*******************************************************/
void BSP_Torque_RequestData(TorqueReadMode_e mode)
{
    // 单通道读取指令
    uint8_t single_cmd[8] = {0x01, 0x03, 0x01, 0xF4, 0x00, 0x02, 0x84, 0x05};
    // 双通道读取指令
    uint8_t double_cmd[8] = {0x01, 0x03, 0x01, 0xf4, 0x00, 0x04, 0x04, 0x07};
    //  uint8_t double_cmd[8] = {0x40, 0x03, 0x01, 0xF4, 0x00, 0x04, 0x01, 0xFA};
   // uint8_t double_cmd[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x04, 0xFF, 0xFF};
    switch(mode)
    {
        case TORQUE_MODE_SINGLE:
             Torque_RS232_Send(single_cmd, 8);
            
        //   HAL_StatusTypeDef status = Usart3_Send_Data(single_cmd, 8);
            break;
            
        case TORQUE_MODE_DOUBLE:
           Torque_RS232_Send(double_cmd, 8);

            break;
            
        default:
            break;
    }
}





/*******************************************************
 Author: PAN        Version: V1.0       Date:2026/04/22
 Function:          Torque_Parse 解析力矩传感器
 Description:       通过uart3发送RS232指令请求力矩传感器数据直接赋值传递模式
 Input:             mode 读取模式，单AD或双AD
 Output:            数据包解析结果
*******************************************************/
uint8_t Torque_Parse_mode(uint8_t *rxbuf, uint16_t rxlen, int32_t *torquetmp)
{
  if (rxbuf[0] == 0x01 && rxbuf[1] == 0x03&& rxbuf[2] == 0x08) 
        {
    torquetmp[0] = (int32_t)((uint32_t)rxbuf[3] << 24 | 
                             (uint32_t)rxbuf[4] << 16 | 
                             (uint32_t)rxbuf[5] << 8  | 
                             (uint32_t)rxbuf[6]);

    torquetmp[1] = (int32_t)((uint32_t)rxbuf[7] << 24 | 
                             (uint32_t)rxbuf[8] << 16 | 
                             (uint32_t)rxbuf[9] << 8  | 
                             (uint32_t)rxbuf[10]);
              return 2; // 双力矩解析成功，返回2表示解析了两个值
        }

    else if (rxbuf[0] == 0x01 && rxbuf[1] == 0x03&& rxbuf[2] == 0x04) 
    {
        torquetmp[0] = (int32_t)(((int32_t)rxbuf[3] << 24) | 
                                 ((int32_t)rxbuf[4] << 16) | 
                                 ((int32_t)rxbuf[5] << 8)  | 
                                  (int32_t)rxbuf[6]);
              return 1; // 单力矩解析成功，返回2表示解析了两个值
    }
    else
    {
        return 0;
    }
    
    
}
