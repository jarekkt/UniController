/*! 
    \file   timer_pulse.h

    \brief  File with support for timer pulse generation
    
    
*/


#ifndef TIMER_PULSE_H
#define TIMER_PULSE_H


#include "system.h"

extern TIM_HandleTypeDef htim8;
extern TIM_HandleTypeDef htim12;
extern TIM_HandleTypeDef htim17;
extern TIM_HandleTypeDef htim16;

void srv_timer_pulse_init(void);
void srv_timer_pulse_once(void);
void srv_timer_pulse_period_for_hz(uint32_t step_freq);

#define TMR_TIRGGER_X()	__HAL_TIM_ENABLE(&htim8);
#define TMR_TIRGGER_Y()	__HAL_TIM_ENABLE(&htim12);
#define TMR_TIRGGER_Z()	__HAL_TIM_ENABLE(&htim17);







#endif //TIMER_PULSE
