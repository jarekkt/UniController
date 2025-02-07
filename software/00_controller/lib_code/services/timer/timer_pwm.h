/*! 
    \file   timer_pwm.h

    \brief  File with support for timer pwm's
    
    
*/


#ifndef TIMER_PWM_H
#define TIMER_PWM_H

#include "system.h"



void srv_timer_pwm_init(void);
void srv_timer_pwm_once(void);
void srv_timer_pwm_refresh(void);
void srv_timer_pwm(uint32_t ch, uint32_t duty_percent);






#endif //TIMER_PWM
