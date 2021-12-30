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



typedef struct
{
    uint32_t  can_id;
    uint32_t  can_dlc;
    uint8_t  data[64];
}can_frame_t;

void mwr_cancomm_init();
void mwr_cancomm_once(void);
void mwr_cancomm_test(void);

int mwr_cancomm_receive(can_frame_t * raw_frame);
void mwr_cancomm_send(can_frame_t * raw_frame,uint32_t wait4ready);







#endif // CANCOMM_MWR_H
