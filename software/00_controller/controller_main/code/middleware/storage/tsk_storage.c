/*!
    \file tsk_storage.c

    \brief This file process handles non volatile memory storage


*/


#include "system.h"
#include "services.h"
#include "middleware.h"


#define STORAGE_INTERVAL      (7/*minutes*/ *60 /*sec*/ * 1000/* ms*/)
#define STORAGE_DUMP_CHANGE   0


typedef struct
{

   xSemaphoreHandle    wait_sema;
   xSemaphoreHandle    mutex_sema;  
    

   int                 run_cntr;



}storage_task_ctx_t;


void tsk_storage_task(void * params);


storage_task_ctx_t    sctx;


/*!
    \brief This function activates storage ( in non blocking way).
           All changed data will be updated in NOV in the backgoround.

*/
void tsk_storage_activate(void)
{
    xSemaphoreGive(sctx.wait_sema);    
}






/*!
    \brief Storage task intialization function

*/
void tsk_storage_init(void)
{
    memset(&sctx,0,sizeof(sctx));
      
}

void tsk_storage_once(void)
{
  vSemaphoreCreateBinary(sctx.wait_sema);        
  sctx.mutex_sema = xSemaphoreCreateMutex();

  xTaskCreate( tsk_storage_task, (char * ) "Store", 8* configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL );
}



/*!
    \brief This function activates storage ( in blocking way).
           It will not return until all data is stored.

*/
void tsk_storage_execute(void)
{
   int result;


   
   xSemaphoreTake(sctx.mutex_sema,portMAX_DELAY);
   
   if( srv_nov_is_changed()!= 0)
   {
#if STORAGE_DUMP_CHANGE != 0
       srv_nov_dump_change();
#endif
     
       result = srv_nov_store();


       if(result != 0)
       {
         burst_log_printf(1,"nov: storage error!\r\n");
       }

       sctx.run_cntr++;
    }


    xSemaphoreGive(sctx.mutex_sema);
}


/*!
    \brief Storage task. Peridically stores data to NOV ( with #STORAGE_INTERVAL interval) or by explicit request.

*/

void tsk_storage_task(void * params)
{


    // Forced storage check during startup - to be sure that we do not loose anything 
    // which might have been changed during init phase ( e.g. supervison or test results)


    /*!
        \sto  Storage activation once when staring storage task - to store all changes from startup phase
    */

    
    srv_nov_start();

    tsk_storage_activate();

    while(1)
    {

        xSemaphoreTake(sctx.wait_sema, STORAGE_INTERVAL / portTICK_RATE_MS);

        // Ignore result - either it was activation or timeout
        tsk_storage_execute();
        
        fw_stack_check();
    }
}



