#include "services.h"
#include "timer_pulse.h"




#define TIMER_PWM_PRESCALER_10MHZ	10000000

static void srv_timer_pulse_timing(uint32_t freq);

void srv_timer_pulse_init(void)
{
	srv_timer_pulse_timing(100000);
}


static void srv_timer_pulse_timing(uint32_t freq)
{
   uint32_t				 tim_clock;
   TIM_OC_InitTypeDef    sConfigOC = {0};
   uint32_t				 period_01us;

   tim_clock = SystemCoreClock / 4;


   period_01us = ((10 * 1000000) / freq)/2;


   htim12.Init.Prescaler = (tim_clock/TIMER_PWM_PRESCALER_10MHZ);
   htim12.Init.Period    = period_01us;

   if (HAL_TIM_Base_Init(&htim12) != HAL_OK)
   {
 	  Error_Handler();
   }

   sConfigOC.OCMode 	= TIM_OCMODE_TIMING;
   sConfigOC.Pulse 	    = 1;
   sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
   sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

   if (HAL_TIM_OC_ConfigChannel(&htim12, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
   {
	  Error_Handler();
   }


   htim8.Init.Prescaler = (tim_clock/TIMER_PWM_PRESCALER_10MHZ);
   htim8.Init.Period    = period_01us;

   if (HAL_TIM_Base_Init(&htim8) != HAL_OK)
   {
	  Error_Handler();
   }

   sConfigOC.OCMode = TIM_OCMODE_TIMING;
   sConfigOC.Pulse = 1;
   sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
   sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
   sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
   sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
   sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
   if (HAL_TIM_OC_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
   {
	   Error_Handler();
   }


   htim15.Init.Prescaler = (tim_clock/TIMER_PWM_PRESCALER_10MHZ);
   htim15.Init.Period    = period_01us;

   if (HAL_TIM_Base_Init(&htim15) != HAL_OK)
   {
	  Error_Handler();
   }

   sConfigOC.OCMode = TIM_OCMODE_TIMING;
   sConfigOC.Pulse = 1;
   sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
   sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
   sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
   sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
   sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
   if (HAL_TIM_OC_ConfigChannel(&htim15, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
   {
	   Error_Handler();
   }
}

void srv_timer_pulse_once(void)
{

}

void srv_timer_pulse_period(uint32_t period_01us)
{





}
