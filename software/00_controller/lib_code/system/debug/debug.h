/*! \file   debug.h

    \brief  Debugging macros and functions
      
*/


#ifndef DEBUG_H
#define DEBUG_H 



void   debug_assert(char * file, unsigned int  line);
void   debug_freertos_stack(void);

#ifdef  DEBUG
  #define fw_assert(expr)       ((expr) ? (void)0 : debug_assert((char *)__FILE__, __LINE__))
  #define fw_stack_check()      debug_freertos_stack();

#else
  #define fw_assert(expr)       ((void)0)
  #define fw_stack_check()      ((void)0)
#endif 


#endif