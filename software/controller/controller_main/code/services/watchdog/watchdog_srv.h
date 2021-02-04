/*! 
    \file   watchdog_srv.h

    \brief  File with support for watchdog
    
    
*/


#ifndef SRV_WATCHDOG_H
#define SRV_WATCHDOG_H

#include "system.h"


#define SRV_WDG_SLEEP_TIME          50      /*!< Time the device sleeps before waking up. */
#define SRV_WDG_ENABLED             0



void srv_wdg_init(void);
void srv_wdg_once(void);
void srv_wdg_kick(void);
void srv_wdg_power_down(uint32_t timeout);
void srv_wdg_timeout(uint32_t timeout_ms);




#endif //SRV_WATCHDOG_H
