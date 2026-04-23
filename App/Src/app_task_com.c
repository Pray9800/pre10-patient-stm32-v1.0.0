/*******************************************************
 Copyright (C), HangZou Jianjia Co.,Ltd.
 File name:         app_task_com.c
 Author: PENG       Version: V1.0       Date:2026/03/26
 Description:       串口处理任务
 Function List:
 History:
*******************************************************/
#include "app_task.h"
// #include "bsp_adc.h"
// #include "bsp_delay.h"
#include "bsp_gpio.h"
#include "bsp_usart.h"
#include "cmsis_os2.h"
#include "bsp_torque_com.h"
#include <string.h>
#include "bsp_crc.h"

// 解析后的数据结构体
typedef struct{
    uint8_t  cmd;    // 命令
    uint8_t  reg_addr;
    uint8_t   *data;  // 数据主体指针
}rxdata_t;

// 待发送的数据结构体
typedef struct{
    uint16_t  cmd;    // 命令(特别说明：读写+地址)
    uint8_t   *data;  // 数据主体指针
}txdata_t;

uint8_t g_brake_state_ctrl = 0;
uint8_t g_park_state_ctrl = 0;
SystemMsg_t SysMsg = {0};


/*******************************************************
 Author: PAN        Version: V1.0       Date:2026/04/23
 Function:          RxData_Parse
 Description:       解析接收到的数据包
 Calls:
 Called By:
 Input:
 Output:
 Return:
 Others:
*******************************************************/
uint8_t RxData_Parse(uint8_t *rxbuf, uint16_t rxlen, rxdata_t *rxdata)
{
    // 0. 基础长度保护 (最短指令 0A 03 10 01 3F FC = 6字节)
    if (rxlen < 6) 
    {
        return 3; 
    }

    // 1. 校验设备地址码 (0x0A)
    if (rxbuf[0] != 0x0A) 
    {
        return 1;   //帧头出错
    }

    // 2. CRC16 校验
    uint16_t calc_crc = CRC16_Modbus(rxbuf, rxlen - 2);
    uint16_t rx_crc = rxbuf[rxlen - 2] | (rxbuf[rxlen - 1] << 8);
    
    if (calc_crc != rx_crc) 
    {
        return 2;   // CRC校验失败
    }

    // uint8_t func_code = rxbuf[1]; 
    // uint8_t reg_addr  = rxbuf[2]; 
    // 高8位存功能码(03/06)，低8位存寄存器(10/11/12等)
    // rxdata->cmd = (uint16_t)((func_code << 8) | reg_addr);//取消原来16位cmd用法
    rxdata->cmd = rxbuf[1]; 
    rxdata->reg_addr = rxbuf[2]; 
    // 绑定数据指针 (不管是 03 还是 06，后续数据区都从索引 3 开始)
    if (rxdata->cmd == 0x03 || rxdata->cmd == 0x06) 
    {
        rxdata->data = &rxbuf[3];
    }
    else 
    {
        return 3;  // 不支持的功能码
    }

    return 0; // 解析成功
}



/*******************************************************
 Author: PENG       Version: V1.0       Date:2026/01/19
 Function:          TxData_Send
 Description:       打包并发送数据包
 Calls:
 Called By:
 Input:
 Output:
 Return:
 Others:
*******************************************************/
void TxData_Send(txdata_t *txdata)
{
    uint8_t txbuf[17] = {0}; // 发送缓冲区

    // 帧头部
    txbuf[0] =0x0A;
    // 状态反馈类型
    txbuf[1] =txdata->cmd;
    // 数据内容
    
    memcpy(&txbuf[5], txdata->data, 4);

    // 校验（当前版本暂无校验，预留接口）
    uint16_t tx_crc = CRC16_Modbus(txbuf, 9);
    txbuf[9] = tx_crc & 0xFF;
    txbuf[10] = (tx_crc >> 8) & 0xFF;

    // 发送数据包
    Usart6_Send_Data(txbuf, 11);
}

