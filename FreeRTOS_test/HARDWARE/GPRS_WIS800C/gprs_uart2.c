
#include "gprs_uart2.h"
#include "rs485.h"
#include <string.h>
#include "SysTick.h"

char gprs_buf[1024];             // gprs 接收缓存
uint32_t gprs_cnt = 0;           // 接收计数
uint8_t usart1_rev_cnt = 0,usart1_rev_flag = 0,usart1_rev_finish = 0;  // 串口1接收计数，串口1接收标志串口接收完成标志

/**
  * @brief  串口2发送字符串
  * @param  str：要发送的数据
  *         len：数据长度
  * @retval 无
  * @note   无
  */
void gprs_send_string(char* str)
{
    while(*str)                                                     // 检测字符串结束符，则跳出循环
    {
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE)==RESET); 
        USART_SendData(USART1 ,*str++);                             // 发送当前字符
        while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET); 
    }
}
void usart1_send_string(unsigned char *str, unsigned short len)
{
    uint8_t count = 0;
    for(; count < len; count++)
    {
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);     // 等待发送完成
        USART_SendData(USART1, *str++);                                 // 发送数据
        while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);     // 等待发送完成
    }
}

/**
  * @brief  串口2发送命令
  * @param  cmd：命令
  *         res：需要检查的返回指令
  *         timeOut 返回时间
  *         resnum 命令发送次数
  * @retval 无
  * @note   0-成功  1-失败
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
            if(usart1_rev_finish)                                   // 如果串口接收完成
            {
                if(strstr(gprs_buf, res) != NULL)     // 如果检索到关键词
                {
                    printf("%s\r\n", gprs_buf);
                    gprs_clear();                                   // 清空缓存
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
            if(usart1_rev_finish)                                   // 如果串口接收完成
            {
                if(strstr(gprs_buf, res) != NULL)     // 如果检索到关键词
                {
                    k = get_gprs_data();
                    //printf("gprs_buf : %s\r\n", gprs_buf);
                    tmp = (u8 *)malloc(sizeof(u8) * 22);
                    for(j =0; j < 22; j++)
                    {
                        tmp[j] = gprs_buf[ k + j];
                    }
                    printf("tmp : %s\r\n", tmp);
                    gprs_clear();                                   // 清空缓存
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
  * @brief  GPRS清空缓存
  * @param  无
  * @retval 无
  * @note   无
  */
void gprs_clear(void)
{
    memset(gprs_buf, 0, sizeof(gprs_buf));
    gprs_cnt = 0;
}


