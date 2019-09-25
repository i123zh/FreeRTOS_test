#include "SysTick.h"
#include "timer.h"

volatile int gTimer;

//初始化延迟函数
void SysTick_Init(void)
{
    TIM2_Init(1, 35);
}

//延时 2 * n us
void delay_us(u32 nus)
{
    gTimer = nus;
    while(gTimer > 0)
        ;
}

//延时 1 * n ms
void delay_ms(u16 nms)
{
    gTimer = nms * 500;
    while(gTimer > 0)
        ;
}

void vDelay_ms(u16 nms)
{
    vTaskDelay(nms/portTICK_RATE_MS);
}



