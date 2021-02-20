#include "system.h"
#include "services.h"
#include "gpio_srv.h"


volatile int32_t gpio_test_mode = 0;



void  srv_gpio_pinmode(uint32_t mask)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pull = GPIO_NOPULL;

    GPIO_InitStruct.Pin = IN_CPU5_ADC1_18_Pin;
	if(mask & 0x10)
	{
	    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	}
	else
	{
		 GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	}
    HAL_GPIO_Init(IN_CPU5_ADC1_18_GPIO_Port, &GPIO_InitStruct);



    GPIO_InitStruct.Pin = IN_CPU6_ADC1_19_Pin;
	if(mask & 0x20)
	{
	    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	}
	else
	{
		 GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	}
    HAL_GPIO_Init(IN_CPU6_ADC1_19_GPIO_Port, &GPIO_InitStruct);


    GPIO_InitStruct.Pin = IN_CPU7_ADC1_9_Pin;
	if(mask & 0x40)
	{
	    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	}
	else
	{
		 GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	}
    HAL_GPIO_Init(IN_CPU7_ADC1_9_GPIO_Port, &GPIO_InitStruct);


    GPIO_InitStruct.Pin = IN_CPU8_ADC1_5_Pin;
	if(mask & 0x80)
	{
	    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	}
	else
	{
		 GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	}
    HAL_GPIO_Init(IN_CPU8_ADC1_5_GPIO_Port, &GPIO_InitStruct);

}

void      srv_gpio_init(void)
{
	srv_gpio_pinmode(0);
}

uint32_t  srv_gpio_get_io(void)
{
	uint32_t  io = 0;

	if(GPIO_Get(IN_CPU1)!=0)
	{
		io |= 0x01;
	}

	if(GPIO_Get(IN_CPU2)!=0)
	{
		io |= 0x02;
	}

	if(GPIO_Get(IN_CPU3)!=0)
	{
		io |= 0x04;
	}

	if(GPIO_Get(IN_CPU4)!=0)
	{
		io |= 0x08;
	}

	if(GPIO_Get(IN_CPU5_ADC1_18)!=0)
	{
		io |= 0x10;
	}

	if(GPIO_Get(IN_CPU6_ADC1_19)!=0)
	{
		io |= 0x20;
	}

	if(GPIO_Get(IN_CPU7_ADC1_9)!=0)
	{
		io |= 0x40;
	}

	if(GPIO_Get(IN_CPU8_ADC1_5)!=0)
	{
		io |= 0x80;
	}

	if(GPIO_Get(MID_IO1)!=0)
	{
		io |= 0x100;
	}

	if(GPIO_Get(MID_IO2)!=0)
	{
		io |= 0x200;
	}



	return io;
}

void      srv_gpio_set_io(uint32_t mask,uint32_t value)
{

	if(mask & 0x01)
	{
		if(value & 0x01)
		{
			GPIO_Set(OUT_CPU1);
		}
		else
		{
			GPIO_Clr(OUT_CPU1);
		}
	}

	if(mask & 0x02)
	{
		if(value & 0x02)
		{
			GPIO_Set(OUT_CPU2);
		}
		else
		{
			GPIO_Clr(OUT_CPU2);
		}
	}


	if(mask & 0x04)
	{
		if(value & 0x04)
		{
			GPIO_Set(OUT_CPU3);
		}
		else
		{
			GPIO_Clr(OUT_CPU3);
		}
	}

	if(mask & 0x08)
	{
		if(value & 0x08)
		{
			GPIO_Set(OUT_CPU4);
		}
		else
		{
			GPIO_Clr(OUT_CPU4);
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


	if(mask & 0x40)
	{
		if(value & 0x40)
		{
			GPIO_Set(MID_IO1);
		}
		else
		{
			GPIO_Clr(MID_IO1);
		}
	}

	if(mask & 0x80)
	{
		if(value & 0x80)
		{
			GPIO_Set(MID_IO2);
		}
		else
		{
			GPIO_Clr(MID_IO2);
		}
	}



}



uint32_t srv_gpio_dump(char * buffer, uint32_t buffer_size)
{
  uint32_t length;
  uint32_t in;

  in = srv_gpio_get_io();

  length = snprintf(buffer,buffer_size,"GPIO: IN1 %ld IN2 %ld IN3 %ld IN4 %ld IN5 %ld IN6 %ld IN7 %ld IN8 %ld MID1 %ld MID2 %ld \r\n",
		  in & 0x01,
		  (in>>1) & 0x01,
		  (in>>2) & 0x01,
		  (in>>3) & 0x01,
		  (in>>4) & 0x01,
		  (in>>5) & 0x01,
		  (in>>6) & 0x01,
		  (in>>7) & 0x01,
		  (in>>8) & 0x01,
		  (in>>9) & 0x01
  );


  return length;
}



void  srv_gpio_once(void)
{

}