/*******************************************************
 Author: PENG       Version: V1.0       Date:2026/01/19
 Function:          Command_Process
 Description:       处理接收到的指令
 Calls:
 Called By:
 Input:
 Output:
 Return:
 Others:
*******************************************************/
void Command_Process(rxdata_t *rxdata, txdata_t *txdata)
{
    // 根据指令类型处理不同的指令
    switch(rxdata->cmd)
    {
        case CMD_TEST:    // A1 测试指令
            App_Printf("ComTask: Command Test received\r\n");
            // 回复测试结果
            txdata->cmd = 0; // 回复指令
            txdata->data[3] = 0x01; // 测试结果
            txdata->data[2] = 0;
            txdata->data[1] = 0;
            txdata->data[0] = 0;
            TxData_Send(txdata);
            break;

        // TODO: 其他指令处理
        
        default:
            App_Printf("ComTask: Unknown command received: 0x%02X\r\n", rxdata->cmd);
            break;
    }
}





/*******************************************************
 Author: PAN        Version: V1.0       Date:2026/04/23
 Function:          Protocol_Cmd_Dispatch
 Description:       协议指令业务分发中心
 Input:             p_rxdata: 已经解析好的数据结构体指针
*******************************************************/
void Protocol_Cmd_Dispatch(rxdata_t *p_rxdata)
{
    //  确保指针不为空
    if (p_rxdata == NULL || p_rxdata->data == NULL) return;
 
    // 第一层读写区分
    switch(p_rxdata->cmd)
    {
        // ==========================================
        // 写单寄存器 (主机下发命令)
        // ==========================================
        case 0x06: 
            // 第二层：判断具体写哪个寄存器
            switch(p_rxdata->reg_addr)
            {
               case 0x11: // UPS通断
                    // 处理灯带...
                    break;  
                case 0x14: // 抱闸
                    g_brake_state_ctrl = p_rxdata->data[0];
                    osEventFlagsSet(ArmBKEventHandle, EVENT_BRAKE_UPDATE);
                    break;
                
                case 0x13: // 驻车   原来是按钮控制驻车，现在改成串口控制了暂定
                    g_park_state_ctrl = p_rxdata->data[0]; 
                    break;
                    
                case 0x12: // 灯带
                    // 处理灯带...
                    break;

  
            }
            break; 

        // ==========================================
        // 读单寄存器 (主机来查岗)
        // ==========================================
        case 0x03:
            // 第二层：判断具体读哪个寄存器
            switch(p_rxdata->reg_addr)
            {
                case 0x14: // 查询抱闸

                    break;
                
                case 0x15: // 查询大臂高度
                    uint16_t current_height = 60000; 

                    break;
            }
            break; // 结束读指令大分支
            
        default:
            break;
    }
}






//测试git



/*******************************************************
 Author: PAN        Version: V1.0       Date:2026/04/22
 Function:          StartComTask
 Description:       通讯处理 包含一个力矩传感器的RS232接收u3和一个交换机的RS232接收u6
                    以及两个台车轮子驱动的RS232接收u2 u4
 Calls:             
 Called By:         应用层
 Input:
 Output:
 Return:
 Others:
*******************************************************/

void StartComTask(void *argument)
{
            uint8_t  rxbuf[256] = {0x0A,0X06,0X14,0X01,0XAC,0XBD};      // 待解析的数据包
            rxlen=6; // 模拟接收到的数据长度
            rxdata_t rxdata ;	    // 解析后的数据包 最多可以保存10个字节数据
            txdata_t txdata ;	    // 待发送的数据包
            
            // 解析接收到的数据包
            if (RxData_Parse(rxbuf, rxlen, &rxdata) == 0) { // 解析成功
                Protocol_Cmd_Dispatch(&rxdata);
                Command_Process(&rxdata, &txdata);   // 处理指令 发送回去指令
            } else {
                App_Printf("ComTask: RxData_Parse Error\r\n");
            }

            
}


// void StartComTask(void *argument)
// {
//     uint32_t flags;
//     uint8_t  rxbuf[256] = {0};      // 待解析的数据包
//     // uint8_t  rx[10] = {0};          // 接收数据包内容
//     // uint8_t  tx[4] = {0};           // 发送数据包内容
//     rxdata_t rxdata ;	    // 解析后的数据包 最多可以保存10个字节数据
//     txdata_t txdata ;	    // 待发送的数据包
//     //用于存放力矩解析结果的局部变量
//     uint32_t parsed_torque[2] = {0};
//     uint8_t  torque_count = 0;
//     // 1. 开启底层接收
//     // BSP_UART_Start_Receive();  暂时没写

