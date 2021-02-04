/*!
    \file burst_mux.h


*/


#ifndef BURST_MUX_H
#define BURST_MUX_H


#include "system.h"
#include "burst_rcv.h"

void burst_mux_init();
void burst_mux_once();

int32_t burst_mux_serial_process(uint32_t idx,char * buffer,uint32_t len);

#endif //BURST_MUX_H
