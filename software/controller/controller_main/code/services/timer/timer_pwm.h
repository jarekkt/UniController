/*! 
    \file   timer_pwm.h

    \brief  File with support for timer pwm's
    
    
*/


#ifndef TIMER_PWM_H
#define TIMER_PWM_H

#include "system.h"


#define TMR_PWM_CH_OUT5			0
#define TMR_PWM_CH_OUT6			1
#define TMR_PWM_CH_SERVO1		2
#define TMR_PWM_CH_SERVO2		3




void srv_timer_pwm_init(void);
void srv_timer_pwm_once(void);
void srv_timer_pwm(uint32_t ch, uint32_t duty);






#endif //TIMER_PWM
