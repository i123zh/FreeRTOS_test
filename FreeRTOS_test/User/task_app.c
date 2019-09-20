
#include "task_app.h"
#include "usart.h"
#include "SysTick.h"
#include "timer.h"

#include "led.h"
#include "gprs_at.h"
#include "gprs_data_unit.h"


//FreeRTOS定时器组
static xTimerHandle tim_led[2];

static void TimObjCreate(void);
static void vTimerCallback(xTimerHandle pxTimer);

//LED标志组
static EventGroupHandle_t flag_led = NULL;

EventGroupHandle_t flag_gprs = NULL;  //Gprs标志组

static void FlagObjCreate(void);



void sys_init()
{
    NVIC_Configuration();
    SysTick_Init();
    usart2_init(115200);
    LED_Init();             //LED端口初始化
    
    Gprs_Init();
}

void task_create()
{
    BaseType_t res;
    TaskHandle_t *LED0_Task_handle, *LED1_Task_handle, *GPRS_Task_handle;
    LED0_Task_handle = (TaskHandle_t *)pvPortMalloc(sizeof(TaskHandle_t));
    LED1_Task_handle = (TaskHandle_t *)pvPortMalloc(sizeof(TaskHandle_t));
    GPRS_Task_handle = (TaskHandle_t *)pvPortMalloc(sizeof(TaskHandle_t));
    
    res = xTaskCreate(LED0_Task,(const char *)"LED0",configMINIMAL_STACK_SIZE,NULL,2,LED0_Task_handle);
    if(res != pdTRUE)
    {
        PRINT("LED0_Task创建失败！\r\n");
    }
    
    res = xTaskCreate(LED1_Task,(const char *)"LED1",configMINIMAL_STACK_SIZE,NULL,3,LED1_Task_handle);
    if(res != pdTRUE)
    {
        PRINT("LED1_Task创建失败！\r\n");
    }
    
    res = xTaskCreate(GPRS_Task,(const char *)"GPRS",((configMINIMAL_STACK_SIZE) * 2),NULL,2,GPRS_Task_handle);
    if(res != pdTRUE)
    {
        PRINT("GPRS_Task创建失败！\r\n");
    }
}

void obj_create()
{
    TimObjCreate();  //创建定时器组
    FlagObjCreate();  //创建事件标志组
}


void LED0_Task(void * pv)
{
    EventBits_t uBit;
    while(1)
    {
        uBit = xEventGroupWaitBits(flag_led, LED0, pdTRUE, pdTRUE, portMAX_DELAY);
        //PRINT("uBit : %x\r\n", uBit);
        if(uBit == LED0)
        {
            led0 = !led0;
        }
        else
        {
            PRINT("等待超时！\r\n");
        }
        //vTaskDelay(3000/portTICK_RATE_MS);
        //delay_ms(1000);
        //PRINT("task_dep : %d", (int)uxTaskGetStackHighWaterMark( NULL ));
    }
}

void LED1_Task(void * pv)
{
    while(1)
    {
        led1=!led1;
        vTaskDelay(1000);
    }
}

void GPRS_Task(void *pv)
{
    while(1)
    {
        connect_server();
        vDelay_ms(100);
        PRINT("task_dep : %d", (int)uxTaskGetStackHighWaterMark( NULL ));
    }
}

static void TimObjCreate(void)
{
    uint32_t i;
    const TickType_t  xTimerPer[2] = {1, 1000};
    
    for(i = 0; i < 2; i++)
    {
        tim_led[i] = xTimerCreate("Timer", xTimerPer[i], pdTRUE, (void *) i, vTimerCallback);
        if(tim_led[i] == NULL)
        {
            PRINT("TimerCreate failed!\r\n");
        }
        else
        {
             if(xTimerStart(tim_led[i], 100) != pdPASS)
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
        gprs_interrupt_process();
    }
    
    if(ulTimerID == 1)
    {
        xResult = xEventGroupSetBitsFromISR(flag_led, LED0, &hpri_task);
        if(xResult != pdFAIL)
        {
            portYIELD_FROM_ISR(hpri_task);
        }
    }
}


static void FlagObjCreate(void)
{
    flag_led = xEventGroupCreate();
    if(flag_led == NULL)
    {
        PRINT("EventGroupCreate failed!\r\n");
    }
    
    flag_gprs = xEventGroupCreate();
    if(flag_gprs == NULL)
    {
        PRINT("EventGroupCreate failed!\r\n");
    }
}


