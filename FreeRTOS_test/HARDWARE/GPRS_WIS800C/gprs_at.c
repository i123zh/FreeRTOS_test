
#include "gprs_at.h"
#include "usart.h"
#include "SysTick.h"
#include "gprs_data_unit.h"


extern EventGroupHandle_t flag_gprs;  //Gprs��־��

char gprs_buf[256];             // gprs ���ջ���
uint32_t gprs_cnt = 0;           // ���ռ���
uint8_t usart1_rev_cnt = 0;


void Gprs_Init(void)
{
    usart1_init(115200);
    pen_gpio_config();      //���ú��İ�GPIO
    delay_ms(30);
    PEN_ON;
}


/**
  * @brief  gpio_config
  * @param  ���� GPIO ����
  * @retval ��
  * @note   PENΪGPRS���İ�ʹ�����ţ�LEDΪ��Ƭ���ϵ�LED��
  */
void pen_gpio_config(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);    // ʹ��GPIO_RCC
    
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_13;                // ѡ��Ҫ��ʼ����GPIOB����PB1
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;        // �������Ź���ģʽΪͨ���������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       // ������������������Ϊ50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    PEN_OFF;
}

/**
  * @brief  GPRS ��ʼ������
  * @param  ��
  * @retval ��
  * @note   ��
  */
uint8_t gprs_init(void)
{
    PRINT("+++\r\n");                        // ����AT����ͨ���Ƿ�����
    vDelay_ms(500);                                      // ��ֹ���İ廹��������͸��ģʽ������ר�ã�����ԣ�
    gprs_send_string("+++");                             // ��ֹ���İ廹��������͸��ģʽ������ר�ã�����ԣ�
    vDelay_ms(500);                                      // ��ֹ���İ廹��������͸��ģʽ������ר�ã�����ԣ�

    PRINT("AT\r\n");                                   // ����AT����ͨ���Ƿ�����
    if(gprs_send_cmd("AT\r\n","OK",500,5))
    {
        return 1;
    }
    PRINT("ATE0\r\n");                                 // ȡ�����İ���Թ���
    if(gprs_send_cmd("ATE0\r\n","OK",200,2))
    {
        return 2;
    }
    PRINT("AT+CPIN?\r\n");                              // ��ѯ���İ��Ƿ������
    if(gprs_send_cmd("AT+CPIN?\r\n","+CPIN: READY",200,2))
    {
        return 3;
    }
    PRINT("AT+CSQ\r\n");                                 //��ѯ�ź�ǿ��
    if(gprs_send_cmd("AT+CSQ\r\n","OK",500,5))
    {
        return 4;
    }
    PRINT("AT+CREG?\r\n");                              // ��ѯ����ע����������İ���Զ�ע�����磬�ϵ絽ע���� 10s ����
    if(gprs_send_cmd("AT+CREG?\r\n","0,1",500,5))
    {
        return 4;
    }
    PRINT("AT+CGATT?\r\n");                              // ��������
    if(gprs_send_cmd("AT+CGATT?\r\n","+CGATT: 1",500,1))  // �������ע��ɹ�����һֱ���Ų������磬�����ʹ��AT+CGATT=1
    {
        return 5;
    }
    PRINT("GPRS ��ʼ�����\r\n");
    xEventGroupSetBits(flag_gprs, Gprs_Init_Flag);        // ��ʼ����ɣ���־λ��1
    return 0;
}

/**
  * @brief  GPRS ��������
  * @param  ��
  * @retval �������ý��������������� err_process ��������
  * @note   ��
  */
uint8_t gprs_net_config(void)
{
    PRINT("AT+CIPSHUT\r\n");                          // �ر��ƶ�����
    if(gprs_send_cmd("AT+CIPSHUT\r\n","OK",500,2))
    {
        return 10;
    }

    PRINT("AT+CIPMODE=1\r\n");                        // ����Ϊ͸��ģʽ
    if(gprs_send_cmd("AT+CIPMODE=1\r\n","OK",200,2))
    {
        return 11;
    }
    PRINT("AT+CIPMUX=0\r\n");                         // ����Ϊ��·���ӣ�͸��ģʽֻ���ǵ�·����
    if(gprs_send_cmd("AT+CIPMUX=0\r\n","OK",200,2))
    {
        return 12;
    }
    PRINT("AT+CSTT=\"CMNET\"\r\n");                   // ����APN
    if(gprs_send_cmd("AT+CSTT=\"CMNET\"\r\n","OK",500,2))
    {
        return 13;
    }
    PRINT("AT+CIICR\r\n");                            // �����ƶ�����
    if(gprs_send_cmd("AT+CIICR\r\n","OK",500,5))
    {
        return 14;
    }
    PRINT("AT+CIFSR\r\n");                            // ��ȡ����IP
    if(gprs_send_cmd("AT+CIFSR\r\n",".",300,1))    
    {
        return 15;
    }
    xEventGroupSetBits(flag_gprs, Gprs_Net_Flag);      // ������ɣ���־λ��1
    return 0;
}

