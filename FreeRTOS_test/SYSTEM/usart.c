#include "usart.h"

extern void uarst1_recv_irq(void);

int fputc(int ch,FILE *p)  //����Ĭ�ϵģ���ʹ��printf����ʱ�Զ�����
{
    USART_SendData(USART2,(u8)ch);
    while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);
    return ch;
}

/*******************************************************************************
* �� �� ��         : USART1_Init
* ��������         : USART1��ʼ������
* ��    ��         : bound:������
* ��    ��         : ��
*******************************************************************************/
void usart1_init(u32 bound)
{
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	 //��ʱ��


    /*  ����GPIO��ģʽ��IO�� */
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//TX             //�������PA9
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;           //�����������
    GPIO_Init(GPIOA,&GPIO_InitStructure);  /* ��ʼ����������IO */
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//RX            //��������PA10
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;     //ģ������
    GPIO_Init(GPIOA,&GPIO_InitStructure); /* ��ʼ��GPIO */


    //USART1 ��ʼ������
    USART_InitStructure.USART_BaudRate = bound;//����������
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
    USART_Init(USART1, &USART_InitStructure); //��ʼ������1

    USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1 
    USART_ClearFlag(USART1, USART_FLAG_TC); //��շ��ͱ�־λ
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������ж�

    //Usart1 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//����1�ж�ͨ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;       //�����ȼ�0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);                 //����ָ���Ĳ�����ʼ��VIC�Ĵ���
}

/*******************************************************************************
* �� �� ��         : USART1_IRQHandler
* ��������         : USART1�жϺ���
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/ 
void USART1_IRQHandler(void)                        //����1�жϷ������
{
//    u8 r;
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�
    {
        USART_ClearFlag(USART1,USART_IT_RXNE);
        uarst1_recv_irq();
//        r =USART_ReceiveData(USART1);//(USART1->DR);    //��ȡ���յ�������
//        USART_SendData(USART1,r);
        //while(USART_GetFlagStatus(USART1,USART_FLAG_TC) != SET);
    }
} 


/*******************************************************************************
* �� �� ��         : USART1_Init
* ��������         : USART1��ʼ������
* ��    ��         : bound:������
* ��    ��         : ��
*******************************************************************************/
void usart2_parameter_config(uint32_t baud);    
void usart2_gpio_config(void);
void usart2_nvic_config(void);

//����2��ʼ��  
void usart2_init(uint32_t baud)
{
    usart2_gpio_config();                                     // �˿�2����ģʽ����
    usart2_nvic_config();
    usart2_parameter_config(baud);                            // ����2������������
}

//����2��GPIO�˿����û� 
void usart2_gpio_config(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);  // ʹ��USART2��GPIOA����ʱ�� 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    USART_DeInit(USART2);                                   // ��λ����2

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;               // USART2_TXD(PA.2)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;         // �����������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       // ������������������Ϊ50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);                  // ���ÿ⺯���е�GPIO��ʼ����������ʼ��USART2_TXD(PA.2) 

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;               // USART2_RXD(PA.3)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   // ��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);                  // ���ÿ⺯���е�GPIO��ʼ����������ʼ��USART2_RXD(PA.3)
}
//����2��������
void usart2_parameter_config(uint32_t baud)
{
    USART_InitTypeDef   USART_InitStructure;
    USART_InitStructure.USART_BaudRate = baud;                              // ���ò�����
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;             // 8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                  // 1��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;                     // ����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;         // ����ģʽ����Ϊ�շ�ģʽ
    
    USART_Init(USART2, &USART_InitStructure);                               // ��ʼ������2
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);                          // ʹ�ܴ���2�����ж�
    USART_Cmd(USART2, ENABLE);                                              // ʹ�ܴ���
}

//����2�ж���������
void usart2_nvic_config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;                        // ����NVIC��ʼ���ṹ��
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);             // �����ж����ȼ���Ϊ2����ռ���ȼ��ʹ����ȼ�����λ(����0��3)

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;           // ����2�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;     // ������ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;          // ������Ӧ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;             // IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************
* �� �� ��         : USART2_IRQHandler
* ��������         : USART2�жϺ���
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/ 
void USART2_IRQHandler(void)                                //����1�жϷ������
{
    u8 r;
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)   //�����ж�
    {
        r = USART_ReceiveData(USART2);                      //��ȡ���յ�������
        USART_SendData(USART2,r);                           //����
        while(USART_GetFlagStatus(USART2,USART_FLAG_TC) != SET);
    } 
    USART_ClearFlag(USART2,USART_FLAG_TC);
} 




/*******************************************************************************
* �� �� ��         : RS485_Init
* ��������         : USART3��ʼ������
* ��    ��         : bound:������
* ��    ��         : ��
*******************************************************************************/  
void usart3_init(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE); //ʹ��ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);   //ʹ��USART3ʱ��
    
    /*  ����GPIO��ģʽ��IO�� */
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;                //�������PB10
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;           //�����������
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO_InitStructure);           /* ��ʼ����������IO */
    
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;                //��������PB11
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;	    //ģ������
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
    
    //USART3 ��ʼ������
    USART_InitStructure.USART_BaudRate = bound;                         //����������
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;         //�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;              //һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;                 //����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;     //�շ�ģʽ
    USART_Init(USART3, &USART_InitStructure);                           //��ʼ������3
    
    //USART3 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2; //��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;       //�����ȼ�2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);                         //����ָ���Ĳ�����ʼ��VIC�Ĵ�����
    
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);          //���������ж�
    USART_Cmd(USART3, ENABLE);                              //ʹ�ܴ��� 3
}


extern u8 USART3_RX_BUF[64];  //���ջ��棬���64�ֽ�
extern u8 USART3_RX_CNT;

/*******************************************************************************
* �� �� ��         : USART3_IRQHandler
* ��������         : USART3�жϺ���
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/ 
void USART3_IRQHandler(void)
{
    u8 res;	                                                //�������ݻ������
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)   //���յ�����
    {
        res =USART_ReceiveData(USART3);                     //��ȡ���յ�������USART3->DR
        if(USART3_RX_CNT<sizeof(USART3_RX_BUF))             //һ��ֻ�ܽ���64���ֽڣ���Ϊ�趨�����Ը��󣬵��˷�ʱ��
        {
            USART3_RX_BUF[USART3_RX_CNT]=res;               //��¼���յ���ֵ
            USART3_RX_CNT++;                                //ʹ����������1 
        }
    }
}





