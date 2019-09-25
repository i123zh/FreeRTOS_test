#ifndef _TASK_APP_H
#define _TASK_APP_H

#include "system.h"

void sys_init(void);
void task_create(void);
void obj_create(void);

void LED0_Task(void * pv);
void LED1_Task(void * pv);
void Interrupt_Task(void * pv);

void Task_Suspend_Test(char r);

#endif
