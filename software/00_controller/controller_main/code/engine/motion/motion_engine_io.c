#include "motion_engine.h"


int32_t  motion_engine_io(uint32_t pin,uint32_t value)
{
	uint32_t 	ctrl_value = 0;
	uint32_t 	mask 	   = ( 1<< pin);

	if(value != 0)
	{
		ctrl_value = (1<<pin);
	}

	srv_gpio_set_io(mask,ctrl_value);

	return 0;
}

int32_t  motion_engine_io_analog(char * buffer,uint32_t length)
{
	length = snprintf(buffer,length,"ok V0:%u V1:%u\r\n",(unsigned int)srv_adc_get(ADC_S1),(unsigned int)srv_adc_get(ADC_S2) );

	return length;
}
