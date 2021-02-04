#include "common.h"
#include "services.h"
#include "middleware.h"
#include "cancomm_prv.h"



typedef struct
{
    uint32_t                 tick;


}mwr_cancomm1_type;




typedef struct
{
    uint32_t dummy;
}mwr_cancomm1_nv_type;


mwr_cancomm1_nv_type   ccm1_nv VAR_NV_ATTR;
mwr_cancomm1_type      ccm1; 

const var_ptable_t   cancomm1_mwr_var_ptable[] SERMON_ATTR =  
{
  { "can_dummy",    &ccm1_nv.dummy   ,E_VA_UINT_FREE    },

};


void mwr_cancomm1_default_init(void)
{

}


void mwr_cancomm_init_can1_task(void)
{

    srv_sermon_register(cancomm1_mwr_var_ptable,sizeof(cancomm1_mwr_var_ptable));
    srv_nov_register(&ccm1_nv, sizeof(ccm1_nv),mwr_cancomm1_default_init);


}

void mwr_cancomm_callback_can1(void)
{
 
}



void mwr_cancomm_execute_can1_app(void)
{
/*

    while(mwr_cancomm_receive(CANCOMM_TASK1_CAN_ID,&frame) == 0)
    {


    }
*/

}


void mwr_cancomm_execute_can1_task(void)
{
    TickType_t          xLastWakeTime;
    const  TickType_t   xFrequency = CANCOMM_TASK1_TICK / portTICK_PERIOD_MS;
   
    xLastWakeTime = xTaskGetTickCount();
    while(1)
    {
      vTaskDelayUntil( &xLastWakeTime, xFrequency);
      mwr_cancomm_execute_can1_app();
    }
}






