
#include "rs485.h"
#include "timer.h"
#include "SysTick.h"
#include "protocol.h"
#include "usart.h"

u8 old_buf[60];                            //上一帧发送的数据
u8 old_len;                             //上一帧数据长度
u8 flagFrame=0;                         //帧接收完成标志，即接收到一帧新数据
u8 USART3_RX_BUF[64];                   //接收缓冲，最大64字节
u8 USART3_RX_CNT=0;                       //接收字节计数器

void rs485_init(int bound)
{
    usart3_init(bound);
    TIM2_Init();
}


//计算接收到的数据长度，并且将数据放到*buf数组中
u8 UartRead(u8 *buf, u8 len)
{
    u8 i;
    if(len>USART3_RX_CNT)           //指定读取长度大于实际接收到的数据长度时
    {
        len=USART3_RX_CNT;          //读取长度设置为实际接收到的数据长度
    }
        for(i=0;i<len;i++)          //拷贝接收到的数据到接收指针中
    {
        *buf=USART3_RX_BUF[i];      //将数据复制到buf中
        buf++;
    }
    USART3_RX_CNT=0;                //接收计数器清零
    return len;                     //返回实际读取长度
}


u8 rs485_UartWrite(u8 *buf ,u8 len)     //发送
{
    u8 i=0;
    delay_ms(3);                                                               //3MS延时
    for(i=0;i<len;i++)
    {
        old_buf[i] = buf[i];
    }
    old_len = len-2;
    for(i=0;i<len;i++)
    {
        while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET);             //检查指定的USART标志位设置与否，发送数据空位标志
        USART_SendData(USART3,buf[i]);                                       //通过USARTx外设发送单个数据
    }
    return i;
}


//串口驱动函数，检测数据帧的接收，调度功能函数，需在主循环中调用
void UartDriver()
{
//    unsigned char i=0;
//    unsigned int crc;
//    unsigned char crch,crcl;
    static u8 len;
    static u8 buf[60];
    
    if(flagFrame)            //帧接收完成标志，即接收到一帧新数据
    {
        flagFrame=0;           //帧接收完成标志清零
        len = UartRead(buf,sizeof(buf));   //将接收到的命令读到缓冲区中
        
        work1(buf, len);
        
//        crc=GetCRC16(buf,len-2);        //计算CRC校验值，除去CRC校验值
//        crch=crc>>8;                    //crc高位
//        crcl=crc&0xFF;                  //crc低位
//        
//        if((buf[len-2]==crch)&&(buf[len-1]==crcl))  //判断校验位
//        {
//            if(buf[0] != sAddr1)
//            {
//                buf[1] = Err_Addr;              //返回从机地址错误
//                len -= 2;
//                crc=GetCRC16(buf,len);           //计算CRC校验值
//                buf[len++]=crc>>8;           //CRC高字节
//                buf[len++]=crc&0xff;        //CRC低字节
//                rs485_UartWrite(buf,len);  //发送响应帧
//            }
//            else
//            {
//                switch(buf[1])
//                {
//                    case WORK1 :                 //功能1
//                        len = work1(buf, len-2);
//                        break;
//                    case WORK2 :                 //功能2
//                        len -= 2;
//                        break;
//                    case WORK3 :                 //功能3
//                        len -= 2;
//                        break;
//                    case Err_Addr :                 //从机地址错误
//                    case Err_Data :                 //数据校验错误
//                        //重新发送上一帧数据
//                        for(i=0;i<old_len;i++)
//                        {
//                            buf[i] = old_buf[i];
//                        }
//                        len = old_len;
//                        break;
//                    default :
//                        len -= 2;
//                        break;
//                }
//                crc=GetCRC16(buf,len);           //计算CRC校验值
//                buf[len++]=crc>>8;           //CRC高字节
//                buf[len++]=crc&0xff;        //CRC低字节
//                rs485_UartWrite(buf,len);  //发送响应帧
//            }
//        }
//        else
//        {
//            buf[1] = Err_Data;              //返回数据校验错误
//            len -= 2;
//            crc=GetCRC16(buf,len);           //计算CRC校验值
//            buf[len++]=crc>>8;           //CRC高字节
//            buf[len++]=crc&0xff;        //CRC低字节
//            rs485_UartWrite(buf,len);  //发送响应帧
//        }
    }
}


void UartRxMonitor(u8 ms) //串口接收监控
{
    static u8 USART3_RX_BKP=0;              //定义USART3_RC_BKP暂时存储诗句长度与实际长度比较
    static u8 idletmr=0;                    //定义监控时间
    if(USART3_RX_CNT>0)                     //接收计数器大于零时，监控总线空闲时间
    {
        if(USART3_RX_BKP!=USART3_RX_CNT)    //接收计数器改变，即刚接收到数据时，清零空闲计时
        {
            USART3_RX_BKP=USART3_RX_CNT;    //赋值操作，将实际长度给USART3_RX_BKP
            idletmr=0;                      //将监控时间清零
        }
        else                                //接收计数器未改变，即总线空闲时，累计空闲时间
        {
            //如果在一帧数据完成之前有超过3.5个字节时间的停顿，接收设备将刷新当前的消息并假定下一个字节是一个新的数据帧的开始
            if(idletmr<4)                   //空闲时间小于4ms时，持续累加
            {
                idletmr += ms;
            }
            else                            //空闲时间达到4ms时，即判定为1帧接收完毕
            {
                flagFrame=1;                //设置命令到达标志，帧接收完毕标志
            }
        }
    }
    else
    {
        USART3_RX_BKP=0;
    }
}




