/*!
    \file iotest_io.h
*/


#ifndef IOTEST_IO_H
#define IOTEST_IO_H

/*---------------------------------------------------------------------------
;
;      File: iotest_io.h
;
---------------------------------------------------------------------------*/

#include "system.h"
#include "common.h"


void mwr_iotest_io_init();
void mwr_iotest_io_control(uint32_t io_request);
void mwr_iotest_io_task();




#endif // IOTEST_IO_H
