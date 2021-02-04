/*!
    \file cancomm_mwr.h
*/


#ifndef CANCOMM_MWR_H
#define CANCOMM_MWR_H

/*---------------------------------------------------------------------------
;
;      File: cancomm_mwr.h
;
---------------------------------------------------------------------------*/

#include "system.h"
#include "common.h"
#include "cancomm_task_can1.h"




void mwr_cancomm_init();
void mwr_cancomm_once(void);
void mwr_cancomm_test(uint32_t ch);








#endif // CANCOMM_MWR_H
