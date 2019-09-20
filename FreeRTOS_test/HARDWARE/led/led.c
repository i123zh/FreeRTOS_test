#include "led.h"

/*******************************************************************************
* �� �� ��         : LED_Init
* ��������         : LED��ʼ������
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void LED_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure;//����ṹ�����
        //���ö˿ڸ���
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);

    RCC_APB2PeriphClockCmd(LED_PORT_RCC,ENABLE);

    GPIO_InitStructure.GPIO_Pin=LED_PINa;  //ѡ����Ҫ���õ�IO��
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	 //�����������ģʽ
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  //���ô�������
    GPIO_Init(LED_PORTa,&GPIO_InitStructure);        /* ��ʼ��GPIO */
    
    GPIO_InitStructure.GPIO_Pin=LED_PINb;  //ѡ����Ҫ���õ�IO��
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	 //�����������ģʽ
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  //���ô�������
    GPIO_Init(LED_PORTb,&GPIO_InitStructure);        /* ��ʼ��GPIO */

    GPIO_ResetBits(LED_PORTa,LED_PINa);   //��LED�˿����ߣ�Ϩ������LED
    GPIO_ResetBits(LED_PORTb,LED_PINb);   //��LED�˿����ߣ�Ϩ������LED
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
        LED_ON_OFF(0x0F);  //ȫ��
    else
        LED_ON_OFF(0x00);  //ȫ��
}

void LED_Twinkle(int flag)
{
    if(flag == 1)
        LED_ON_OFF(0x0F);  //ȫ��
    else
        LED_ON_OFF(0x00);  //ȫ��
}

