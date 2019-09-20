#include "SysTick.h"
#include "timer.h"

//��ʼ���ӳٺ���
void SysTick_Init()
{
    TIM2_Init();
}


//��ʱus
void delay_us(u32 nus)
{
    u16 counter=nus&0xffff;
    TIM_Cmd(TIM2,ENABLE);
    TIM_SetCounter(TIM2,counter);
    while(counter>1)
    {
        counter=TIM_GetCounter(TIM2);
    }
    TIM_Cmd(TIM2,DISABLE);
}

//��ʱms ���ֵ65535
//���1%
void delay_ms(u16 nms)
{
    int i = 0;
    u16 counter = 0;
    TIM_Cmd(TIM2,ENABLE);
    for(i = 0; i < 1000; i++)
    {
        counter=nms;
        TIM_SetCounter(TIM2,counter);
        while(counter>1)
        {
            counter=TIM_GetCounter(TIM2);
        }
    }
    TIM_Cmd(TIM2,DISABLE);
}

void vDelay_ms(u16 nms)
{
    vTaskDelay(nms/portTICK_RATE_MS);
}




