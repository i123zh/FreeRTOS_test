#ifndef _rs485_H
#define _rs485_H

#include "system.h"

void rs485_init(int bound);

void UartRxMonitor(u8 ms); //���ڽ��ռ��
void UartDriver(void); //������������void UartRead(u8 *buf, u8 len); //���ڽ�������
u8 rs485_UartWrite(u8 *buf2 ,u8 len2);  //���ڷ�������
u8 UartRead(u8 *buf, u8 len) ;


#endif

