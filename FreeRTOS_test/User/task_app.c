
#include "task_app.h"
#include "usart.h"
#include "SysTick.h"
#include "timer.h"

#include "led.h"

//������
TaskHandle_t LED0_Task_handle, LED1_Task_handle;

//FreeRTOS��ʱ����
static xTimerHandle tim_led[2];

static void TimObjCreate(void);
static void vTimerCallback(xTimerHandle pxTimer);

//LED��־��
static EventGroupHandle_t flag_led = NULL;

static void FlagObjCreate(void);



void sys_init()
{
    NVIC_Configuration();
    SysTick_Init();
    usart2_init(9600);
    LED_Init();             //LED�˿ڳ�ʼ��
}

void task_create()
{
    BaseType_t res;
//    LED0_Task_handle = (TaskHandle_t *)pvPortMalloc(sizeof(TaskHandle_t));
//    LED1_Task_handle = (TaskHandle_t *)pvPortMalloc(sizeof(TaskHandle_t));
    
    res = xTaskCreate(LED0_Task,(const char *)"LED0",configMINIMAL_STACK_SIZE,NULL,2,&LED0_Task_handle);
    if(res != pdTRUE)
    {
        PRINT("LED0_Task����ʧ�ܣ�\r\n");
    }
    
    res = xTaskCreate(LED1_Task,(const char *)"LED1",configMINIMAL_STACK_SIZE,NULL,3,&LED1_Task_handle);
    if(res != pdTRUE)
    {
        PRINT("LED1_Task����ʧ�ܣ�\r\n");
    }
}

void obj_create()
{
    TimObjCreate();  //������ʱ����
    FlagObjCreate();  //�����¼���־��
}


void LED0_Task(void * pv)
{
    EventBits_t uBit;
    while(1)
    {
        uBit = xEventGroupWaitBits(flag_led, LED0, pdTRUE, pdTRUE, portMAX_DELAY);
        if(uBit == LED0)
        {
            led0 = !led0;
        }
        else
        {
            PRINT("�ȴ���ʱ��\r\n");
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
             if(xTimerStart(tim_led[i], 100) != pdPASS)     //100��ʱ������ʱ��
             {
                 PRINT("TimerStart failed!\r\n");
             }
        }
    }
}
//�ص�����
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
}


