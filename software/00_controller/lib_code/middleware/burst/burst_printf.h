/*!
    \file burst_printf.h


*/


#ifndef BURST_PRINTF_H
#define BURST_PRINTF_H


#include "system.h"



#define LVL_TRACE	0x01
#define LVL_DEBUG	0x02
#define LVL_INFO	0x04
#define LVL_WARN	0x08
#define LVL_ERROR	0x10

#define printd(level_mask,format,...)  burst_log_printf(level_mask,format, __VA_ARGS__)
#define printds(level_mask,string)     burst_log_printf(level_mask,string)




void burst_printf_init(void);
void burst_printf_once(void);


void burst_log_printf(uint32_t level_mask,const char  * format, ...);




#endif /* BURST_PRINTF_H */

