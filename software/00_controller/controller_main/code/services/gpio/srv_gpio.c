#include "system.h"
#include "services.h"
#include "gpio_srv.h"


volatile int32_t gpio_test_mode = 0;



gpio_data_t gpio;




void      srv_gpio_init(void)
{



	memset(&gpio,0,sizeof(gpio));



	gpio.address = srv_gpio_read_addr();

}

uint32_t  srv_gpio_get_io(void)
{
	uint32_t  io = 0;

	if(GPIO_Get(IN1)!=0)
	{
		io |= 0x01;
	}

	if(GPIO_Get(IN2)!=0)
	{
		io |= 0x02;
	}

	if(GPIO_Get(IN3)!=0)
	{
		io |= 0x04;
	}

	if(GPIO_Get(IN4)!=0)
	{
		io |= 0x08;
	}


	if(GPIO_Get(IN_ADC1)!=0)
	{
		io |= 0x10;
	}

	if(GPIO_Get(IN_ADC2)!=0)
	{
		io |= 0x20;
	}

	if(GPIO_Get(IN_ADC3)!=0)
	{
		io |= 0x40;
	}

	if(GPIO_Get(IN_ADC4)!=0)
	{
		io |= 0x80;
	}

	return io;
}

void      srv_gpio_set_io(uint32_t mask,uint32_t value)
{

	if(mask & 0x01)
	{
		if(value & 0x01)
		{
			GPIO_Set(OUT1);
		}
		else
		{
			GPIO_Clr(OUT1);
		}
	}

	if(mask & 0x02)
	{
		if(value & 0x02)
		{
			GPIO_Set(OUT2);
		}
		else
		{
			GPIO_Clr(OUT2);
		}
	}


	if(mask & 0x04)
	{
		if(value & 0x04)
		{
			GPIO_Set(OUT3);
		}
		else
		{
			GPIO_Clr(OUT3);
		}
	}

	if(mask & 0x08)
	{
		if(value & 0x08)
		{
			GPIO_Set(OUT4);
		}
		else
		{
			GPIO_Clr(OUT4);
		}
	}


	if(mask & 0x10)
	{
		if(value & 0x10)
		{
			srv_timer_pwm(TMR_PWM_CH_OUT5, 1000);
		}
		else
		{
			srv_timer_pwm(TMR_PWM_CH_OUT5, 0);
		}
	}

	if(mask & 0x20)
	{
		if(value & 0x20)
		{
			srv_timer_pwm(TMR_PWM_CH_OUT6, 1000);
		}
		else
		{
			srv_timer_pwm(TMR_PWM_CH_OUT6, 0);
		}
	}


	if(mask & 0x08)
	{
		if(value & 0x08)
		{
			GPIO_Set(LIGHT_ON);
		}
		else
		{
			GPIO_Clr(LIGHT_ON);
		}
	}

}



uint32_t srv_gpio_dump(char * buffer, uint32_t buffer_size)
{
  uint32_t length;
  uint32_t in;

  in = srv_gpio_get_io();

  length = snprintf(buffer,buffer_size,"GPIO: IN1 %ld IN2 %ld IN3 %ld IN4 %ld IN5_ADC1 %ld IN6_ADC2 %ld IN7_ADC3 %ld IN8_ADC4 %ld ADDR 0x%02lX\r\n",
		  in & 0x01,
		  (in>>1) & 0x01,
		  (in>>2) & 0x01,
		  (in>>3) & 0x01,
		  (in>>4) & 0x01,
		  (in>>5) & 0x01,
		  (in>>6) & 0x01,
		  (in>>7) & 0x01,
		  srv_gpio_read_addr()
  );


  return length;
}



void  srv_gpio_once(void)
{

}


