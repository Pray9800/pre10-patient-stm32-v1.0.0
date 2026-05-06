#ifndef __BSP_TORQUE_COM_H__
#define __BSP_TORQUE_COM_H__

#include "main.h"

// 定义读取模式枚举，让应用层调用时极其清晰
typedef enum {
    TORQUE_MODE_SINGLE = 1,  // 单AD读取
    TORQUE_MODE_DOUBLE = 2   // 双AD读取
} TorqueReadMode_e;

// 发送和读取力矩传感器数据的函数声明
void BSP_Torque_RequestData(TorqueReadMode_e mode);
uint32_t* Torque_Parse(uint8_t *rxbuf, uint16_t rxlen);//注意返回的是地址
uint8_t Torque_Parse_mode(uint8_t *rxbuf, uint16_t rxlen, int32_t *torquetmp);//赋值模式
#endif // __BSP_TORQUE_H__