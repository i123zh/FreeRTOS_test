
#include "gprs_uart2.h"
#include "rs485.h"
#include <string.h>
#include "SysTick.h"

char gprs_buf[1024];             // gprs ���ջ���
uint32_t gprs_cnt = 0;           // ���ռ���
uint8_t usart1_rev_cnt = 0,usart1_rev_flag = 0,usart1_rev_finish = 0;  // ����1���ռ���������1���ձ�־���ڽ�����ɱ�־

/**
  * @brief  ����2�����ַ���
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
  * @brief  ����2��������
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
    uint32_t timeout;
    for(i=0;i<retime;i++)
    {
      timeout = timeOut;
        gprs_send_string(cmd);
        while(timeout--)
        {
            if(usart1_rev_finish)                                   // ������ڽ������
            {
                if(strstr(gprs_buf, res) != NULL)     // ����������ؼ���
                {
                    printf("%s\r\n", gprs_buf);
                    gprs_clear();                                   // ��ջ���
                    return 0;
                }
            }
            delay_ms(5);
        }
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

u8 *gprs_send_cmd_cpy(char *cmd,char *res,uint32_t timeOut,uint8_t retime)
{
    uint8_t i = 0, j, k;
    uint32_t timeout;
    u8 *tmp;
    for(i=0;i<retime;i++)
    {
      timeout = timeOut;
        gprs_send_string(cmd);
        while(timeout--)
        {
            if(usart1_rev_finish)                                   // ������ڽ������
            {
                if(strstr(gprs_buf, res) != NULL)     // ����������ؼ���
                {
                    k = get_gprs_data();
                    //printf("gprs_buf : %s\r\n", gprs_buf);
                    tmp = (u8 *)malloc(sizeof(u8) * 22);
                    for(j =0; j < 22; j++)
                    {
                        tmp[j] = gprs_buf[ k + j];
                    }
                    printf("tmp : %s\r\n", tmp);
                    gprs_clear();                                   // ��ջ���
                    return tmp;
                }
            }
            delay_ms(5);
        }
        gprs_clear();
    }
    return NULL; 
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


