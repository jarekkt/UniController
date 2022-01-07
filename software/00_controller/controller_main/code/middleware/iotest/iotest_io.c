#include "iotest_io.h"
#include "services.h"
#include "middleware.h"


typedef struct
{
	uint32_t  io_request;
	uint32_t  io_special;
	char      buffer[128];
}iot_io_t;


iot_io_t iot_io;

const var_ptable_t   iotest_io_var_ptable[] SERMON_ATTR =  
{
  { "io_request",         &iot_io.io_request  ,       E_VA_UINT_FREE        },
  { "io_special",         &iot_io.io_special  ,       E_VA_UINT_FREE        },
};


void mwr_iotest_io_init()
{
    memset(&iot_io,0,sizeof(iot_io));

    srv_sermon_register(iotest_io_var_ptable,sizeof(iotest_io_var_ptable));
}



static void iotest_io_special(uint32_t io_special)
{
    if( (io_special & 0x01) != 0)
    {

    }


}


void iotest_io_control(uint32_t io_request)
{
	static int div = 0;


	if( ( div ++ % 100) !=0)
	{
		return;
	}


    if( (io_request & 0x01) != 0)
    {
        srv_adc_dump(iot_io.buffer, sizeof(iot_io.buffer));
        burst_log_printf(1,iot_io.buffer);
    }

    if( (io_request & 0x01) != 0)
    {
    	 srv_adc_dump(iot_io.buffer, sizeof(iot_io.buffer));
    	 burst_log_printf(1,iot_io.buffer);
    }
}




void mwr_iotest_io_task(void)
{
    iotest_io_control(iot_io.io_request);

    if(iot_io.io_special != 0)
    {
      iotest_io_special(iot_io.io_special);
      iot_io.io_special = 0;
    }

}


