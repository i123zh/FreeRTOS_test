
#include "task_app.h"
#include "usart.h"
#include "SysTick.h"
#include "timer.h"

#include "led.h"

//任务句柄
TaskHandle_t LED0_Task_handle, LED1_Task_handle;
TaskHandle_t Interrupt_Task_handle;

//FreeRTOS定时器组
static xTimerHandle tim_led[2];

static void TimObjCreate(void);
static void vTimerCallback(xTimerHandle pxTimer);   //定时器回调函数

//LED标志组
static EventGroupHandle_t flag_led = NULL;

static void FlagObjCreate(void);


/* 硬件初始化函数 */
void sys_init()
{
    NVIC_Configuration();   //中断优先级分组配置
    SysTick_Init();         //用TIM2实现微妙级和毫秒级硬件延时函数
    usart2_init(9600);
    LED_Init();             //LED端口初始化
    
    TIM3_Init(9999,7200);
}

/* 任务创建函数 */
void task_create()
{
    BaseType_t res;
    
    res = xTaskCreate(LED0_Task,(const char *)"LED0",configMINIMAL_STACK_SIZE,NULL,2,&LED0_Task_handle);
    if(res != pdTRUE)
    {
        PRINT("LED0_Task创建失败！\r\n");
    }
    
    res = xTaskCreate(LED1_Task,(const char *)"LED1",configMINIMAL_STACK_SIZE,NULL,8,&LED1_Task_handle);
    if(res != pdTRUE)
    {
        PRINT("LED1_Task创建失败！\r\n");
    }
    
    res = xTaskCreate(Interrupt_Task,(const char *)"Interrupt",configMINIMAL_STACK_SIZE,NULL,1,&Interrupt_Task_handle);
    if(res != pdTRUE)
    {
        PRINT("LED1_Task创建失败！\r\n");
    }
}

/* 全局对象创建 */
void obj_create()
{
    TimObjCreate();  //创建定时器组
    FlagObjCreate();  //创建事件标志组
}

/* 任务函数 */
void LED0_Task(void * pv)       //标志组控制LED测试任务
{
    EventBits_t uBit;
    while(1)
    {
        uBit = xEventGroupWaitBits(flag_led, LED0, pdTRUE, pdTRUE, portMAX_DELAY);  //等待标志位被设置
        if(uBit == LED0)
        {
            led0 = !led0;
        }
        else
        {
            PRINT("等待超时！\r\n");
        }
    }
}

void LED1_Task(void * pv)       //LED1闪烁任务
{
    while(1)
    {
        led1=!led1;
        vTaskDelay(1000);
    }
}

void Interrupt_Task(void * pv)      //调度锁和临界段测试任务
{
    while(1)
    {
        vDelay_ms(5000);
//        vTaskSuspendAll();          //调度锁开启，禁止任务切换
        taskENTER_CRITICAL();       //临界段开始，屏蔽优先级大于1的中断
        
        PRINT("task!!\r\n");
        delay_ms(5000);
        PRINT("task!!\r\n");
        
        taskEXIT_CRITICAL();        //临界段结束
//        if(!xTaskResumeAll())       //调度锁关闭
//        {
//            taskYIELD();            //任务上下文切换
//        }
    }
}


/* 定时器创建函数 */
static void TimObjCreate(void)
{
    uint32_t i;
    const TickType_t  xTimerPer[2] = {100, 1000};
    
    for(i = 0; i < 2; i++)
    {
        tim_led[i] = xTimerCreate("Timer", xTimerPer[i], pdTRUE, (void *) i, vTimerCallback);
        if(tim_led[i] == NULL)
        {
            PRINT("TimerCreate failed!\r\n");
        }
        else
        {
             if(xTimerStart(tim_led[i], 100) != pdPASS)     //100延时后开启定时器
             {
                 PRINT("TimerStart failed!\r\n");
             }
        }
    }
}
//回调函数
static void vTimerCallback(xTimerHandle pxTimer)
{
    uint32_t ulTimerID;
    
    BaseType_t xResult;
    BaseType_t hpri_task = pdFALSE;

    configASSERT(pxTimer);
    ulTimerID = (uint32_t)pvTimerGetTimerID(pxTimer);
    
    if(ulTimerID == 0)
    {
        //led2 = !led2;
    }
    
    if(ulTimerID == 1)
    {
        xResult = xEventGroupSetBitsFromISR(flag_led, LED0, &hpri_task);    //设置标志位
        if(xResult != pdFAIL)
        {
            portYIELD_FROM_ISR(hpri_task);
        }
    }
}

/* 标志组创建函数 */
static void FlagObjCreate(void)
{
    flag_led = xEventGroupCreate();
    if(flag_led == NULL)
    {
        PRINT("EventGroupCreate failed!\r\n");
    }
}

//中断控制任务挂起和回复测试
void Task_Suspend_Test(char r)
{
    BaseType_t res;
    if(r == '1')
    {
        res = xTaskResumeFromISR(LED0_Task_handle);
        if(res == pdTRUE)
        {
            portYIELD_FROM_ISR(res);
        }
    }
    if(r == '2')
    {
        vTaskSuspend(LED0_Task_handle);
    }
}

