
#include "gprs_data_unit.h"

#include "SysTick.h"
#include "gprs_at.h"
#include "protocol.h"

uint8_t err_cnt = 0,err_num = 0;   // ERROR ����������ֵ

extern EventGroupHandle_t flag_gprs;  //Gprs��־��renwu
  
/**
  * @brief  ��������
  * @param  ����ֵ
  * @retval ��
  * @note   ��������������������������Σ����������İ塣
  */
void err_process(uint8_t err)
{
    PRINT("��ʼ���д�����\r\n");
    switch(err)
    {
        case 1:
            PRINT("AT ERROR\r\n");
            err_cnt = 4;               // �������AT������İ�û�з�Ӧ����ֱ��������
        break;
        case 2:
            PRINT("�򿪻���ʧ��\r\n");
        break;
        case 3:
            PRINT("����ʧ�ܣ�����SIM���Ƿ��Ѳ���\r\n");
        break;
        case 4:
            PRINT("����ע��ʧ��\r\n");
        break;
        case 5:
            PRINT("���總��ʧ��\r\n");
        break;
        case 10:
            PRINT("�ر��ƶ�����ʧ��\r\n");
        break;
        case 11:
            PRINT("����Ϊ��͸��ģʽʧ��\r\n");	
        break;
        case 12:
            PRINT("����Ϊ����·ģʽʧ��\r\n");
        break;
        case 13:
            PRINT("����APNʧ��\r\n");
        break;
        case 14:
            PRINT("�����ƶ�����ʧ��\r\n");
        break;
        case 15:
            PRINT("δ��ѯ������IP\r\n");
        break;
        case 20:
            PRINT("����������ʧ�ܣ�����������\r\n");
        break;
        case 21:
            PRINT("δ�յ����������ݣ�������������Ͽ������ӣ�����������\r\n");
        break;
        default:
            PRINT("��ʼ�����\r\n");
    }
    // ��־λ��0
    xEventGroupClearBits(flag_gprs, (Gprs_Init_Flag | Gprs_Net_Flag | Gprs_Connect_Flag | Gprs_Server_Flag));
    err_cnt++;
    if(err_cnt >= 3)                    // ������ڵ�������
    {
        err_cnt = 0;
        err_num = 0;
        gprs_reset();                   // �������İ�
    }
}

/**
  * @brief  ����ģ��
  * @param  ��
  * @retval ��
  * @note   �ػ�������������İ�
  */
void gprs_reset(void)
{
    PRINT("����ģ��\r\n");
    PEN_OFF;
    vDelay_ms(3000);
    PEN_ON;
}

/**
  * @brief  gprs���մ���
  * @param  ��
  * @retval ��
  * @note   ��
  */
void gprs_receive_process(void)
{
    if(((xEventGroupGetBits(flag_gprs)) & Gprs_RevFinish_Flag))
    {
        xEventGroupClearBits(flag_gprs, Gprs_RevFinish_Flag);
        PRINT("recv : %s\r\n", gprs_buf);
        
        gprs_clear();                              // �������
    }
}

/**
  * @brief  ��������
  * @param  ��
  * @retval ��
  * @note   ������ʵ����ָ���Ƕ�ʱ�����������һ�����ݣ���ά�ֳ����ӡ����ݿ����Լ����塣
  */
uint8_t heard_beat_process(void)
{
    if(((xEventGroupGetBits(flag_gprs)) & Gprs_Heart_Flag))       // ������־Ϊ1
    {
        xEventGroupClearBits(flag_gprs, Gprs_Heart_Flag);         // �����������
        //send_heart();  //������������
    }
    if(((xEventGroupGetBits(flag_gprs)) & Gprs_Server_Flag))
    {
        PRINT("��������Ͽ�����\r\n");
        return 21;
    }
    return 0;
}

