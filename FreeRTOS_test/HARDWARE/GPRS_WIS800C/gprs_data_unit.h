#ifndef __DATA_UNIT_H_
#define __DATA_UNIT_H_

#include "system.h"

extern uint8_t err_cnt,err_num; 

void err_process(uint8_t err);
void gprs_reset(void);
void gprs_receive_process(void);
uint8_t heard_beat_process(void);
void connect_server(void);
void gprs_interrupt_process(void);

#endif


