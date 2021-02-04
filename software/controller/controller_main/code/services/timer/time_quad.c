#include "services.h"
#include "timer_pulse.h"



extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;

void srv_timer_quad_init(void)
{
    HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
}

void srv_timer_quad_once(void)
{

}

uint32_t srv_timer_quad_get(uint32_t ch)
{
	uint32_t result;

	switch(ch)
	{
	   case 0:
	   {
		  result = __HAL_TIM_GET_COUNTER(&htim3);
	   }break;

	   case 1:
	   {
	      result = __HAL_TIM_GET_COUNTER(&htim4);
	   }break;

	   default:
	   {
		   result = 0;
	   }break;
	}

	return result;
}
