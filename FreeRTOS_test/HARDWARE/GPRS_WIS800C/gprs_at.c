
#include "gprs_at.h"
#include "usart.h"
#include "SysTick.h"
#include "gprs_data_unit.h"


extern EventGroupHandle_t flag_gprs;  //Gprs标志组

char gprs_buf[256];             // gprs 接收缓存
uint32_t gprs_cnt = 0;           // 接收计数
uint8_t usart1_rev_cnt = 0;


void Gprs_Init(void)
{
    usart1_init(115200);
    pen_gpio_config();      //配置核心板GPIO
    delay_ms(30);
    PEN_ON;
}


/**
  * @brief  gpio_config
  * @param  配置 GPIO 引脚
  * @retval 无
  * @note   PEN为GPRS核心板使能引脚，LED为单片机上的LED灯
  */
void pen_gpio_config(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);    // 使能GPIO_RCC
    
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_13;                // 选择要初始化的GPIOB引脚PB1
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;        // 设置引脚工作模式为通用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       // 设置引脚输出最大速率为50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    PEN_OFF;
}

/**
  * @brief  GPRS 初始化配置
  * @param  无
  * @retval 无
  * @note   无
  */
uint8_t gprs_init(void)
{
    PRINT("+++\r\n");                        // 测试AT命令通信是否正常
    vDelay_ms(500);                                      // 防止核心板还处于数据透传模式（测试专用，请忽略）
    gprs_send_string("+++");                             // 防止核心板还处于数据透传模式（测试专用，请忽略）
    vDelay_ms(500);                                      // 防止核心板还处于数据透传模式（测试专用，请忽略）

    PRINT("AT\r\n");                                   // 测试AT命令通信是否正常
    if(gprs_send_cmd("AT\r\n","OK",500,5))
    {
        return 1;
    }
    PRINT("ATE0\r\n");                                 // 取消核心板回显功能
    if(gprs_send_cmd("ATE0\r\n","OK",200,2))
    {
        return 2;
    }
    PRINT("AT+CPIN?\r\n");                              // 查询核心板是否读到卡
    if(gprs_send_cmd("AT+CPIN?\r\n","+CPIN: READY",200,2))
    {
        return 3;
    }
    PRINT("AT+CSQ\r\n");                                 //查询信号强度
    if(gprs_send_cmd("AT+CSQ\r\n","OK",500,5))
    {
        return 4;
    }
    PRINT("AT+CREG?\r\n");                              // 查询网络注册情况，核心板会自动注册网络，上电到注册大概 10s 左右
    if(gprs_send_cmd("AT+CREG?\r\n","0,1",500,5))
    {
        return 4;
    }
    PRINT("AT+CGATT?\r\n");                              // 附着网络
    if(gprs_send_cmd("AT+CGATT?\r\n","+CGATT: 1",500,1))  // 如果网络注册成功，但一直附着不上网络，则可以使用AT+CGATT=1
    {
        return 5;
    }
    PRINT("GPRS 初始化完成\r\n");
    xEventGroupSetBits(flag_gprs, Gprs_Init_Flag);        // 初始化完成，标志位置1
    return 0;
}

/**
  * @brief  GPRS 网络配置
  * @param  无
  * @retval 返回配置结果，如果出错，将由 err_process 函数处理
  * @note   无
  */
uint8_t gprs_net_config(void)
{
    PRINT("AT+CIPSHUT\r\n");                          // 关闭移动场景
    if(gprs_send_cmd("AT+CIPSHUT\r\n","OK",500,2))
    {
        return 10;
    }

    PRINT("AT+CIPMODE=1\r\n");                        // 设置为透传模式
    if(gprs_send_cmd("AT+CIPMODE=1\r\n","OK",200,2))
    {
        return 11;
    }
    PRINT("AT+CIPMUX=0\r\n");                         // 设置为单路链接，透传模式只能是单路链接
    if(gprs_send_cmd("AT+CIPMUX=0\r\n","OK",200,2))
    {
        return 12;
    }
    PRINT("AT+CSTT=\"CMNET\"\r\n");                   // 设置APN
    if(gprs_send_cmd("AT+CSTT=\"CMNET\"\r\n","OK",500,2))
    {
        return 13;
    }
    PRINT("AT+CIICR\r\n");                            // 激活移动场景
    if(gprs_send_cmd("AT+CIICR\r\n","OK",500,5))
    {
        return 14;
    }
    PRINT("AT+CIFSR\r\n");                            // 获取本地IP
    if(gprs_send_cmd("AT+CIFSR\r\n",".",300,1))    
    {
        return 15;
    }
    xEventGroupSetBits(flag_gprs, Gprs_Net_Flag);      // 配置完成，标志位置1
    return 0;
}

/**
  * @brief  连接 TCP 服务器
  * @param  无
  * @retval 返回配置结果，如果出错，将由 err_process 函数处理
  * @note   无
  */
uint8_t gprs_connect(void)                          // 建立TCP连接
{
    char *tcp_ip;            // 服务器IP和端口号
    //默认："47.106.99.158" "8888"
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
  * @brief  发送字符串
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
  * @brief  发送命令
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
            if((uBit & Gprs_RevFinish_Flag))                                   // 如果串口接收完成
            {
                if(strstr(gprs_buf, res) != NULL)     // 如果检索到关键词
                {
                    PRINT("%s\r\n", gprs_buf);
                    gprs_clear();                                   // 清空缓存
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

void uarst1_recv_irq(void)
{
    xEventGroupSetBits(flag_gprs, Gprs_Rev_Flag);            // 串口2 接收标志
    usart1_rev_cnt = 0;                                      // 串口2接收计数清零
    gprs_buf[gprs_cnt++] = USART1->DR;                       // 存入缓存数组
    if(gprs_cnt >= sizeof(gprs_buf))
    {
         gprs_cnt = 0;                                       // 防止数据量过大
    }
}


