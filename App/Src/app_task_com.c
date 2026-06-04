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
#include "bsp_spi.h"
#include "bsp_crc.h"
#include "bsp_ws2812.h"
#include "cmsis_os2.h"
#include "bsp_torque_com.h"
#include <string.h>
#include "bsp_crc.h"
#include "stdio.h"
#include "stm32h7xx_hal_uart.h"
#include "usart.h"
#include "bsp_servomotor_com.h"
// 解析后的数据结构体
typedef struct{
    uint8_t    cmd;    // 命令
    uint8_t    reg_addr;//寄存器地址
    uint8_t   *data;  // 数据主体指针
}rxdata_t;

// 待发送的数据结构体 由于写入和读取指令的应答格式不同，暂时不定义统一结构体了，直接在函数里处理了
//写的入指令校验通过就原来数据应答
typedef struct{
    uint8_t  cmd;    // 命令(特别说明：读写+地址)
    uint8_t  reg_addr; //寄存器地址
    uint8_t  *data;  // 数据主体指针
}txdata_w_t;  



// //灯带WS2812结构体 暂时用不上 
// typedef struct{
//     uint8_t  num;    // 灯带数量
//     uint32_t  color; //颜色寄存器

// }ws812_t;   


uint8_t volatile g_brake_state_ctrl = 0;
uint8_t volatile g_park_state_ctrl = 0;
uint8_t volatile g_ups_state_ctrl = 0xFF; //保证初始化的时候UPS有电
uint8_t volatile light_reg[4]={0x00,0x00,0x00,0x00}; //灯带状态寄存器数据区
uint16_t volatile adc_current_height = 0;  //机械臂高度
uint16_t volatile adc_targart_height = 0;  //机械臂高度设定高度U6传输
uint8_t  volatile  height_auto_mode=0  ;  //上位机控制高度标志位 0：本地按键控制 1：上位机控制
uint8_t  volatile  height_update_down=0Xff ;  //1上升 2 下降 0停止 初始化没有指令 依赖上位机发送
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
        
        App_Printf("calc_crc is 0x%04X\r\n", calc_crc);
        App_Printf("rx_crc is 0x%04X\r\n", rx_crc);
        App_Printf("ComTask: CRC Check ok and data definitely wrong\r\n");
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
          App_Printf("ComTask: CRC Check ok and data savesuccessfully: cmd=0x%02X, reg_addr=0x%02X\r\n", rxdata->cmd, rxdata->reg_addr);
   
    }
    else 
    {
        return 4;  // 不支持的功能码
        //0
    }

    return 0; // 解析成功
    App_Printf("ComTask: CRC Check ok and data savesuccessfully\r\n");
}



