
#include "protocol.h"
#include "led.h"


u8 work1(u8 *buf, u8 len)
{
    if(buf[2] == 0x01)
    {
        led1 = 1;
    }
    else
    {
        led1 = 0;
    }
    return len;
}



