
#include "system.h"

#include "task_app.h"

/*******************************************************************************
* �� �� ��         : main
* ��������         : ������
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
int main()
{
    sys_init();
    
    PRINT("hello!\r\n");
    task_create();
    
    obj_create();
    
    vTaskStartScheduler();  //������ȿ�ʼ
    
    PRINT("��������������е����\r\n");
    PRINT("����ʱ���������������heap�ռ䲻�����ʧ�ܣ�\r\n");
    PRINT("��Ҫ�Ӵ�configTOTAL_HEAP_SIZE��ֵ��\r\n");
    while(1);
}



