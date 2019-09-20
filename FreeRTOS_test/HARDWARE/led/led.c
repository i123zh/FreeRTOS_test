#include "led.h"

/*******************************************************************************
* 函 数 名         : LED_Init
* 函数功能         : LED初始化函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void LED_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure;//定义结构体变量
        //启用端口复用
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);

    RCC_APB2PeriphClockCmd(LED_PORT_RCC,ENABLE);

    GPIO_InitStructure.GPIO_Pin=LED_PINa;  //选择你要设置的IO口
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	 //设置推挽输出模式
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  //设置传输速率
    GPIO_Init(LED_PORTa,&GPIO_InitStructure);        /* 初始化GPIO */
    
    GPIO_InitStructure.GPIO_Pin=LED_PINb;  //选择你要设置的IO口
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	 //设置推挽输出模式
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  //设置传输速率
    GPIO_Init(LED_PORTb,&GPIO_InitStructure);        /* 初始化GPIO */

    GPIO_ResetBits(LED_PORTa,LED_PINa);   //将LED端口拉高，熄灭所有LED
    GPIO_ResetBits(LED_PORTb,LED_PINb);   //将LED端口拉高，熄灭所有LED
}

void LED_ON_OFF(u8 led)
{
    if((led & 0x01))
        led1 = 1;
    else
        led1 = 0;
    
    if((led & 0x02))
        led2 = 1;
    else
        led2 = 0;
    
    if((led & 0x04))
        led3 = 1;
    else
        led3 = 0;
    
    if((led & 0x08))
        led4 = 1;
    else
        led4 = 0;
}

void LED_ALL(int flag)
{
    if(flag == 1)
        LED_ON_OFF(0x0F);  //全亮
    else
        LED_ON_OFF(0x00);  //全灭
}

void LED_Twinkle(int flag)
{
    if(flag == 1)
        LED_ON_OFF(0x0F);  //全亮
    else
        LED_ON_OFF(0x00);  //全灭
}

