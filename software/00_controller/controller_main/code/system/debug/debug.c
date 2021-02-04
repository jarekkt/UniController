/*! \file   debug.c

    \brief  Debugging support
    
*/


#include "system.h"     

uint32_t stackMark;

/*!
    \brief Debug mode support - assert function

    \param file     Error file name
    \param line     Error line
    
*/

void   debug_assert(char * file, unsigned int  line)
{
    volatile int stop = 1;
    while(stop)
    {
       __BKPT(0);
    };
}


uint32_t minStackMark = -1;


void debug_freertos_stack(void)
{
 volatile int stop = 1;
  uint32_t stackMark;

  stackMark = uxTaskGetStackHighWaterMark(NULL);

  if(stackMark < minStackMark)
  {
    minStackMark = stackMark;
  }

  if(stackMark < 64)
  {
    while(stop)
    {
       __BKPT(0);
    };
  }
}