
#include "timer.h"
#include "SysTick.h"

//ͨ�ö�ʱ��2�жϳ�ʼ��
//1us : arr=1, psc=35;
//1ms : arr=999, psc=72;
//1s  : arr=9999, psc=7200;
void TIM2_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);     //ʹ��TIM2ʱ��
    
    TIM_TimeBaseInitStructure.TIM_Period=arr;          //�Զ�װ��ֵ
    TIM_TimeBaseInitStructure.TIM_Prescaler=psc;           //��Ƶϵ��
    TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Down; //�������¼���ģʽ
    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);
    
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);                //������ʱ���ж�
    TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
    
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;         //��ʱ���ж�ͨ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1; //��ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;       //�����ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);
    
    TIM_Cmd(TIM2,ENABLE); //ʹ�ܶ�ʱ��
}

//TIM2�жϺ���
void TIM2_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)    // ���TIM2�����жϷ������
    {
        TIM_ClearITPendingBit(TIM2,TIM_IT_Update);      //�����ж�
        gTimer--;
//        PRINT("tim2\r\n");
    }
}


//ͨ�ö�ʱ��3�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
void TIM3_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��

    //��ʱ��TIM3��ʼ��
    TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
    TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

    //�ж����ȼ�NVIC����
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //��ռ���ȼ�3��
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //�����ȼ�0��
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
    NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���


    TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx
}
//��ʱ��3�жϷ������
void TIM3_IRQHandler(void)   //TIM3�ж�
{
    if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)    // ���TIM2�����жϷ������
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);      // ���TIM2�����жϱ�־
        PRINT("tim3\r\n");
    }
}








