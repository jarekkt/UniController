/*!
    \file burst_mwr.h


*/


#ifndef BURST_MWR_H
#define BURST_MWR_H


#include "system.h"
#include "burst_printf.h"
#include "burst_rcv.h"
#include "burst_mux.h"

void mwr_burst_init(void);
void mwr_burst_once(void);

int  burst_process_variable(const char * var_name,const char * var_value,char * resp_buffer,uint32_t resp_buffer_len,int32_t * execute_store);
int  burst_execute_var(char* command, int length);



#endif /* BURST_MWR_H */

