#ifndef _rs485_H
#define _rs485_H

#include "system.h"

void rs485_init(int bound);

void UartRxMonitor(u8 ms); //串口接收监控
void UartDriver(void); //串口驱动函数void UartRead(u8 *buf, u8 len); //串口接收数据
u8 rs485_UartWrite(u8 *buf2 ,u8 len2);  //串口发送数据
u8 UartRead(u8 *buf, u8 len) ;


#endif

