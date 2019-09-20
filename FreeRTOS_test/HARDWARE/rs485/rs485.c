
#include "rs485.h"
#include "timer.h"
#include "SysTick.h"
#include "protocol.h"
#include "usart.h"

u8 old_buf[60];                            //��һ֡���͵�����
u8 old_len;                             //��һ֡���ݳ���
u8 flagFrame=0;                         //֡������ɱ�־�������յ�һ֡������
u8 USART3_RX_BUF[64];                   //���ջ��壬���64�ֽ�
u8 USART3_RX_CNT=0;                       //�����ֽڼ�����

void rs485_init(int bound)
{
    usart3_init(bound);
    TIM2_Init();
}


//������յ������ݳ��ȣ����ҽ����ݷŵ�*buf������
u8 UartRead(u8 *buf, u8 len)
{
    u8 i;
    if(len>USART3_RX_CNT)           //ָ����ȡ���ȴ���ʵ�ʽ��յ������ݳ���ʱ
    {
        len=USART3_RX_CNT;          //��ȡ��������Ϊʵ�ʽ��յ������ݳ���
    }
        for(i=0;i<len;i++)          //�������յ������ݵ�����ָ����
    {
        *buf=USART3_RX_BUF[i];      //�����ݸ��Ƶ�buf��
        buf++;
    }
    USART3_RX_CNT=0;                //���ռ���������
    return len;                     //����ʵ�ʶ�ȡ����
}


u8 rs485_UartWrite(u8 *buf ,u8 len)     //����
{
    u8 i=0;
    delay_ms(3);                                                               //3MS��ʱ
    for(i=0;i<len;i++)
    {
        old_buf[i] = buf[i];
    }
    old_len = len-2;
    for(i=0;i<len;i++)
    {
        while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET);             //���ָ����USART��־λ������񣬷������ݿ�λ��־
        USART_SendData(USART3,buf[i]);                                       //ͨ��USARTx���跢�͵�������
    }
    return i;
}


//���������������������֡�Ľ��գ����ȹ��ܺ�����������ѭ���е���
void UartDriver()
{
//    unsigned char i=0;
//    unsigned int crc;
//    unsigned char crch,crcl;
    static u8 len;
    static u8 buf[60];
    
    if(flagFrame)            //֡������ɱ�־�������յ�һ֡������
    {
        flagFrame=0;           //֡������ɱ�־����
        len = UartRead(buf,sizeof(buf));   //�����յ������������������
        
        work1(buf, len);
        
//        crc=GetCRC16(buf,len-2);        //����CRCУ��ֵ����ȥCRCУ��ֵ
//        crch=crc>>8;                    //crc��λ
//        crcl=crc&0xFF;                  //crc��λ
//        
//        if((buf[len-2]==crch)&&(buf[len-1]==crcl))  //�ж�У��λ
//        {
//            if(buf[0] != sAddr1)
//            {
//                buf[1] = Err_Addr;              //���شӻ���ַ����
//                len -= 2;
//                crc=GetCRC16(buf,len);           //����CRCУ��ֵ
//                buf[len++]=crc>>8;           //CRC���ֽ�
//                buf[len++]=crc&0xff;        //CRC���ֽ�
//                rs485_UartWrite(buf,len);  //������Ӧ֡
//            }
//            else
//            {
//                switch(buf[1])
//                {
//                    case WORK1 :                 //����1
//                        len = work1(buf, len-2);
//                        break;
//                    case WORK2 :                 //����2
//                        len -= 2;
//                        break;
//                    case WORK3 :                 //����3
//                        len -= 2;
//                        break;
//                    case Err_Addr :                 //�ӻ���ַ����
//                    case Err_Data :                 //����У�����
//                        //���·�����һ֡����
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
//                crc=GetCRC16(buf,len);           //����CRCУ��ֵ
//                buf[len++]=crc>>8;           //CRC���ֽ�
//                buf[len++]=crc&0xff;        //CRC���ֽ�
//                rs485_UartWrite(buf,len);  //������Ӧ֡
//            }
//        }
//        else
//        {
//            buf[1] = Err_Data;              //��������У�����
//            len -= 2;
//            crc=GetCRC16(buf,len);           //����CRCУ��ֵ
//            buf[len++]=crc>>8;           //CRC���ֽ�
//            buf[len++]=crc&0xff;        //CRC���ֽ�
//            rs485_UartWrite(buf,len);  //������Ӧ֡
//        }
    }
}


void UartRxMonitor(u8 ms) //���ڽ��ռ��
{
    static u8 USART3_RX_BKP=0;              //����USART3_RC_BKP��ʱ�洢ʫ�䳤����ʵ�ʳ��ȱȽ�
    static u8 idletmr=0;                    //������ʱ��
    if(USART3_RX_CNT>0)                     //���ռ�����������ʱ��������߿���ʱ��
    {
        if(USART3_RX_BKP!=USART3_RX_CNT)    //���ռ������ı䣬���ս��յ�����ʱ��������м�ʱ
        {
            USART3_RX_BKP=USART3_RX_CNT;    //��ֵ��������ʵ�ʳ��ȸ�USART3_RX_BKP
            idletmr=0;                      //�����ʱ������
        }
        else                                //���ռ�����δ�ı䣬�����߿���ʱ���ۼƿ���ʱ��
        {
            //�����һ֡�������֮ǰ�г���3.5���ֽ�ʱ���ͣ�٣������豸��ˢ�µ�ǰ����Ϣ���ٶ���һ���ֽ���һ���µ�����֡�Ŀ�ʼ
            if(idletmr<4)                   //����ʱ��С��4msʱ�������ۼ�
            {
                idletmr += ms;
            }
            else                            //����ʱ��ﵽ4msʱ�����ж�Ϊ1֡�������
            {
                flagFrame=1;                //����������־��֡������ϱ�־
            }
        }
    }
    else
    {
        USART3_RX_BKP=0;
    }
}




