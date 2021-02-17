/*!
    \file burst_printf.h


*/


#ifndef BURST_PRINTF_H
#define BURST_PRINTF_H


#include "system.h"


void burst_printf_init(void);
void burst_printf_once(void);


void burst_log_printf(uint32_t level_mask,const char  * format, ...);




#endif /* BURST_PRINTF_H */

