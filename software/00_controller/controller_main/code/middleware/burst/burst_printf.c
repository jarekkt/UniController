/*!
    \file burst_printf.c

    \brief
*/


#include "system.h"
#include "services.h"
#include "middleware.h"

#include "version.h"
#include "burst_printf.h"



#define PRINTF_BUFFER_LENGTH    1024
#define RCV_FRAME_LENGTH		 128



/*!
    Burst module context data
*/


typedef struct
{
    xSemaphoreHandle  	x_mutex;
    char     		  	print_buffer[PRINTF_BUFFER_LENGTH];


}burst_printf_ctx_t;

typedef struct
{
    uint32_t printf_mask;
}burst_printf_nv_t;

burst_printf_ctx_t    bpctx;
burst_printf_nv_t     bpctx_nv VAR_NV_ATTR;



const var_ptable_t   burst_printf_var_ptable[] SERMON_ATTR =
{
  { "printf_mask",               &bpctx_nv.printf_mask                  ,E_VA_UINT_FREE    }
};



/*!
    \brief  Empty storage init function for burst module    (nv vars)                          
                                    
*/

void burst_printf_default_init(void)
{
   memset(&bpctx_nv,0,sizeof(bpctx_nv));

   bpctx_nv.printf_mask = 0xFFFFFFFF;

}
/*!
    \brief  Init function for burst module                              
                                    
*/

void burst_printf_init(void)
{
	burst_mux_init();

    memset(&bpctx,0,sizeof(bpctx));

    srv_sermon_register(burst_printf_var_ptable,sizeof(burst_printf_var_ptable));
    srv_nov_register(&bpctx_nv,sizeof(bpctx_nv),burst_printf_default_init);
}

/*!
    \brief  Start function executed after all blocks are initialized
                                    
*/

void burst_printf_once(void)
{
   bpctx.x_mutex = xSemaphoreCreateMutex();
}





/*!
    \brief  Standard library printf() replacement function. Message will be sent to serial port

    \param [in] format Same as printf

     Note - This call is blocking until whole message is sent
            *DO NOT* use from interrupts. 
            
*/

void burst_log_printf(uint32_t level_mask,const char  * format, ...)
{
     
        va_list  msg;
        int      length;


        xSemaphoreTake(bpctx.x_mutex,portMAX_DELAY);

        va_start(msg, format);
 
        // In production mode we disable all debug messages
        if( (bpctx_nv.printf_mask & level_mask) != 0 )
        {                     
          length = vsnprintf(bpctx.print_buffer,sizeof(bpctx.print_buffer)-3,format, msg);
          srv_serial_send(SRV_SERIAL_DEBUG,bpctx.print_buffer,length);
        }

        va_end(msg);

        xSemaphoreGive(bpctx.x_mutex);

}

