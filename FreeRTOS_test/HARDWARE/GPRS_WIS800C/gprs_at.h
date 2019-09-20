#ifndef _GPRS_AT_H_
#define _GPRS_AT_H_

#include "system.h"

#define Gprs_Init_Flag      (1<<0)      //GPRS初始化完成标志，
#define Gprs_Net_Flag       (1<<1)      //网络配置完成标志，
#define Gprs_Connect_Flag   (1<<2)      //连接成功标志，
#define Gprs_Heart_Flag     (1<<3)      //心跳标志，
#define Gprs_Server_Flag    (1<<4)      //服务器异常标志，
#define Gprs_Rev_Flag       (1<<5)      //数据接收标志，
#define Gprs_RevFinish_Flag (1<<6)      //数据接收完成标志，


#define PEN_ON     GPIO_SetBits(GPIOC,GPIO_Pin_13)
#define PEN_OFF   GPIO_ResetBits(GPIOC,GPIO_Pin_13)

extern char gprs_buf[256];
extern uint8_t usart1_rev_cnt;

void pen_gpio_config(void);
void Gprs_Init(void);
uint8_t gprs_init(void);
uint8_t gprs_connect(void);
uint8_t gprs_net_config(void);

uint8_t gprs_send_cmd(char *cmd,char *res,uint32_t timeOut,uint8_t retime);
void usart1_send_string(unsigned char *str, unsigned short len); 
void gprs_send_string(char* str);
void gprs_clear(void);
void uarst1_recv_irq(void);

#endif
