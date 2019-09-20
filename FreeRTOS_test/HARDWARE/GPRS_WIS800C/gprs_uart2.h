#ifndef __gprs_uart2_H
#define __gprs_uart2_H

#include "stm32f10x.h"

extern char gprs_buf[1024];
extern uint8_t usart1_rev_flag,usart1_rev_cnt,usart1_rev_finish;


void usart2_init(uint32_t baud);
void usart1_init(u32 bound);

uint8_t gprs_send_cmd(char *cmd,char *res,uint32_t timeOut,uint8_t retime);
void usart1_send_string(unsigned char *str, unsigned short len); 
void gprs_send_string(char* str);
void gprs_clear(void);

u8 *gprs_send_cmd_cpy(char *cmd,char *res,uint32_t timeOut,uint8_t retime);


#endif   
