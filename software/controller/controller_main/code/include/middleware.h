#ifndef MIDDLEWARE_H
#define MIDDLEWARE_H


#include "system.h"     

#include "middleware/storage/tsk_storage.h"
#include "middleware/burst/burst_mwr.h"
#include "middleware/periodic/periodic_mwr.h"
#include "middleware/iotest/iotest_mwr.h"
#include "middleware/cancomm/cancomm_mwr.h"



void mwr_init(void);
void mwr_once(void);


#endif //MIDDLEWARE_H
