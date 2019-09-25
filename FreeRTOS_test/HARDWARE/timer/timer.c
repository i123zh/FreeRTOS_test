
#include "timer.h"
#include "SysTick.h"

//通用定时器2中断初始化
//1us : arr=1, psc=35;
//1ms : arr=999, psc=72;
//1s  : arr=9999, psc=7200;
void TIM2_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);     //使能TIM2时钟
    
    TIM_TimeBaseInitStructure.TIM_Period=arr;          //自动装载值
    TIM_TimeBaseInitStructure.TIM_Prescaler=psc;           //分频系数
    TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Down; //设置向下计数模式
    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);
    
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);                //开启定时器中断
    TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
    
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;         //定时器中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1; //抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;       //子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);
    
    TIM_Cmd(TIM2,ENABLE); //使能定时器
}

//TIM2中断函数
void TIM2_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)    // 检查TIM2更新中断发生与否
    {
        TIM_ClearITPendingBit(TIM2,TIM_IT_Update);      //更新中断
        gTimer--;
//        PRINT("tim2\r\n");
    }
}


//通用定时器3中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
void TIM3_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能

    //定时器TIM3初始化
    TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位

    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

    //中断优先级NVIC设置
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级3级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //从优先级0级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
    NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器


    TIM_Cmd(TIM3, ENABLE);  //使能TIMx
}
//定时器3中断服务程序
void TIM3_IRQHandler(void)   //TIM3中断
{
    if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)    // 检查TIM2更新中断发生与否
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);      // 清除TIM2更新中断标志
        PRINT("tim3\r\n");
    }
}