//     for(;;)
//     {
//         // 阻塞等待 4 个串口的任意数据到来
//         flags = osEventFlagsWait(comEventHandle, EVENT_ALL_UART_RX, osFlagsWaitAny, osWaitForever);

//         //  开始分发数据
//         if (flags & EVENT_UART6_RX) {             
//             // 提取接收到的数据包
//             // 注意：原代码 memcpy(&rxbuf, &dma_rxbuf) 语法上略有瑕疵，
//             memcpy(rxbuf, dma_rxbuf, rxlen); 
//             App_Printf("ComTask: Received length: %d\r\n", rxlen);
//             for (int i = 0; i < rxlen; i++) {
//                 App_Printf("%02X ", rxbuf[i]);
//             }
//             App_Printf("\r\n");
//             osEventFlagsSet(ArmBKEventHandle, EVENT_BRAKE_UPDATE);//抱闸的唤醒

//             // 解析接收到的数据包
//             if (RxData_Parse(rxbuf, rxlen, &rxdata) == 0) { // 解析成功
//                 Protocol_Cmd_Dispatch(&rxdata);
//                 Command_Process(&rxdata, &txdata);   // 处理指令 发送回去指令
//             } else {
//                 App_Printf("ComTask: RxData_Parse Error\r\n");
//             }
//         }
        
//         if (flags & EVENT_UART3_RX) {
//             // 力矩传感器数据解析
//             memcpy(rxbuf, rxbuf_u3, rxlen_u3);
//             torque_count = Torque_Parse_mode(rxbuf, rxlen_u3, parsed_torque);

//             //打印读取到数据
//             App_Printf("ComTask: Received length: %d\r\n", rxlen);
//             for (int i = 0; i < rxlen; i++) {
//                 App_Printf("%02X ", rxbuf[i]);
//             }
//             App_Printf("\r\n");


//             // 4. 【核心触发逻辑】：只有成功解析出2个力矩值，才去唤醒大脑！
//             if (torque_count == 2) {
//                 // A. 将干净的数据存入全局情报站
//                 SysMsg.Torque[0] = parsed_torque[0];
//                 SysMsg.Torque[1] = parsed_torque[1];
                
//                 // B. 扣动扳机，唤醒 ctrlTask 去计算速度和驱动电机！
//                 osThreadFlagsSet(ctrlTaskHandle, 0x01); 
//             } else {
//                 // 如果是干扰数据或者单读取数据，仅仅打印警告，绝不触发电机运行
//                 App_Printf("ComTask(U3): Torque Parse Failed or Not Double Mode!\r\n");
//             }


            
//         }

//         if (flags & EVENT_UART2_RX) {
//             // 驱动轮1号反馈来了，扔给驱动解析器
//             memcpy(rxbuf, rxbuf_u2, rxlen_u2);
//             // DriveWheel_Parse_Process(1, rxbuf, rxlen_u2);
//                       //打印读取到数据
//             App_Printf("ComTask: Received length: %d\r\n", rxlen_u2);
//             for (int i = 0; i < rxlen_u2; i++) {
//                 App_Printf("%02X ", rxbuf[i]);
//             }
//             App_Printf("\r\n");
//         }
        
//         if (flags & EVENT_UART4_RX) {
//             // 驱动轮2号反馈来了
//             memcpy(rxbuf, rxbuf_u4, rxlen_u4);
//             // DriveWheel_Parse_Process(2, rxbuf, rxlen_u4);
//             App_Printf("ComTask: Received length: %d\r\n", rxlen_u4);
//             for (int i = 0; i < rxlen_u4; i++) {
//             App_Printf("%02X ", rxbuf[i]);
//             }
//             App_Printf("\r\n");
            
//         }
//     }
// }



inline unsigned int CRC16_MODBUS(unsigned char *data,unsigned int len)
{
  unsigned int i,j, tmp,CRC16;
  
  CRC16 = 0xffff;
  for(i = 0; i < len;i++)
  {
    CRC16 ^= data[i];
    for(j = 0; j < 8;j++)
    {
      tmp = (unsigned int)(CRC16 & 0x0001);
      CRC16 >>= 1;
      if (tmp == 1)
      {
          CRC16 ^= 0xA001;    //异或多项式
      }
    }
  }
  return CRC16;
}