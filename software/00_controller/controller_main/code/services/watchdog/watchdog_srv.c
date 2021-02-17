#include "services.h"
#include "watchdog_srv.h"




#define LSI_FREQ              32000 /* 32kHz average Actually  17.. 47 kHz */
#define DEBUG_SLEEP_ENABLED   1


#if SRV_WDG_ENABLED != 0
IWDG_HandleTypeDef IwdgHandle;
#endif
/*!
    \brief   Watchdog timeout function

*/ 

void srv_wdg_timeout(uint32_t timeout_ms)
{
    
#if SRV_WDG_ENABLED != 0

  /* Set counter reload value to obtain 250ms IWDG TimeOut.
     IWDG counter clock Frequency = LsiFreq / 32
     Counter Reload Value = 250ms / IWDG counter clock period
                          = 0.25s / (32/LsiFreq)
                          = LsiFreq / (32 * 4)
                          = LsiFreq / 128 */


  IwdgHandle.Instance       = IWDG;
  IwdgHandle.Init.Prescaler = IWDG_PRESCALER_32;
  IwdgHandle.Init.Reload    = (timeout_ms * (LSI_FREQ/32)) / 1000  ;


   HAL_IWDG_Init(&IwdgHandle);
#endif

}


/*!
    \brief   Watchdog init function

*/ 
void srv_wdg_init(void)
{
#if SRV_WDG_ENABLED != 0

    /* Stop watchdog counters when debugging */
    DBGMCU->APB1FZ |= DBGMCU_APB1_FZ_DBG_IWDG_STOP; 
    DBGMCU->APB1FZ |= DBGMCU_APB1_FZ_DBG_WWDG_STOP; 

#if DEBUG_SLEEP_ENABLED == 1

    /* Enable low power debug */
    DBGMCU->CR |= DBGMCU_CR_DBG_SLEEP;
    DBGMCU->CR |= DBGMCU_CR_DBG_STOP;
    DBGMCU->CR |= DBGMCU_CR_DBG_STANDBY;
    
#endif    

    srv_wdg_timeout(1000);
#endif

}


void srv_wdg_once(void)
{

}


/*!
    \brief   Watchdog  kick function

*/
void srv_wdg_kick(void)
{
#if SRV_WDG_ENABLED != 0
    /* Reload IWDG counter */
    HAL_IWDG_Refresh(&IwdgHandle);
#endif

}

/*!
    \brief   Power control function for sleep mode
*/
void srv_wdg_power_down(uint32_t timeout)
{
#if SLEEP_ENABLED  == 1

    // TODO
#endif

}

