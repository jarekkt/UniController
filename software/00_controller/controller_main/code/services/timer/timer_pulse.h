/*! 
    \file   timer_pulse.h

    \brief  File with support for timer pulse generation
    
    
*/


#ifndef TIMER_PULSE_H
#define TIMER_PULSE_H


#include "system.h"

extern TIM_HandleTypeDef htim8;
extern TIM_HandleTypeDef htim12;
extern TIM_HandleTypeDef htim15;


void srv_timer_pulse_init(void);
void srv_timer_pulse_once(void);
void srv_timer_pulse_period(uint32_t period_01us);

#define TMR_TIRGGER_X()	htim12.Instance->EGR = TIM_EGR_UG
#define TMR_TIRGGER_Y()	htim8.Instance->EGR = TIM_EGR_UG
#define TMR_TIRGGER_Z()	htim15.Instance->EGR = TIM_EGR_UG


#endif //TIMER_PULSE
