#ifndef _protocol_H
#define _protocol_H

#include "system.h"

#define sAddr1 (0x01)

#define WORK1 (0x01)
#define WORK2 (0x02)
#define WORK3 (0x03)
#define Err_Addr (0x10)
#define Err_Data (0x11)


u8 work1(u8 *buf, u8 len);


#endif
