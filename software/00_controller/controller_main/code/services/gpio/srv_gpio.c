#include "system.h"
#include "services.h"
#include "engine.h"
#include "gpio_srv.h"


volatile int32_t gpio_test_mode = 0;


uint32_t 		gpio_pwm_out[PWM_GLOBAL_CNT] = {0};
uint32_t		gpio_pwm_timer[PWM_GLOBAL_CNT] = {0};
uint32_t		gpio_pwm_value 			= 0;
uint32_t		gpio_pwm_value_prev 	= 0;


void  srv_gpio_pinmode(uint32_t out_mask)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pull = GPIO_NOPULL;

    GPIO_InitStruct.Pin = IO_CPU1_Pin;
	if(out_mask & BV(0))
	{
	    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	}
	else
	{
		 GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	}
    HAL_GPIO_Init(IO_CPU1_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = IO_CPU2_Pin;
	if(out_mask & BV(1))
	{
	    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	}
	else
	{
		 GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	}
    HAL_GPIO_Init(IO_CPU2_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = IO_CPU3_Pin;
	if(out_mask & BV(2))
	{
	    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	}
	else
	{
		 GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	}
    HAL_GPIO_Init(IO_CPU3_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = IO_CPU4_Pin;
	if(out_mask & BV(3))
	{
	    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	}
	else
	{
		 GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	}
    HAL_GPIO_Init(IO_CPU4_GPIO_Port, &GPIO_InitStruct);


    GPIO_InitStruct.Pin = IO_CPU5_Pin;
	if(out_mask & BV(4))
	{
	    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	}
	else
	{
		 GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	}
    HAL_GPIO_Init(IO_CPU5_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = IO_CPU6_Pin;
	if(out_mask & BV(5))
	{
	    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	}
	else
	{
		 GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	}
    HAL_GPIO_Init(IO_CPU6_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = IO_CPU7_Pin;
	if(out_mask & BV(6))
	{
	    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	}
	else
	{
		 GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	}
    HAL_GPIO_Init(IO_CPU7_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = IO_CPU8_Pin;
	if(out_mask & BV(7))
	{
	    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	}
	else
	{
		 GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	}
    HAL_GPIO_Init(IO_CPU8_GPIO_Port, &GPIO_InitStruct);


    GPIO_InitStruct.Pin = IO_CPU9_Pin;
	if(out_mask & BV(8))
	{
	    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	}
	else
	{
		 GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	}
    HAL_GPIO_Init(IO_CPU9_GPIO_Port, &GPIO_InitStruct);
}


void  srv_gpio_pullupmode(uint32_t pullup_mask)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};


    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pin = IN_CPU1_Pin;
	if(pullup_mask & BV(0))
	{
		GPIO_InitStruct.Pull = GPIO_PULLUP;
	}
	else
	{
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	}
    HAL_GPIO_Init(IN_CPU1_GPIO_Port, &GPIO_InitStruct);



    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pin = IN_CPU2_Pin;
	if(pullup_mask & BV(1))
	{
		GPIO_InitStruct.Pull = GPIO_PULLUP;
	}
	else
	{
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	}
    HAL_GPIO_Init(IN_CPU2_GPIO_Port, &GPIO_InitStruct);


    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pin = IN_CPU3_Pin;
	if(pullup_mask & BV(3))
	{
		GPIO_InitStruct.Pull = GPIO_PULLUP;
	}
	else
	{
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	}
    HAL_GPIO_Init(IN_CPU3_GPIO_Port, &GPIO_InitStruct);



    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pin = IN_CPU4_Pin;
	if(pullup_mask & BV(3))
	{
		GPIO_InitStruct.Pull = GPIO_PULLUP;
	}
	else
	{
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	}
    HAL_GPIO_Init(IN_CPU4_GPIO_Port, &GPIO_InitStruct);


    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
     GPIO_InitStruct.Pin = IN_CPU5_Pin;
 	if(pullup_mask & BV(4))
 	{
 		GPIO_InitStruct.Pull = GPIO_PULLUP;
 	}
 	else
 	{
 		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
 	}
     HAL_GPIO_Init(IN_CPU5_GPIO_Port, &GPIO_InitStruct);



     GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
     GPIO_InitStruct.Pin = IN_CPU6_Pin;
 	if(pullup_mask & BV(5))
 	{
 		GPIO_InitStruct.Pull = GPIO_PULLUP;
 	}
 	else
 	{
 		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
 	}
     HAL_GPIO_Init(IN_CPU6_GPIO_Port, &GPIO_InitStruct);


    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pin = IN_CPU7_Pin;
 	if(pullup_mask & BV(6))
 	{
 		GPIO_InitStruct.Pull = GPIO_PULLUP;
 	}
 	else
 	{
 		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
 	}
    HAL_GPIO_Init(IN_CPU7_GPIO_Port, &GPIO_InitStruct);



    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pin = IN_CPU8_Pin;
 	if(pullup_mask & BV(7))
 	{
 		GPIO_InitStruct.Pull = GPIO_PULLUP;
 	}
 	else
 	{
 		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
 	}
    HAL_GPIO_Init(IN_CPU8_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pin = IN_CPU9_Pin;
	if(pullup_mask & BV(8))
	{
		GPIO_InitStruct.Pull = GPIO_PULLUP;
	}
	else
	{
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	}
    HAL_GPIO_Init(IN_CPU9_GPIO_Port, &GPIO_InitStruct);



    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pin = IN_CPU10_Pin;
	if(pullup_mask & BV(9))
	{
		GPIO_InitStruct.Pull = GPIO_PULLUP;
	}
	else
	{
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	}
    HAL_GPIO_Init(IN_CPU10_GPIO_Port, &GPIO_InitStruct);
}


void      srv_gpio_init(void)
{

}



uint32_t  srv_gpio_get_io(void)
{
	uint32_t  io = 0;

	if(GPIO_Get(IN_CPU1)!=0)
	{
		io |= BV(0);
	}

	if(GPIO_Get(IN_CPU2)!=0)
	{
		io |= BV(1);
	}

	if(GPIO_Get(IN_CPU3)!=0)
	{
		io |= BV(2);
	}

	if(GPIO_Get(IN_CPU4)!=0)
	{
		io |= BV(3);
	}

	if(GPIO_Get(IN_CPU5)!=0)
	{
		io |= BV(4);
	}

	if(GPIO_Get(IN_CPU6)!=0)
	{
		io |= BV(5);
	}

	if(GPIO_Get(IN_CPU7)!=0)
	{
		io |= BV(6);
	}

	if(GPIO_Get(IN_CPU8)!=0)
	{
		io |= BV(7);
	}

	if(GPIO_Get(IN_CPU9)!=0)
	{
		io |= BV(8);
	}

	if(GPIO_Get(IN_CPU10)!=0)
	{
		io |= BV(9);
	}

	if(GPIO_Get(IO_CPU1)!=0)
	{
		io |= BV(0+16);
	}

	if(GPIO_Get(IO_CPU2)!=0)
	{
		io |= BV(1+16);
	}

	if(GPIO_Get(IO_CPU3)!=0)
	{
		io |= BV(2+16);
	}

	if(GPIO_Get(IO_CPU4)!=0)
	{
		io |= BV(3+16);
	}

	if(GPIO_Get(IO_CPU5)!=0)
	{
		io |= BV(4+16);
	}

	if(GPIO_Get(IO_CPU6)!=0)
	{
		io |= BV(5+16);
	}

	if(GPIO_Get(IO_CPU7)!=0)
	{
		io |= BV(6+16);
	}

	if(GPIO_Get(IO_CPU8)!=0)
	{
		io |= BV(7+16);
	}

	if(GPIO_Get(IO_CPU9)!=0)
	{
		io |= BV(8+16);
	}


	return io;
}


void      srv_gpio_set_pwm(uint32_t mask,uint32_t value)
{

	if(mask & BV(0))
	{
		if(value & BV(0))
		{
			gpio_pwm_timer[PWM_OUT1]  = ppctx_nv->pwm_range[PWM_OUT1].delay_ms;

			if(gpio_pwm_timer[PWM_OUT1] == 0)
			{
				srv_timer_pwm(PWM_OUT1,ppctx_nv->pwm_range[PWM_OUT1].max);
			}
			else
			{
				srv_timer_pwm(PWM_OUT1,100);
			}

			gpio_pwm_out [PWM_OUT1]  = 1;
		}
		else
		{
			gpio_pwm_timer[PWM_OUT1] = 0;
			gpio_pwm_out[PWM_OUT1] 	 = 0;

			srv_timer_pwm(PWM_OUT1,ppctx_nv->pwm_range[PWM_OUT1].min);
		}
	}

	if(mask & BV(1))
	{

		if(value & BV(1))
		{
			gpio_pwm_timer[PWM_OUT2]  = ppctx_nv->pwm_range[PWM_OUT2].delay_ms;

			if(gpio_pwm_timer[PWM_OUT2] == 0)
			{
				srv_timer_pwm(PWM_OUT2,ppctx_nv->pwm_range[PWM_OUT2].max);
			}
			else
			{
				srv_timer_pwm(PWM_OUT2,100);
			}

			gpio_pwm_out [PWM_OUT2]  = 1;
		}
		else
		{
			gpio_pwm_timer[PWM_OUT2]  = 0;
			gpio_pwm_out [PWM_OUT2]   = 0;

			srv_timer_pwm(PWM_OUT2,ppctx_nv->pwm_range[PWM_OUT2].min);
		}
	}


	if(mask & BV(2))
	{

		if(value & BV(2))
		{
			gpio_pwm_timer[PWM_OUT3]  = ppctx_nv->pwm_range[PWM_OUT3].delay_ms;


			if(gpio_pwm_timer[PWM_OUT3] == 0)
			{
				srv_timer_pwm(PWM_OUT3,ppctx_nv->pwm_range[PWM_OUT3].max);
			}
			else
			{
				srv_timer_pwm(PWM_OUT3,100);
			}

			gpio_pwm_out [PWM_OUT3]  = 1;
		}
		else
		{
			gpio_pwm_timer[PWM_OUT3]  = 0;
			gpio_pwm_out [PWM_OUT3]   = 0;

			srv_timer_pwm(PWM_OUT3,ppctx_nv->pwm_range[PWM_OUT3].min);
		}
	}

	if(mask & BV(3))
	{
		if(value & BV(3))
		{
			gpio_pwm_timer[PWM_OUT4]  = ppctx_nv->pwm_range[PWM_OUT4].delay_ms;


			if(gpio_pwm_timer[PWM_OUT4] == 0)
			{
				srv_timer_pwm(PWM_OUT4,ppctx_nv->pwm_range[PWM_OUT4].max);
			}
			else
			{
				srv_timer_pwm(PWM_OUT4,100);
			}

			gpio_pwm_out [PWM_OUT4]  = 1;
		}
		else
		{
			gpio_pwm_timer[PWM_OUT4]  =   0;
			gpio_pwm_out [PWM_OUT4]   =   0;

			srv_timer_pwm(PWM_OUT4,ppctx_nv->pwm_range[PWM_OUT4].min);
		}
	}

	if(mask & BV(10))
	{
		if(value & BV(10))
		{
			gpio_pwm_timer[PWM_OUT11]  = ppctx_nv->pwm_range[PWM_OUT11].delay_ms;


			if(gpio_pwm_timer[PWM_OUT11] == 0)
			{
				srv_timer_pwm(PWM_OUT11,ppctx_nv->pwm_range[PWM_OUT11].max);
			}
			else
			{
				srv_timer_pwm(PWM_OUT11,100);
			}

			gpio_pwm_out [PWM_OUT11]   = 1;
		}
		else
		{
			gpio_pwm_timer[PWM_OUT11]  =   0;
			gpio_pwm_out [PWM_OUT11]   =   0;

			srv_timer_pwm(PWM_OUT11,ppctx_nv->pwm_range[PWM_OUT11].min);
		}
	}

	if(mask & BV(11))
	{
		if(value & BV(11))
		{
			gpio_pwm_timer[PWM_OUT12]  = ppctx_nv->pwm_range[PWM_OUT12].delay_ms;


			if(gpio_pwm_timer[PWM_OUT12] == 0)
			{
				srv_timer_pwm(PWM_OUT12,ppctx_nv->pwm_range[PWM_OUT12].max);
			}
			else
			{
				srv_timer_pwm(PWM_OUT12,100);
			}
			gpio_pwm_out [PWM_OUT12]     = 1;
		}
		else
		{
			gpio_pwm_timer[PWM_OUT12]  =   0;
			gpio_pwm_out [PWM_OUT12]   =   1;

			srv_timer_pwm(PWM_OUT12,ppctx_nv->pwm_range[PWM_OUT12].min);
		}
	}
}



void      srv_gpio_tick_io(void)
{
	uint32_t ii;

	uint32_t  gpio_pwm_value_next;


	gpio_pwm_value_next =  gpio_pwm_value;
	srv_gpio_set_pwm(gpio_pwm_value_prev ^ gpio_pwm_value_next,gpio_pwm_value);
	gpio_pwm_value_prev = gpio_pwm_value_next;


	for(ii = 0; ii < PWM_GLOBAL_CNT;ii++)
	{
		if(gpio_pwm_out[ii] != 0 )
		{
			if( gpio_pwm_timer[ii] > 0)
			{
				gpio_pwm_timer[ii]--;
				if(gpio_pwm_timer[ii] == 0)
				{
					srv_timer_pwm(ii,ppctx_nv->pwm_range[ii].max);
				}
			}
		}
	}
}


void      srv_gpio_set_io(uint32_t mask,uint32_t value)
{
	uint32_t pwm_mask = mask & ( BV(0)|BV(1)|BV(2)|BV(3)|BV(10)|BV(11));

	gpio_pwm_value = (gpio_pwm_value & (~pwm_mask)  ) | (value & pwm_mask );

	if(mask & BV(4))
	{
		if(value & BV(4))
		{
			GPIO_Set(OUT_CPU5);
		}
		else
		{
			GPIO_Clr(OUT_CPU5);
		}
	}

	if(mask & BV(5))
	{
		if(value & BV(5))
		{
			GPIO_Set(OUT_CPU6);
		}
		else
		{
			GPIO_Clr(OUT_CPU6);
		}
	}

	if(mask & BV(6))
	{
		if(value & BV(6))
		{
			GPIO_Set(OUT_CPU7);
		}
		else
		{
			GPIO_Clr(OUT_CPU7);
		}
	}
	if(mask & BV(7))
	{
		if(value & BV(7))
		{
			GPIO_Set(OUT_CPU8);
		}
		else
		{
			GPIO_Clr(OUT_CPU8);
		}
	}

	if(mask & BV(8))
	{
		if(value & BV(8))
		{
			GPIO_Set(OUT_CPU9);
		}
		else
		{
			GPIO_Clr(OUT_CPU9);
		}
	}

	if(mask & BV(9))
	{
		if(value & BV(9))
		{
			GPIO_Set(OUT_CPU10);
		}
		else
		{
			GPIO_Clr(OUT_CPU10);
		}
	}


	if(mask & BV(0+16))
	{
		if(value & BV(0+16))
		{
			GPIO_Set(IO_CPU1);
		}
		else
		{
			GPIO_Clr(IO_CPU1);
		}
	}


	if(mask & BV(1+16))
	{
		if(value & BV(1+16))
		{
			GPIO_Set(IO_CPU2);
		}
		else
		{
			GPIO_Clr(IO_CPU2);
		}
	}

	if(mask & BV(2+16))
	{
		if(value & BV(2+16))
		{
			GPIO_Set(IO_CPU3);
		}
		else
		{
			GPIO_Clr(IO_CPU3);
		}
	}

	if(mask & BV(3+16))
	{
		if(value & BV(3+16))
		{
			GPIO_Set(IO_CPU4);
		}
		else
		{
			GPIO_Clr(IO_CPU4);
		}
	}

	if(mask & BV(4+16))
	{
		if(value & BV(4+16))
		{
			GPIO_Set(IO_CPU5);
		}
		else
		{
			GPIO_Clr(IO_CPU5);
		}
	}

	if(mask & BV(5+16))
	{
		if(value & BV(5+16))
		{
			GPIO_Set(IO_CPU6);
		}
		else
		{
			GPIO_Clr(IO_CPU6);
		}
	}

	if(mask & BV(6+16))
	{
		if(value & BV(6+16))
		{
			GPIO_Set(IO_CPU7);
		}
		else
		{
			GPIO_Clr(IO_CPU7);
		}
	}

	if(mask & BV(7+16))
	{
		if(value & BV(7+16))
		{
			GPIO_Set(IO_CPU8);
		}
		else
		{
			GPIO_Clr(IO_CPU8);
		}
	}

	if(mask & BV(8+16))
	{
		if(value & BV(8+16))
		{
			GPIO_Set(IO_CPU9);
		}
		else
		{
			GPIO_Clr(IO_CPU9);
		}
	}
}



uint32_t srv_gpio_dump(char * buffer, uint32_t buffer_size)
{
  uint32_t length;
  uint32_t in;

  in = srv_gpio_get_io();

  length = snprintf(
		  	  buffer,
			  buffer_size,
			  "GPIO : IN1 %ld IN2 %ld IN3 %ld IN4 %ld IN5 %ld IN6 %ld IN7 %ld IN8 %ld IN9 %ld IN10 %ld \r\n"
			  "GPIOE: IO1 %ld IO2 %ld IO3 %ld IO4 %ld IO5 %ld IO6 %ld IO7 %ld IO8 %ld IO9 %ld\r\n"
			  ,
		  (in>>0) & 0x01,
		  (in>>1) & 0x01,
		  (in>>2) & 0x01,
		  (in>>3) & 0x01,
		  (in>>4) & 0x01,
		  (in>>5) & 0x01,
		  (in>>6) & 0x01,
		  (in>>7) & 0x01,
		  (in>>8) & 0x01,
		  (in>>9) & 0x01,

		  (in>>16) & 0x01,
		  (in>>17) & 0x01,
		  (in>>18) & 0x01,
		  (in>>19) & 0x01,
		  (in>>20) & 0x01,
		  (in>>21) & 0x01,
		  (in>>22) & 0x01,
		  (in>>23) & 0x01,
		  (in>>24) & 0x01
  );


  return length;
}


void  srv_gpio_refresh(void)
{
	srv_gpio_pinmode(ppctx_nv->io_cpu_dir);
	srv_gpio_pullupmode(ppctx_nv->in_cpu_pullup);
}




void  srv_gpio_once(void)
{
	srv_gpio_refresh();

	srv_timer_callback_slow_add(srv_gpio_tick_io);
}


