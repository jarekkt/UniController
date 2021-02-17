/*! \file   cancomm_prv.h

    \brief  
    
    
*/

#ifndef CANCOMM_PRV_H
#define CANCOMM_PRV_H

#include "system.h"
#include "cancomm_mwr.h"



#define CANCOMM_TASK1_TICK   10    /* ms */

#define CANCOMM_TASK1_CAN_ID 0






typedef struct 
{
    uint32_t can_id;  
    uint8_t  can_dlc; 
    uint8_t  data[8];
}can_frame_t;




void mwr_cancomm_send(uint32_t cidx,can_frame_t * raw_frame,uint32_t wait4ready);
int  mwr_cancomm_receive(uint32_t cidx,can_frame_t * raw_frame);


#endif //CANCOMM_PRV_H
