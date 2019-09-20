
#include "gprs_data_unit.h"

#include "SysTick.h"
#include "gprs_at.h"
#include "protocol.h"

uint8_t err_cnt = 0,err_num = 0;   // ERROR 计数，错误值

extern EventGroupHandle_t flag_gprs;  //Gprs标志组renwu
  
/**
  * @brief  错误处理函数
  * @param  错误值
  * @retval 无
  * @note   出错处理函数，当错误次数超过三次，则重启核心板。
  */
void err_process(uint8_t err)
{
    PRINT("开始进行错误处理\r\n");
    switch(err)
    {
        case 1:
            PRINT("AT ERROR\r\n");
            err_cnt = 4;               // 如果发送AT命令，核心板没有反应，则直接重启。
        break;
        case 2:
            PRINT("打开回显失败\r\n");
        break;
        case 3:
            PRINT("读卡失败，请检查SIM卡是否已插入\r\n");
        break;
        case 4:
            PRINT("网络注册失败\r\n");
        break;
        case 5:
            PRINT("网络附着失败\r\n");
        break;
        case 10:
            PRINT("关闭移动场景失败\r\n");
        break;
        case 11:
            PRINT("设置为非透传模式失败\r\n");	
        break;
        case 12:
            PRINT("设置为单链路模式失败\r\n");
        break;
        case 13:
            PRINT("设置APN失败\r\n");
        break;
        case 14:
            PRINT("激活移动场景失败\r\n");
        break;
        case 15:
            PRINT("未查询到本地IP\r\n");
        break;
        case 20:
            PRINT("服务器连接失败，将进行重连\r\n");
        break;
        case 21:
            PRINT("未收到服务器数据，可能与服务器断开了连接，将进行重连\r\n");
        break;
        default:
            PRINT("初始化完成\r\n");
    }
    // 标志位置0
    xEventGroupClearBits(flag_gprs, (Gprs_Init_Flag | Gprs_Net_Flag | Gprs_Connect_Flag | Gprs_Server_Flag));
    err_cnt++;
    if(err_cnt >= 3)                    // 出错大于等于三次
    {
        err_cnt = 0;
        err_num = 0;
        gprs_reset();                   // 重启核心板
    }
}

/**
  * @brief  重启模块
  * @param  无
  * @retval 无
  * @note   关机三秒后，重启核心板
  */
void gprs_reset(void)
{
    PRINT("重启模块\r\n");
    PEN_OFF;
    vDelay_ms(3000);
    PEN_ON;
}

/**
  * @brief  gprs接收处理
  * @param  无
  * @retval 无
  * @note   无
  */
void gprs_receive_process(void)
{
    if(((xEventGroupGetBits(flag_gprs)) & Gprs_RevFinish_Flag))
    {
        xEventGroupClearBits(flag_gprs, Gprs_RevFinish_Flag);
        PRINT("recv : %s\r\n", gprs_buf);
        
        gprs_clear();                              // 清除缓存
    }
}

/**
  * @brief  心跳处理
  * @param  无
  * @retval 无
  * @note   心跳包实际上指的是定时向服务器发送一组数据，以维持长连接。数据可以自己定义。
  */
uint8_t heard_beat_process(void)
{
    if(((xEventGroupGetBits(flag_gprs)) & Gprs_Heart_Flag))       // 心跳标志为1
    {
        xEventGroupClearBits(flag_gprs, Gprs_Heart_Flag);         // 清空心跳计数
        //send_heart();  //发送心跳报文
    }
    if(((xEventGroupGetBits(flag_gprs)) & Gprs_Server_Flag))
    {
        PRINT("与服务器断开连接\r\n");
        return 21;
    }
    return 0;
}

//定时器中断函数
void gprs_interrupt_process(void)
{
    static uint16_t tim_cnt1 = 0,tim_cnt2 = 0;           // 定时器计数
    static uint8_t ser_hb_cnt = 0,heart_beat_cnt = 0;    // 服务器心跳计数，心跳计数
    
    BaseType_t xResult;
    BaseType_t hp_task = pdFALSE;
    
    EventBits_t uBit;
    uBit = xEventGroupGetBits(flag_gprs);

    /* 串口1 接收处理 */
    if((uBit & Gprs_Rev_Flag))                                // 如果 usart1 接收数据标志为1
    {
        heart_beat_cnt = 0;                             // 当串口有数据过来时，心跳计数清零
        ser_hb_cnt = 0;
        tim_cnt1 = 0;
        tim_cnt2 = 0;
        usart1_rev_cnt++;                                // usart2 接收计数	
        if(usart1_rev_cnt >= 3)                          // 当超过 3 ms 未接收到数据，则认为数据接收完成。
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
    /* GPRS 核心板心跳处理 */
    if(!(uBit & Gprs_Rev_Flag) && (uBit & Gprs_Connect_Flag))         // 当GPRS核心板建立了连接且串口没有接收到数据
    {
        tim_cnt1++;                                    // 开始心跳计数
        if(tim_cnt1 >= 1000)
        { 
            tim_cnt1 = 0;
            heart_beat_cnt++;
            if(heart_beat_cnt >= 60)                  // 计数达到 30s 则心跳标志置 1，可以根据实际需求修改该值。
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

    /* 服务器心跳处理 */
//    if((uBit & Gprs_Connect_Flag))                               // GPRS 连接标志为 1
//    {
//        tim_cnt2++;                                       // tim计数
//        if(tim_cnt2 >= 1000)                              // 计时到 1s
//        {
//            tim_cnt2 = 0;                                   // tim 计数清零
//            ser_hb_cnt++;                                   // 服务器心跳计数
//            if(ser_hb_cnt >= 80)                            // 当 40s 都未接受到服务器数据，开启错误处理
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


//主要函数，连接服务器，根据服务器指令执行功能
void connect_server(void)
{
    EventBits_t uBit;
    uBit = xEventGroupGetBits(flag_gprs);
    
    if(!(uBit & Gprs_Init_Flag))                 // 如果核心板未初始化
    { 
        err_num = gprs_init();            // 核心板初始化，并将初始化结果保存在 err_num
    }
    else 
    {
        err_num = gprs_net_config();      // 核心板网络配置 
    }
        
    if((uBit & Gprs_Init_Flag) && (uBit & Gprs_Net_Flag)) // 如果核心板初始化和网络配置都完成
    {
        err_num = gprs_connect();         // 开始 TCP 连接
    }
    
    if((uBit & Gprs_Init_Flag) && (uBit & Gprs_Connect_Flag) && (uBit & Gprs_Net_Flag) && (!err_num))  //数据处理
    {
        vDelay_ms(100);
        //开始任务
        while(!err_num)                         // 服务器连接完成，通信处理
        {
            gprs_receive_process();            // 处理TCP发送过来的数据
            err_num = heard_beat_process();     //心跳
        }
    }
    
    if(((!(uBit & Gprs_Init_Flag)) || (!(uBit & Gprs_Net_Flag)) || (!(uBit & Gprs_Connect_Flag)) || (uBit & Gprs_Server_Flag)) && err_num)  // 配置未通过
    {
        err_process(err_num);                // AT命令错误处理
    }
}




