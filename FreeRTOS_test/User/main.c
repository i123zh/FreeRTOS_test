
#include "system.h"

#include "task_app.h"

/*******************************************************************************
* 函 数 名         : main
* 函数功能         : 主函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
int main()
{
    sys_init();
    
    PRINT("hello!\r\n");
    task_create();
    
    obj_create();
    
    vTaskStartScheduler();  //任务调度开始
    
    PRINT("正常情况不会运行到这里，\r\n");
    PRINT("若定时器任务或空闲任务的heap空间不足可能失败，\r\n");
    PRINT("需要加大configTOTAL_HEAP_SIZE的值。\r\n");
    while(1);
}



