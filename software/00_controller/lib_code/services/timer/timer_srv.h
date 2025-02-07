/*! 
    \file   timer_srv.h

    \brief  File with support for timers
    
    
*/


#ifndef TIMER_SRV_H
#define TIMER_SRV_H

#define TIMER_SRV_SLOW_HZ    1000    /* Hz */
#define TIMER_SRV_FAST_HZ    10000   /* Hz */
#define TIMER_SRV_1MHZ       1000000 /* Hz */
#define TIMER_SRV_10MHZ      10000000 /* Hz */

#include "timer_pulse.h"
#include "timer_pwm.h"
#include "timer_quad.h"




#include "system.h"


typedef         uint16_t srv_hwio_timestamp_t;
typedef void  (*timer_callback_fn)(void);

void 				 srv_timer_init(void);
void 				 srv_timer_once(void);


void 				 srv_udelay(uint32_t cnt);
// For slow callbacks, no FreeRTOS support
void 				 srv_timer_callback_slow_add(timer_callback_fn fn);
// For fast callbacks, no FreeRTOS support
void 				 srv_timer_callback_fast_add(timer_callback_fn fn);
// For step generation
void 				 srv_timer_callback_step(uint32_t freq,timer_callback_fn fn);
void 				 srv_timer_callback_step_disable(void);


void     			 srv_hwio_delay_ms(uint32_t ms);
void     			 srv_hwio_delay_us(uint32_t us);


uint32_t             srv_hwio_timestamp_ms_tick(void);
srv_hwio_timestamp_t srv_hwio_timestamp_ms(void);
srv_hwio_timestamp_t srv_hwio_timestamp_us(void);
uint32_t 			 srv_hwio_timestamp_diff( srv_hwio_timestamp_t t_new,srv_hwio_timestamp_t t_old);





#endif //TIMER_SRV s