// ==========================================
// 应答函数和业务分发函数分开写，逻辑更清晰
// ==========================================
void Command_Process(uint8_t *rxbuf, uint16_t rxlen)
{
    if (rxbuf == NULL || rxlen < 6) return; // 安全校验

     uint8_t reg_len=0; // 数据长度（,每个case里面确定）
    // 直接从原始数组中提取关键信息
    uint8_t cmd      = rxbuf[1]; // 功能码
    uint8_t reg_addr = rxbuf[2]; // 寄存器地址

    // ---------------------------------------------------------
    // 0x06 写指令：马上返回原始数据
    // ---------------------------------------------------------
    if (cmd == 0x06)
    {
        Usart6_Send_Data(rxbuf, rxlen); //原路返回
        return; 
    }

    // ---------------------------------------------------------
    // 0x03 读指令：根据寄存器地址组装应答包
    // ---------------------------------------------------------
    if (cmd == 0x03)
    {
        uint8_t payload[16] = {0};   // 临时数据载体 读的数据

        // 根据数组里的寄存器地址
        switch (reg_addr)
        {
            case 0x11: payload[0] = g_ups_state_ctrl;  reg_len=1; break; // UPS

            case 0x12: 
                payload[0] = light_reg[0];
                payload[1] = light_reg[1];
                payload[2] = light_reg[2];
                payload[3] = light_reg[3];
                reg_len=4;
                break; // 灯带

            case 0x13: payload[0] = g_park_state_ctrl; reg_len=1;break; // 驻车

            case 0x14: payload[0] = g_brake_state_ctrl; reg_len=1;break; // 抱闸

            case 0x15: 
            {
                
                payload[0] = (adc_current_height >> 8) & 0xFF; 
                payload[1] = adc_current_height & 0xFF; 
                reg_len=2;       
                break; // 高度
            }


            case 0x16: payload[0] = height_update_down; reg_len=1;break; // 抱闸
            default: return; // 未知寄存器
        }

        // ---------------------------------------------------------
        // 协议打包层 
        // ---------------------------------------------------------
        uint8_t txbuf[32] = {0};
        uint16_t tx_len = 0;
        uint16_t crc = 0;
        
        if (reg_len == 0) return; 

        // 装填头部
        txbuf[0] = rxbuf[0];      // 从机地址
        txbuf[1] = cmd;           // 功能码 0x03
        txbuf[2] = reg_len;      // 数据长度

        // 装填数据
        for (int i = 0; i < reg_len; i++)
        {
            txbuf[3 + i] = payload[i];
        }

        tx_len = 3 + reg_len; 

        // 计算CRC并发送
        crc = CRC16_Modbus(txbuf, tx_len);
        txbuf[tx_len++] = crc & 0xFF;
        txbuf[tx_len++] = (crc >> 8) & 0xFF;
        Usart6_Send_Data(txbuf, tx_len);  

        App_Printf("ARM_Height : ");
        for (int i = 3; i < tx_len-2; i++) {
            App_Printf("%02X ", txbuf[i]);
        }
        App_Printf("\r\n");
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
                        uint8_t target_ups = p_rxdata->data[0];
                        App_UPS_Request(target_ups); //g_ups_state_ctrl在这个函数里面赋值
                        App_Printf("g_ups_state_ctrl update to %d\r\n", g_ups_state_ctrl);
                        break;  
                    case 0x14: // 抱闸
                        g_brake_state_ctrl = p_rxdata->data[0]; 
                        App_Printf("g_brake_state_ctrl update to %d\r\n", g_brake_state_ctrl);
                        osEventFlagsSet(ArmBKEventHandle, EVENT_BRAKE_UPDATE);
                        break;     
                                     
                    // case 0x13: // 驻车   不用写了只读取就好了
                    //     g_park_state_ctrl = p_rxdata->data[0]; 
                    //     FOOTP_Ctrl(g_park_state_ctrl);
                    //     App_Printf("g_park_state_ctrl update to %d\r\n", g_park_state_ctrl);
                    //     break;
                        
                    case 0x12: // 灯带
                    
                    light_reg[0] = p_rxdata->data[0]; // LED 灯数量
                    light_reg[1] = p_rxdata->data[1]; // 绿色 G
                    light_reg[2] = p_rxdata->data[2]; // 红色 R
                    light_reg[3] = p_rxdata->data[3]; // 蓝色 B

                    // 提取并核对参数 (增加代码可读性，防止传参错位)
                    uint8_t led_num = p_rxdata->data[0];
                    uint8_t led_g   = p_rxdata->data[1]; // 第2字节是绿
                    uint8_t led_r   = p_rxdata->data[2]; // 第3字节是红
                    uint8_t led_b   = p_rxdata->data[3]; // 第4字节是蓝

                   
                    // 自动处理 SPI 占空比转换、D-Cache 清洗和 DMA 发送
                    ws2812_set_num(led_num, led_r, led_g, led_b);

                    // 打印信息调试用，正式发布可以注释掉
                    App_Printf("WS2812 Update -> Num:%d, R:%02X, G:%02X, B:%02X\r\n", 
                               led_num, led_r, led_g, led_b);
                    
                        break;



                    case 0x15: //给高度数值 高度控制改变变量 后续由TASK自行解决 
                    adc_targart_height = (p_rxdata->data[0] << 8) | p_rxdata->data[1];  
                    height_update_down =0xFF; // 清零升降指令，暂停升降指令 使用高度控制
                    height_auto_mode=1; //标志位                 
                    App_Printf("adc_adc_targart_height update to %d\r\n", adc_targart_height);
                    break;   


                    case 0x16: //给升降信号按键控制升降
                    height_update_down=p_rxdata->data[0];//升降信号
                    height_auto_mode=1; //标志位 
                    App_Printf("height_update_down update to %d\r\n", height_update_down);
                    break;  

                }
                break; 

            // ==========================================
            // 读单寄存器 功能码 0x03 (主机查询状态)
            // ==========================================
           
                
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
#if 0
void StartComTask(void *argument)
{
             BSP_UART_Start_Receive();
    // 伺服电机初始化
            BSP_ServoMotor_Init();
// 伺服电机启动 (使能)
            BSP_ServoMotor_Start();
           uint8_t speed_cmd[] = {0x06, 0x01, 0x11, 0x18};  //100pms
           uint8_t speed_cmd_rev[] = {0x06, 0xFE, 0xEF, 0xF3}; 
            //{0x06, 0x00, 0x89, 0x8F} 
            Trolley_Drive_RS232_ComTX1(speed_cmd,4);
            Trolley_Drive_RS232_ComTX2(speed_cmd,4);

            // osDelay
            // speed_cmd[] = {0x06, 0x00, 0x89, 0x8F};
            // Trolley_Drive_RS232_ComTX1(speed_cmd,4);
            // Trolley_Drive_RS232_ComTX2(speed_cmd,4);

 
            // speed_cmd[] = {0x06, 0x00, 0x1B, 0x22};
            // Trolley_Drive_RS232_ComTX1(speed_cmd,4);
            // Trolley_Drive_RS232_ComTX2(speed_cmd,4);




    uint32_t flags;
         //用于存放力矩解析结果的局部变量
    int32_t parsed_torque[2] = {0};
    uint8_t  torque_count = 0;
             uint16_t current_len_u3 ;
  for(;;){

            // Trolley_Drive_RS232_ComTX1(speed_cmd,4);
            // Trolley_Drive_RS232_ComTX2(speed_cmd_rev,4);
            // osDelay(5000);
            // Trolley_Drive_RS232_ComTX1(speed_cmd_rev,4);
            // Trolley_Drive_RS232_ComTX2(speed_cmd_rev,4);
                 //  uint8_t  rxbuf[256] ;
                // osDelay(200);
           // //uint8_t  rxbuf[256] = {0x0A,0X03,0X11,0X01,0X6C,0X3E};      // 待解析的数据包
            uint8_t  rxbuf[256] = {0x0A,0X03,0X15,0X02,0X7C,0XAD};  //高度测试 
            // // uint8_t mode=0XFF;
            // // uint8_t  rxbuf[256] = {0x0A,0X03,0X15,0X02,0X7C,0XAD};  //M1-3测试 

             rxlen=6; // 模拟接收到的数据长度
             rxdata_t rxdata ;	    // 解析后的数据包 最多可以保存10个字节数据
            
            // 解析接收到的数据包
            if (RxData_Parse(rxbuf, rxlen, &rxdata) == 0) { // 解析成功
                Protocol_Cmd_Dispatch(&rxdata); //分发指令
                Command_Process(rxbuf, rxlen); // 处理完毕 发送回去指令
               
            } else {
                App_Printf("Return %d\r\n",RxData_Parse(rxbuf, rxlen, &rxdata));
                App_Printf("ComTask: RxData_Parse Error\r\n");
                App_Printf("ComTask: Received length: %d\r\n", rxlen);
                for (int i = 0; i < rxlen; i++) {
                    App_Printf("%02X ", rxbuf[i]);
                }

            } 
           #if 0

    
            BSP_Torque_RequestData(TORQUE_MODE_DOUBLE); // 请求双AD数据
            flags = osEventFlagsWait(comEventHandle, EVENT_ALL_UART_RX, osFlagsWaitAny, osWaitForever);
              if (flags & EVENT_UART3_RX) {            // 力矩传感器数据解析
                osEventFlagsClear(comEventHandle, EVENT_UART3_RX);//先清除
                if (rxlen_u3 > 0) {
                current_len_u3 =rxlen_u3;
                 rxlen_u3 = 0; 
                 memcpy(rxbuf, rxbuf_u3, current_len_u3);
                 torque_count = Torque_Parse_mode(rxbuf, current_len_u3, parsed_torque);
            //解析
                  if (torque_count == 2) {
                //将干净的数据存入全局情报站
                SysMsg.Torque[0] = parsed_torque[0];
                SysMsg.Torque[1] = parsed_torque[1];
                App_Printf("Torque Parsed: Right=%ld, Left=%ld\r\n", SysMsg.Torque[0], SysMsg.Torque[1]);    
             }
                }
        
            //打印读取到数据
            App_Printf("ComTask UART3.1: Received length: %d\r\n", current_len_u3);
            for (int i = 0; i < current_len_u3; i++) {
                App_Printf("%02X ", rxbuf[i]);
            }
            App_Printf("\r\n");
            memset(rxbuf, 0, sizeof(rxbuf)); // 清零接收缓冲区
        }
            #endif
     

            //  }    
            osDelay(100);  
            }

}

#endif





#if 1
    void StartComTask(void *argument)
    {
        uint32_t flags;
        uint8_t  rxbuf[128] = {0};      // 待解析的数据包
        rxdata_t rxdata ;	    // 解析后的数据包 最多可以保存10个字节数据
        // txdata_t txdata ;	    // 待发送的数据包
        //用于存放力矩解析结果的局部变量
        int32_t  parsed_torque[2] = {0};
        uint8_t  torque_count = 0;
        uint16_t current_len_u3 ;
        // 开启底层接收
       BSP_UART_Start_Receive();
        for(;;)
        {
            

            // 阻塞等待 4 个串口的任意数据到来
            flags = osEventFlagsWait(comEventHandle, EVENT_ALL_UART_RX, osFlagsWaitAny, osWaitForever);

           //  开始分发数据
            if (flags & EVENT_UART6_RX) {             
                // 提取接收到的数据包
                // 注意：原代码 memcpy(&rxbuf, &dma_rxbuf) 语法上略有瑕疵，
                HAL_NVIC_DisableIRQ(USART6_IRQn); //防止数据被覆盖
                memcpy(rxbuf, dma_rxbuf, rxlen); 
                HAL_NVIC_DisableIRQ(USART6_IRQn); 
                // 解析接收到的数据包
                if (RxData_Parse(rxbuf, rxlen, &rxdata) == 0) { // 解析成功
                    Protocol_Cmd_Dispatch(&rxdata); //分发指令
                    Command_Process(rxbuf, rxlen); // 处理完毕 发送回去指令
                } else {
                    App_Printf("ComTask: RxData_Parse Error\r\n");
                    //解析失败才去反馈收到了啥
                    App_Printf("ComTask: Received length: %d\r\n", rxlen);
                for (int i = 0; i < rxlen; i++) {
                    App_Printf("%02X ", rxbuf[i]);
                }
                App_Printf("\r\n");
                }
            }
            
            if (flags & EVENT_UART3_RX) {
                // 力矩传感器数据解析
               osEventFlagsClear(comEventHandle, EVENT_UART3_RX);//先清除
                if (rxlen_u3 > 0) {
                current_len_u3 =rxlen_u3;
                 rxlen_u3 = 0; 
                 memcpy(rxbuf, rxbuf_u3, current_len_u3);
                 torque_count = Torque_Parse_mode(rxbuf, current_len_u3, parsed_torque);
            //解析
                  if (torque_count == 2) {
                //将干净的数据存入全局情报站
                SysMsg.Torque[0] = parsed_torque[0];
                SysMsg.Torque[1] = parsed_torque[1];
                App_Printf("Torque Parsed: Right=%ld, Left=%ld\r\n", SysMsg.Torque[0], SysMsg.Torque[1]);    
                osThreadFlagsSet(torqueMoveTaskHandle, FLAG_TORQUE_READY);  //释放通知
            } else {
                    // 如果是干扰数据或者单读取数据，仅仅打印警告，绝不触发电机运行
                    App_Printf("ComTask(U3): Torque Parse Failed or Not Double Mode!\r\n");
                }              
                }
             memset(rxbuf, 0, sizeof(rxbuf)); // 清零接收缓冲区   
            }

            if (flags & EVENT_UART2_RX) {
                // 驱动轮1号反馈 
                memcpy(rxbuf, rxbuf_u2, rxlen_u2);
                // 校验：提取前两个字节，判断是否是速度应答或停机应答
            if (rxlen_u2 >= 2 && (rxbuf[0] == 0x00 && rxbuf[1] == 0x00)) {
            osThreadFlagsSet(torqueMoveTaskHandle, FLAG_U2_ACK_READY);
    }
                        //打印读取到数据
                // App_Printf("ComTaskU2: Received length: %d\r\n", rxlen_u2);
                // for (int i = 0; i < rxlen_u2; i++) {
                //     App_Printf("%02X ", rxbuf[i]);
                // }
                // App_Printf("\r\n");
            }
            
            if (flags & EVENT_UART4_RX) {
                // 驱动轮2号反馈 
                memcpy(rxbuf, rxbuf_u4, rxlen_u4);
            if (rxlen_u4 >= 2 && (rxbuf[0] == 0x00 && rxbuf[1] == 0x00)) {
                    osThreadFlagsSet(torqueMoveTaskHandle, FLAG_U4_ACK_READY);
                }
                // App_Printf("ComTaskU4: Received length: %d\r\n", rxlen_u4);
                // for (int i = 0; i < rxlen_u4; i++) {
                // App_Printf("%02X ", rxbuf[i]);
                // }
                // App_Printf("\r\n");
                
            }
            // osDelay(10); 
           
        }
    }

#endif


