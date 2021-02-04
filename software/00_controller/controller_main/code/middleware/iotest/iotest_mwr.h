/*!
    \file iotest_mwr.h
*/


#ifndef IOTEST_MWR_H
#define IOTEST_MWR_H

/*---------------------------------------------------------------------------
;
;      File: iotest_mwr.h
;
---------------------------------------------------------------------------*/

#include "system.h"
#include "common.h"
#include "iotest_io.h"



void mwr_iotest_init();
void mwr_iotest_once(void);
void mwr_iotest_set_mode(unit_mode_e mode);



#endif // IOTEST_MWR_H