//��ʱ���жϺ���
void gprs_interrupt_process(void)
{
    static uint16_t tim_cnt1 = 0,tim_cnt2 = 0;           // ��ʱ������
    static uint8_t ser_hb_cnt = 0,heart_beat_cnt = 0;    // ������������������������
    
    BaseType_t xResult;
    BaseType_t hp_task = pdFALSE;
    
    EventBits_t uBit;
    uBit = xEventGroupGetBits(flag_gprs);

    /* ����1 ���մ��� */
    if((uBit & Gprs_Rev_Flag))                                // ��� usart1 �������ݱ�־Ϊ1
    {
        heart_beat_cnt = 0;                             // �����������ݹ���ʱ��������������
        ser_hb_cnt = 0;
        tim_cnt1 = 0;
        tim_cnt2 = 0;
        usart1_rev_cnt++;                                // usart2 ���ռ���	
        if(usart1_rev_cnt >= 3)                          // ������ 3 ms δ���յ����ݣ�����Ϊ���ݽ�����ɡ�
        {
            xEventGroupClearBits(flag_gprs, Gprs_Rev_Flag);
            usart1_rev_cnt = 0;
            xResult = xEventGroupSetBitsFromISR(flag_gprs, Gprs_RevFinish_Flag, &hp_task);
            if(xResult != pdFAIL)
            {
                portYIELD_FROM_ISR(hp_task);
            }
        }
    }
    /* GPRS ���İ��������� */
    if(!(uBit & Gprs_Rev_Flag) && (uBit & Gprs_Connect_Flag))         // ��GPRS���İ彨���������Ҵ���û�н��յ�����
    {
        tim_cnt1++;                                    // ��ʼ��������
        if(tim_cnt1 >= 1000)
        { 
            tim_cnt1 = 0;
            heart_beat_cnt++;
            if(heart_beat_cnt >= 60)                  // �����ﵽ 30s ��������־�� 1�����Ը���ʵ�������޸ĸ�ֵ��
            {
                heart_beat_cnt = 0;
                xResult = xEventGroupSetBitsFromISR(flag_gprs, Gprs_Heart_Flag, &hp_task);
                if(xResult != pdFAIL)
                {
                    portYIELD_FROM_ISR(hp_task);
                }
            }
        }
    }

    /* �������������� */
//    if((uBit & Gprs_Connect_Flag))                               // GPRS ���ӱ�־Ϊ 1
//    {
//        tim_cnt2++;                                       // tim����
//        if(tim_cnt2 >= 1000)                              // ��ʱ�� 1s
//        {
//            tim_cnt2 = 0;                                   // tim ��������
//            ser_hb_cnt++;                                   // ��������������
//            if(ser_hb_cnt >= 80)                            // �� 40s ��δ���ܵ����������ݣ�����������
//            {
//                ser_hb_cnt = 0;
//                xResult = xEventGroupSetBitsFromISR(flag_gprs, Gprs_Server_Flag, &hp_task);
//                if(xResult != pdFAIL)
//                {
//                    portYIELD_FROM_ISR(hp_task);
//                }
//            }
//        }
//    }
}


//��Ҫ���������ӷ����������ݷ�����ָ��ִ�й���
void connect_server(void)
{
    EventBits_t uBit;
    uBit = xEventGroupGetBits(flag_gprs);
    
    if(!(uBit & Gprs_Init_Flag))                 // ������İ�δ��ʼ��
    { 
        err_num = gprs_init();            // ���İ��ʼ����������ʼ����������� err_num
    }
    else 
    {
        err_num = gprs_net_config();      // ���İ��������� 
    }
        
    if((uBit & Gprs_Init_Flag) && (uBit & Gprs_Net_Flag)) // ������İ��ʼ�����������ö����
    {
        err_num = gprs_connect();         // ��ʼ TCP ����
    }
    
    if((uBit & Gprs_Init_Flag) && (uBit & Gprs_Connect_Flag) && (uBit & Gprs_Net_Flag) && (!err_num))  //���ݴ���
    {
        vDelay_ms(100);
        //��ʼ����
        while(!err_num)                         // ������������ɣ�ͨ�Ŵ���
        {
            gprs_receive_process();            // ����TCP���͹���������
            err_num = heard_beat_process();     //����
        }
    }
    
    if(((!(uBit & Gprs_Init_Flag)) || (!(uBit & Gprs_Net_Flag)) || (!(uBit & Gprs_Connect_Flag)) || (uBit & Gprs_Server_Flag)) && err_num)  // ����δͨ��
    {
        err_process(err_num);                // AT���������
    }
}




