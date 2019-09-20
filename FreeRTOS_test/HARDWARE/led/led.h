#ifndef _led_H
#define _led_H

#include "system.h"

/*  LED时钟端口、引脚定义 */
#define LED_PORTa        GPIOA
#define LED_PINa         GPIO_Pin_15 | GPIO_Pin_4 | GPIO_Pin_11
#define LED_PORTb        GPIOB
#define LED_PINb         GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2
#define LED_PORT_RCC    RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB

#define led0 PAout(15)  //运行灯
#define led1 PAout(4)   //高压
#define led2 PBout(0)   //吸尘
#define led3 PBout(1)   //泡沫
#define led4 PBout(2)   //清水
#define led5 PAout(11)  //结束

#define LED0 (1 << 0)
#define LED1 (1 << 1)

void LED_Init(void);
void LED_ON_OFF(u8 led);
void LED_ALL(int flag);
void LED_Twinkle(int flag);


#endif
