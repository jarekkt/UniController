/*!
    \file periodic_mwr.c

    \brief This file contains scheduler for  all non-real time background activities
*/



#include "periodic_mwr.h"

#define MWR_PERIODIC_CNT 10


typedef void (*periodic_fn_t)(void);



typedef struct
{
    periodic_fn_t   function;
    uint32_t        period;
    uint32_t        timer;
}periodic_task_t;


typedef struct
{
    periodic_task_t  task[MWR_PERIODIC_CNT];
    int               cnt;
}periodic_data_low_t;


typedef struct
{    
    periodic_task_t task[MWR_PERIODIC_CNT];
    int              cnt;
}periodic_data_high_t;



periodic_data_low_t       pldata;
periodic_data_high_t      phdata;

static void mwr_low_task(void * params);
static void mwr_high_task(void * params);


/*!
        \brief Background task init function.
             
*/
void mwr_periodic_init(void)
{
    memset(&pldata,0,sizeof(pldata));
    memset(&phdata,0,sizeof(phdata));
}

void mwr_periodic_once(void)
{
    xTaskCreate( mwr_low_task,        "PerLow",  8*configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY   + 2, NULL );
    xTaskCreate( mwr_high_task,       "PerHigh", 8*configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY   + 5, NULL );


}


static void mwr_periodic_register(void (*periodic_fn)(void), uint32_t period,periodic_task_t * tasks, int *cnt,int max_cnt,uint32_t tmr_period)
{
    if((*cnt < max_cnt) & (period > 0))
    {
      tasks[*cnt].function = periodic_fn;
      // Convert to ticks ( round up)
      tasks[*cnt].period   = (period  + tmr_period - 1)/(tmr_period);

      // We actually need to count one less
      if((tasks[*cnt]).period > 0)
      {
        tasks[*cnt].period--;
      }

      tasks[*cnt].timer    = tasks[*cnt].period;
      (*cnt)++;

    }
    else
    {
      fw_assert(0);
    }
}



void mwr_periodic_low_register(void (*periodic_fn)(void), uint32_t period)
{
  mwr_periodic_register(periodic_fn,period,pldata.task,&pldata.cnt,DIM(pldata.task),MWR_PERIODIC_LOW_PR_TICK);
  
}

void mwr_periodic_high_register(void (*periodic_fn)(void), uint32_t period)
{
  mwr_periodic_register(periodic_fn,period,phdata.task,&phdata.cnt,DIM(phdata.task),MWR_PERIODIC_HIGH_PR_TICK);
  
}


static void mwr_task(periodic_task_t * tasks, int cnt,uint32_t freq)
{
    TickType_t          xLastWakeTime;
    const  TickType_t   xFrequency = freq;
    int                 ii;

    xLastWakeTime = xTaskGetTickCount();
    
    while(1)
    {
       vTaskDelayUntil( &xLastWakeTime, xFrequency );

       for(ii = 0; ii < cnt;ii++)
       {
          if(tasks[ii].timer > 0)
          {
            tasks[ii].timer--;
          }
          else
          {
            tasks[ii].timer = tasks[ii].period;
            tasks[ii].function();
          }
       }

       fw_stack_check();
    }
}


/*!
        \brief Periodic low priority task function. Executes all threads with predefined timing.
             
*/
static void mwr_low_task(void * params)
{
    mwr_task(pldata.task,pldata.cnt,MWR_PERIODIC_LOW_PR_TICK / portTICK_PERIOD_MS);;
}


/*!
        \brief Periodic high priority task function. Executes all threads with predefined timing.
             
*/
static void mwr_high_task(void * params)
{
    mwr_task(phdata.task,phdata.cnt,MWR_PERIODIC_HIGH_PR_TICK / portTICK_PERIOD_MS);
}



