/*! 
    \file   timer_quad.h

    \brief  File with support for timer encoder
    
    
*/


#ifndef TIMER_QUAD_H
#define TIMER_QUAD_H


#include "system.h"



void srv_timer_quad_init(void);
void srv_timer_quad_once(void);

uint32_t srv_timer_quad_get(uint32_t ch);


#endif //TIMER_QUAD
