/*!
    \file periodic_mwr.h

*/



#ifndef PERIODIC_MWR_H
#define PERIODIC_MWR_H


#define PERIODIC_OP_FIRST         0
#define PERIODIC_OP_LAST          100




#include "system.h"

/*! Low priority periodic background tasks */
#define MWR_PERIODIC_LOW_PR_TICK       10    /* ms */

/*! High priority periodic tasks, targeted for fast measurement processing */
#define MWR_PERIODIC_HIGH_PR_TICK       5    /* ms */


void mwr_periodic_init(void);
void mwr_periodic_once(void);
void mwr_periodic_low_register(void (*periodic_fn)(void), uint32_t period);
void mwr_periodic_high_register(void (*periodic_fn)(void), uint32_t period);




#endif /* TSK_BACKGROUND_H */

