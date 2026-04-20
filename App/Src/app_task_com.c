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

#include <string.h>


// 解析后的数据结构体
typedef struct{
    uint16_t  cmd;    // 命令
    uint8_t   *data;  // 数据主体指针
}rxdata_t;

// 待发送的数据结构体
typedef struct{
    uint16_t  cmd;    // 命令
    uint8_t   *data;  // 数据主体指针
}txdata_t;

/*******************************************************
 Author: PENG       Version: V1.0       Date:2026/01/19
 Function:          RxData_Parse
 Description:       解析接收到的数据包
 Calls:
 Called By:
 Input:
 Output:
 Return:
 Others:
*******************************************************/
uint8_t RxData_Parse(uint8_t *rxbuf, rxdata_t *rxdata)
{
    // 检查帧头部
    if (rxbuf[0] != '@' || rxbuf[1] != 'E' || rxbuf[2] != 'C'|| rxbuf[3] != 'U') 
    {
        return 1; // 无效帧头部
    }
    // 检查帧尾部
    if (rxbuf[15] != 0x0D || rxbuf[16] != 0x0A) 
    {
        return 2; // 无效帧尾部
    }

    // 提取指令类型和数据
    rxdata->cmd = rxbuf[4];
    memcpy(rxdata->data, &rxbuf[5], 10);

    // 数据校验 当前版本暂无校验，预留接口

    return 0; // 成功解析
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
    txbuf[0] = '@';
    txbuf[1] = 'A';
    txbuf[2] = 'P';
    txbuf[3] = 'P';

    // 状态反馈类型
    txbuf[4] = txdata->cmd;

    // 数据内容
    memcpy(&txbuf[5], txdata->data, 4);

    // 校验（当前版本暂无校验，预留接口）
    
    // 帧尾部
    txbuf[9] = 0x0D;
    txbuf[10] = 0x0A;

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
 Author: PENG       Version: V1.0       Date:2026/01/16
 Function:          StartComTask
 Description:
 Calls:
 Called By:
 Input:
 Output:
 Return:
 Others:
*******************************************************/
void StartComTask(void *argument)
{
    uint32_t getEevent;
    uint8_t  rxbuf[17];             // 待解析的数据包
    uint8_t  rx[10] = {0};          // 接收数据包内容
    uint8_t  tx[4] = {0};           // 发送数据包内容
    rxdata_t rxdata = {0, rx};	    // 解析后的数据包
    txdata_t txdata = {0, tx};	    // 待发送的数据包

    // osDelay(2000);

    while(1)
    {
        // 等待事件标志 由串口接收中断和 app_task_key.c 设置
        getEevent = osEventFlagsWait(
            comEventHandle, 
            COMRX_EVENT | K1_EVENT | K2_EVENT | K3_EVENT | K4_EVENT | K5_EVENT | K6_EVENT | K7_EVENT | K8_EVENT,
            osFlagsWaitAny, 
            osWaitForever);

        if (getEevent & COMRX_EVENT)         // 串口接收事件
        {
            // 提取接收到的数据包
            rxlen = 17;                      // 固定长度17字节
            memcpy(&rxbuf, &dma_rxbuf, rxlen);
            App_Printf("ComTask: Received length: %d\r\n", rxlen);
            for (int i = 0; i < rxlen; i++) {
                App_Printf("%02X ", rxbuf[i]);
            }
            App_Printf("\r\n");

            // 解析接收到的数据包
            if (RxData_Parse(rxbuf, &rxdata) == 0) { // 解析成功
                Command_Process(&rxdata, &txdata);   // 处理指令
            } else {
                App_Printf("ComTask: RxData_Parse Error\r\n");
            }
        }

        else if (getEevent & K1_EVENT)       // 高度控制开始时串口回复事件
        {
            txdata.cmd = 0;           // 回复指令
            txdata.data[3] = 0;        // 目标挡位
            txdata.data[2] = 0;        // 目标挡位
            txdata.data[1] = 0;
            txdata.data[0] = 0;
            TxData_Send(&txdata);
            // App_Printf("ComTask: HighCtl Rise Event: TarGear = %d\r\n", TarGear);
        }

        // TODO: 其他指令处理

        else
        {
            // 无效事件，忽略
        }

	    osDelay(100);

	  }
}git add App/Src/app_task_com.c
//测试git