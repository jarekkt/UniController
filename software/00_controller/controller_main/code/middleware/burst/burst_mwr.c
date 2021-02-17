/*!
    \file mwr_burst.c

    \brief This file process 'burst mode' for sending device info to pc (for debugging). It also responds to single commands from PC.
*/


#include "system.h"
#include "services.h"
#include "middleware.h"

#include "version.h"
#include "burst_mux.h"



#define var_table          ((var_ptable_t*)__sermon_start__)
#define var_table_size     ((uint32_t)(__sermon_end__ - __sermon_start__))





/*!
    \brief  Init function for burst module                              
                                    
*/

void mwr_burst_init(void)
{
	burst_printf_init();
	burst_mux_init();
	burst_rcv_init();

}

/*!
    \brief  Start function executed after all blocks are initialized
                                    
*/

void mwr_burst_once(void)
{
   burst_printf_once();
   burst_mux_once();
   burst_rcv_once();

}


int burst_process_variable(const char * var_name, const char * var_value,char * resp_buffer,uint32_t resp_buffer_len,int32_t * execute_store)
{
  int               ii;
  int               resp_len;
  const  uint32_t   var_table_dim = var_table_size/sizeof(var_ptable_t) ;  
  int32_t           value_i; 
  uint32_t          value_u;
  float             value_f;  


  
  resp_len = -1; 



  for(ii = 0; ii < (int)var_table_dim;ii++)
  {
      if(strcmp((char*)var_table[ii].name,(char*)var_name) == 0)
      {
          if(var_value != NULL)
          {
              // Write request
              if( (var_table[ii].access != E_VA_INT_NONE) &&  (var_table[ii].access != E_VA_UINT_NONE)  &&  (var_table[ii].access !=  E_VA_FLT_NONE)  )
              {                       
                  if( (var_table[ii].access == E_VA_INT_FREE) || (var_table[ii].access == E_VA_INT_LIMIT))
                  {
                      // Signed                            
                      value_i = strtol(var_value,NULL,0);

                      if(  ((value_i >= var_table[ii].range.range_I.min) && (value_i <= var_table[ii].range.range_I.max))  ||
                           (var_table[ii].access == E_VA_INT_FREE)
                      )
                      {
                          *((int32_t*)(var_table[ii].ptr)) = value_i;
                      }
                      else
                      {
                          value_i = *((int32_t*)(var_table[ii].ptr));
                      }
                      resp_len = snprintf(resp_buffer,resp_buffer_len,"%ld",value_i);
                   }
                   else if( (var_table[ii].access == E_VA_UINT_FREE) || (var_table[ii].access == E_VA_UINT_LIMIT))
                   {
                       // Unsigned                            
                       value_u = strtoul(var_value,NULL,0);
                     
                       if(  ((value_u >= var_table[ii].range.range_U.min) && (value_u <= var_table[ii].range.range_U.max))  ||
                            (var_table[ii].access == E_VA_UINT_FREE)
                       )
                       {
                           *((uint32_t*)(var_table[ii].ptr)) = value_u;
                       }
                       else
                       {
                           value_u = *((uint32_t*)(var_table[ii].ptr));
                       }
                       resp_len = snprintf(resp_buffer,resp_buffer_len,"%lu",value_u);
                   }
                   else if( (var_table[ii].access == E_VA_FLT_FREE) || (var_table[ii].access == E_VA_FLT_LIMIT))
                   {
                       // Float                            
                       value_f = atof(var_value);
                     
                       if(  ((value_f >= var_table[ii].range.range_F.min) && (value_f <= var_table[ii].range.range_F.max))  ||
                            (var_table[ii].access == E_VA_FLT_FREE)
                       )
                       {
                           *((float*)(var_table[ii].ptr)) = value_f;
                       }
                       else
                       {
                           value_f = *((float*)(var_table[ii].ptr));
                       }
                       resp_len = snprintf(resp_buffer,resp_buffer_len,"%f",value_f);                           
                   }
                   else
                   {
                      // Not writable
                      var_value = NULL;
                   }
                   if(execute_store != NULL)
                   {
                     *execute_store = 1;
                   }
              }         
              else
              {
                  // Not writable
                  var_value = NULL;
              }                    
         }
       

         // Read request
         if(var_value == NULL)
         {
              if( (var_table[ii].access == E_VA_INT_NONE) || (var_table[ii].access == E_VA_INT_LIMIT)  || (var_table[ii].access == E_VA_INT_FREE))
              {                                       
                  value_i  =  *((int32_t*)(var_table[ii].ptr));
                  resp_len = snprintf(resp_buffer,resp_buffer_len,"%ld",value_i);
              }
              else if( (var_table[ii].access == E_VA_UINT_NONE) || (var_table[ii].access == E_VA_UINT_LIMIT)  || (var_table[ii].access == E_VA_UINT_FREE))
              {
                  value_u  = *((uint32_t*)(var_table[ii].ptr));   
                  resp_len = snprintf(resp_buffer,resp_buffer_len,"%lu",value_u);
              }                        
              else if( (var_table[ii].access == E_VA_FLT_NONE) || (var_table[ii].access == E_VA_FLT_LIMIT)  || (var_table[ii].access == E_VA_FLT_FREE))
              {
                   value_f = *((float*)(var_table[ii].ptr));
                   resp_len = snprintf(resp_buffer,resp_buffer_len,"%f",value_f);  
              }
          }               
          break;
      }
  }

  if(resp_len == -1)
  {
     // Some special cases
     if(strlen(var_name) == 1)
     {
        switch(var_name[0])
        {
            case 'H':
            {
                resp_len = snprintf(resp_buffer,resp_buffer_len,"%s",git_hash_short); 
            }break;

            case 'Y':
            {
                resp_len = snprintf(resp_buffer,resp_buffer_len,"%s",git_hash_long); 
            }break;

            case 'V':
            {
                /*!
                     \tst  Special command 'V'  Provides system  version
             
                */        
                resp_len = snprintf(resp_buffer,resp_buffer_len,"%s",version_string); 

            }break;
        }
      }
  }



  return resp_len;
}






