#include "motion_engine.h"


int32_t  motion_engine_io(uint32_t pin,uint32_t value)
{
	uint32_t 	ctrl_value = 0;

	if(pin == 0)
	{
		return -1;
	}

	// Logical order starts from 1, but addressing starts from 0
	pin = pin - 1;

	if(value != 0)
	{
		ctrl_value = (1<<pin);
	}

	srv_gpio_set_io( (1<< pin) /* mask */ ,ctrl_value);

	return 0;
}

int32_t  motion_engine_io_analog(char * buffer,uint32_t length)
{
	length = snprintf(buffer,length,"ok V0:%u V1:%u\r\n",(unsigned int)srv_adc_get(ADC_IN1),(unsigned int)srv_adc_get(ADC_IN2) );

	return length;
}
