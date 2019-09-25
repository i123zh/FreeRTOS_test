#ifndef _SysTick_H
#define _SysTick_H

#include "system.h"

extern volatile int gTimer;

void SysTick_Init(void);
void Tim2_Irq(void);
void delay_ms(u16 nms);
void delay_us(u32 nus);
void vDelay_ms(u16 nms);


#endif
