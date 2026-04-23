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
    uint8_t single_cmd[8] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xc4, 0x0b};
    // 双通道读取指令
    uint8_t double_cmd[8] = {0x01, 0x03, 0x01, 0xf4, 0x00, 0x04, 0x04, 0x07};

    switch(mode)
    {
        case TORQUE_MODE_SINGLE:
            Torque_RS232_Send(single_cmd, 8);
            break;
            
        case TORQUE_MODE_DOUBLE:
            Torque_RS232_Send(double_cmd, 8);
            break;
            
        default:
            break;
    }
}


// /*******************************************************
//  Author: PAN        Version: V1.0       Date:2026/04/22
//  Function:          Torque_Parse 解析力矩传感器
//  Description:       通过uart3发送RS232指令请求力矩传感器数据传递指针办法
//  Input:             mode 读取模式，单AD或双AD
//  Output:            数据包解析结果
// *******************************************************/
// uint32_t* Torque_Parse(uint8_t *rxbuf, uint16_t rxlen)
// {
//     // Implementation for parsing torque data
//     static uint32_t torquetmp[2] = {0};
//     // 检查是否为双读取（13字节）
//   if(rxlen >= 13 && rxbuf[2] == 0x08) 
//   {  // rxbuf[2]是数据长度字段
//     // 双读取：两个32位值
//     torquetmp[0] = (uint32_t)rxbuf[3] << 24;
//     torquetmp[0] += (uint32_t)rxbuf[4] << 16;
//     torquetmp[0] += (uint32_t)rxbuf[5] << 8;
//     torquetmp[0] += (uint32_t)rxbuf[6];

//     torquetmp[1] = (uint32_t)rxbuf[7] << 24;
//     torquetmp[1] += (uint32_t)rxbuf[8] << 16;
//     torquetmp[1] += (uint32_t)rxbuf[9] << 8;
//     torquetmp[1] += (uint32_t)rxbuf[10];
//   }
//   else if(rxlen >= 9 && rxbuf[2] == 0x04) 
//   {  // 单读取：一个32位值
  
//     torquetmp[0]= (uint32_t)rxbuf[3]<<24;
//     torquetmp[0]+=(uint32_t)rxbuf[4]<<16;
//     torquetmp[0]+=(uint32_t)rxbuf[5]<<8;
//     torquetmp[0]+=(uint32_t)rxbuf[6];

//     }  
//     else
//     {
//         // 无效数据包，返回错误码或空指针
//         return NULL;
//     }
//     return torquetmp;
// }






/*******************************************************
 Author: PAN        Version: V1.0       Date:2026/04/22
 Function:          Torque_Parse 解析力矩传感器
 Description:       通过uart3发送RS232指令请求力矩传感器数据直接赋值传递模式
 Input:             mode 读取模式，单AD或双AD
 Output:            数据包解析结果
*******************************************************/
uint8_t Torque_Parse_mode(uint8_t *rxbuf, uint16_t rxlen, uint32_t *torquetmp)
{

    // 检查是否为双读取（13字节）
  if(rxlen >= 13 && rxbuf[2] == 0x08) 
  {  // rxbuf[2]是数据长度字段
    // 双读取：两个32位值
    torquetmp[0] = (uint32_t)rxbuf[3] << 24;
    torquetmp[0] += (uint32_t)rxbuf[4] << 16;
    torquetmp[0] += (uint32_t)rxbuf[5] << 8;
    torquetmp[0] += (uint32_t)rxbuf[6];

    torquetmp[1] = (uint32_t)rxbuf[7] << 24;
    torquetmp[1] += (uint32_t)rxbuf[8] << 16;
    torquetmp[1] += (uint32_t)rxbuf[9] << 8;
    torquetmp[1] += (uint32_t)rxbuf[10];
    return 2; // 双力矩解析成功，返回2表示解析了两个值
  }
  else if(rxlen >= 9 && rxbuf[2] == 0x04) 
  {  // 单读取：一个32位值
  
    torquetmp[0]= (uint32_t)rxbuf[3]<<24;
    torquetmp[0]+=(uint32_t)rxbuf[4]<<16;
    torquetmp[0]+=(uint32_t)rxbuf[5]<<8;
    torquetmp[0]+=(uint32_t)rxbuf[6];
    return 1; // 单力矩解析成功，返回1表示解析了1个值

    }  

        // 无效数据包
         return 0;
}





