#include "usart.h"

extern void uarst1_recv_irq(void);

int fputc(int ch,FILE *p)  //函数默认的，在使用printf函数时自动调用
{
    USART_SendData(USART2,(u8)ch);
    while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);
    return ch;
}

/*******************************************************************************
* 函 数 名         : USART1_Init
* 函数功能         : USART1初始化函数
* 输    入         : bound:波特率
* 输    出         : 无
*******************************************************************************/
void usart1_init(u32 bound)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	 //打开时钟


    /*  配置GPIO的模式和IO口 */
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//TX             //串口输出PA9
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;           //复用推挽输出
    GPIO_Init(GPIOA,&GPIO_InitStructure);  /* 初始化串口输入IO */
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//RX            //串口输入PA10
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;     //模拟输入
    GPIO_Init(GPIOA,&GPIO_InitStructure); /* 初始化GPIO */


    //USART1 初始化设置
    USART_InitStructure.USART_BaudRate = bound;//波特率设置
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(USART1, &USART_InitStructure); //初始化串口1

    USART_Cmd(USART1, ENABLE);  //使能串口1 
    USART_ClearFlag(USART1, USART_FLAG_TC); //清空发送标志位
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断

    //Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;       //子优先级0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);                 //根据指定的参数初始化VIC寄存器
}

/*******************************************************************************
* 函 数 名         : USART1_IRQHandler
* 函数功能         : USART1中断函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/ 
void USART1_IRQHandler(void)                        //串口1中断服务程序
{
//    u8 r;
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断
    {
        USART_ClearFlag(USART1,USART_IT_RXNE);
        uarst1_recv_irq();
//        r =USART_ReceiveData(USART1);//(USART1->DR);    //读取接收到的数据
//        USART_SendData(USART1,r);
        //while(USART_GetFlagStatus(USART1,USART_FLAG_TC) != SET);
    }
} 


/*******************************************************************************
* 函 数 名         : USART1_Init
* 函数功能         : USART1初始化函数
* 输    入         : bound:波特率
* 输    出         : 无
*******************************************************************************/
void usart2_parameter_config(uint32_t baud);    
void usart2_gpio_config(void);
void usart2_nvic_config(void);

//串口2初始化  
void usart2_init(uint32_t baud)
{
    usart2_gpio_config();                                     // 端口2工作模式设置
    usart2_nvic_config();
    usart2_parameter_config(baud);                            // 串口2工作参数设置
}

//串口2的GPIO端口配置化 
void usart2_gpio_config(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);  // 使能USART2和GPIOA外设时钟 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    USART_DeInit(USART2);                                   // 复位串口2

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;               // USART2_TXD(PA.2)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;         // 复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       // 设置引脚输出最大速率为50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);                  // 调用库函数中的GPIO初始化函数，初始化USART2_TXD(PA.2) 

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;               // USART2_RXD(PA.3)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   // 浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);                  // 调用库函数中的GPIO初始化函数，初始化USART2_RXD(PA.3)
}
//串口2参数配置
void usart2_parameter_config(uint32_t baud)
{
    USART_InitTypeDef   USART_InitStructure;
    USART_InitStructure.USART_BaudRate = baud;                              // 设置波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;             // 8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                  // 1个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;                     // 无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;         // 工作模式设置为收发模式
    
    USART_Init(USART2, &USART_InitStructure);                               // 初始化串口2
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);                          // 使能串口2接收中断
    USART_Cmd(USART2, ENABLE);                                              // 使能串口
}

//串口2中断向量配置
void usart2_nvic_config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;                        // 定义NVIC初始化结构体
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);             // 设置中断优先级组为2，先占优先级和从优先级各两位(可设0～3)

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;           // 串口2中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;     // 设置抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;          // 设置响应优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;             // IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************
* 函 数 名         : USART2_IRQHandler
* 函数功能         : USART2中断函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/ 
void USART2_IRQHandler(void)                                //串口1中断服务程序
{
    u8 r;
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)   //接收中断
    {
        r = USART_ReceiveData(USART2);                      //读取接收到的数据
        USART_SendData(USART2,r);                           //回显
        while(USART_GetFlagStatus(USART2,USART_FLAG_TC) != SET);
    } 
    USART_ClearFlag(USART2,USART_FLAG_TC);
} 




/*******************************************************************************
* 函 数 名         : RS485_Init
* 函数功能         : USART3初始化函数
* 输    入         : bound:波特率
* 输    出         : 无
*******************************************************************************/  
void usart3_init(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE); //使能时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);   //使能USART3时钟
    
    /*  配置GPIO的模式和IO口 */
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;                //串口输出PB10
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;           //复用推挽输出
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO_InitStructure);           /* 初始化串口输入IO */
    
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;                //串口输入PB11
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;	    //模拟输入
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
    
    //USART3 初始化设置
    USART_InitStructure.USART_BaudRate = bound;                         //波特率设置
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;         //字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;              //一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;                 //无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;     //收发模式
    USART_Init(USART3, &USART_InitStructure);                           //初始化串口3
    
    //USART3 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2; //抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;       //子优先级2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);                         //根据指定的参数初始化VIC寄存器、
    
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);          //开启接受中断
    USART_Cmd(USART3, ENABLE);                              //使能串口 3
}


extern u8 USART3_RX_BUF[64];  //接收缓存，最大64字节
extern u8 USART3_RX_CNT;

/*******************************************************************************
* 函 数 名         : USART3_IRQHandler
* 函数功能         : USART3中断函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/ 
void USART3_IRQHandler(void)
{
    u8 res;	                                                //定义数据缓存变量
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)   //接收到数据
    {
        res =USART_ReceiveData(USART3);                     //读取接收到的数据USART3->DR
        if(USART3_RX_CNT<sizeof(USART3_RX_BUF))             //一次只能接收64个字节，人为设定，可以更大，但浪费时间
        {
            USART3_RX_BUF[USART3_RX_CNT]=res;               //记录接收到的值
            USART3_RX_CNT++;                                //使收数据增加1 
        }
    }
}





