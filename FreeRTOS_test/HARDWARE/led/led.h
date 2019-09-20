#ifndef _led_H
#define _led_H

#include "system.h"

/*  LEDʱ�Ӷ˿ڡ����Ŷ��� */
#define LED_PORTa        GPIOA
#define LED_PINa         GPIO_Pin_15 | GPIO_Pin_4 | GPIO_Pin_11
#define LED_PORTb        GPIOB
#define LED_PINb         GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2
#define LED_PORT_RCC    RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB

#define led0 PAout(15)  //���е�
#define led1 PAout(4)   //��ѹ
#define led2 PBout(0)   //����
#define led3 PBout(1)   //��ĭ
#define led4 PBout(2)   //��ˮ
#define led5 PAout(11)  //����

#define LED0 (1 << 0)
#define LED1 (1 << 1)

void LED_Init(void);
void LED_ON_OFF(u8 led);
void LED_ALL(int flag);
void LED_Twinkle(int flag);


#endif
