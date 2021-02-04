 /*! \file   srv_sermon.c

    \brief  Configures variables to be sent in burst mode
      
*/

#include "system.h"
#include "services.h"


volatile const var_ptable_t  * ptr;



void srv_sermon_init(void)
{

}

void srv_sermon_once(void)
{

}

void srv_sermon_register(const var_ptable_t  table[],int cnt)
{
   // otherwise optimized
   ptr = table;
}