/**
  * @brief  ���� TCP ������
  * @param  ��
  * @retval �������ý��������������� err_process ��������
  * @note   ��
  */
uint8_t gprs_connect(void)                          // ����TCP����
{
    char *tcp_ip;            // ������IP�Ͷ˿ں�
    //Ĭ�ϣ�"47.106.99.158" "8888"
    tcp_ip = (char *)malloc(sizeof(char) * (23 + 16 + 5));
    //sprintf(tcp_ip, "AT+CIPSTART=\"TCP\",\"%s\",%s\r\n", ip_itos(Eeprom_data.IpAddr), port_itos(Eeprom_data.PortNum));
    sprintf(tcp_ip, "AT+CIPSTART=\"TCP\",\"47.106.99.158\",8888\r\n");
    PRINT("tcp_ip : %s\r\n", tcp_ip);
    if(gprs_send_cmd((char*)tcp_ip,"OK",500,2))
    {
         return 20;
    }
    xEventGroupSetBits(flag_gprs, Gprs_Connect_Flag);
    free(tcp_ip);
    return 0;
}

/**
  * @brief  �����ַ���
  * @param  str��Ҫ���͵�����
  *         len�����ݳ���
  * @retval ��
  * @note   ��
  */
void gprs_send_string(char* str)
{
    while(*str)                                                     // ����ַ�����������������ѭ��
    {
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE)==RESET); 
        USART_SendData(USART1 ,*str++);                             // ���͵�ǰ�ַ�
        while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET); 
    }
}
void usart1_send_string(unsigned char *str, unsigned short len)
{
    uint8_t count = 0;
    for(; count < len; count++)
    {
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);     // �ȴ��������
        USART_SendData(USART1, *str++);                                 // ��������
        while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);     // �ȴ��������
    }
}

/**
  * @brief  ��������
  * @param  cmd������
  *         res����Ҫ���ķ���ָ��
  *         timeOut ����ʱ��
  *         resnum ����ʹ���
  * @retval ��
  * @note   0-�ɹ�  1-ʧ��
  */
uint8_t gprs_send_cmd(char *cmd,char *res,uint32_t timeOut,uint8_t retime)
{
    uint8_t i = 0;
    TickType_t timeout;
    EventBits_t uBit;
    for(i=0;i<retime;i++)
    {
        timeout = timeOut;
        gprs_send_string(cmd);
        uBit = xEventGroupWaitBits(flag_gprs, Gprs_RevFinish_Flag, pdFALSE, pdTRUE, timeout);
        while(timeout--)
        {
            uBit = xEventGroupGetBits(flag_gprs);
            if((uBit & Gprs_RevFinish_Flag))                                   // ������ڽ������
            {
                if(strstr(gprs_buf, res) != NULL)     // ����������ؼ���
                {
                    PRINT("%s\r\n", gprs_buf);
                    gprs_clear();                                   // ��ջ���
                    return 0;
                }
            }
            vDelay_ms(5);
        }
        PRINT("\r\n===================\r\n");
        PRINT("%d bit : %04x\r\n", timeout, uBit);
        gprs_clear();
    }
    return 1;
}

int get_gprs_data()
{
    int i = 0;
    while(1)
    {
        if((gprs_buf[i] == '\r') && (gprs_buf[i+1] == '\n'))
            return i+2;
        else
            i++;
    }
}

/**
  * @brief  GPRS��ջ���
  * @param  ��
  * @retval ��
  * @note   ��
  */
void gprs_clear(void)
{
    memset(gprs_buf, 0, sizeof(gprs_buf));
    gprs_cnt = 0;
}

void uarst1_recv_irq(void)
{
    xEventGroupSetBits(flag_gprs, Gprs_Rev_Flag);            // ����2 ���ձ�־
    usart1_rev_cnt = 0;                                      // ����2���ռ�������
    gprs_buf[gprs_cnt++] = USART1->DR;                       // ���뻺������
    if(gprs_cnt >= sizeof(gprs_buf))
    {
         gprs_cnt = 0;                                       // ��ֹ����������
    }
}


