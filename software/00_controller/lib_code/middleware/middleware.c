#include "middleware.h"



void mwr_init(void)
{
   // Logging & database communication
   mwr_burst_init();

   // Periodic task server
   mwr_periodic_init();

   // Storage init
   tsk_storage_init();


   // Iotest
   mwr_iotest_init();



}

void mwr_once(void)
{
  mwr_burst_once();
  mwr_periodic_once();
  tsk_storage_once();
  mwr_iotest_once();

}
