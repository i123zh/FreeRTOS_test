#include "SysTick.h"
#include "timer.h"

//初始化延迟函数
void SysTick_Init()
{
    TIM2_Init();
}


//延时us
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

//延时ms 最大值65535
//误差1%
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




