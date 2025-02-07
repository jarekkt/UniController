#include "services.h"
#include "timer_pulse.h"




#define TIMER_PWM_PRESCALER_10MHZ	10000000





void srv_timer_pulse_init(void)
{
	srv_timer_pulse_period_for_hz(100000);
}


void srv_timer_pulse_period_for_hz(uint32_t step_freq)
{
   TIM_OC_InitTypeDef 			 sConfigOC;
   TIM_ClockConfigTypeDef 		 sClockSourceConfig;
   uint32_t						 tim_clock;
   uint32_t 					 pulse_01us;


   memset(&sClockSourceConfig,0,sizeof(sClockSourceConfig));
   memset(&sConfigOC,0,sizeof(sConfigOC));


   tim_clock = SystemCoreClock / 2;

   pulse_01us = (TIMER_PWM_PRESCALER_10MHZ / step_freq)/2;


   // TIM12 - STEP2
   htim12.Instance 					= TIM12;
   htim12.Init.Prescaler 			= (tim_clock/TIMER_PWM_PRESCALER_10MHZ);
   htim12.Init.CounterMode 			= TIM_COUNTERMODE_UP;
   htim12.Init.Period 				= pulse_01us;
   htim12.Init.ClockDivision 		= TIM_CLOCKDIVISION_DIV1;
   htim12.Init.AutoReloadPreload 	= TIM_AUTORELOAD_PRELOAD_DISABLE;

   if (HAL_TIM_Base_Init(&htim12) != HAL_OK)
   {
     Error_Handler();
   }
   sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
   if (HAL_TIM_ConfigClockSource(&htim12, &sClockSourceConfig) != HAL_OK)
   {
     Error_Handler();
   }

   // Select OnePulse mode
   htim12.Instance->CR1 |= TIM_CR1_OPM;


   // Configure the pulse as PWM - bastarized :-)
   if (HAL_TIM_PWM_Init(&htim12) != HAL_OK)
   {
     Error_Handler();
   }
   sConfigOC.OCMode 				= TIM_OCMODE_PWM1;
   sConfigOC.Pulse 					= 1;
   sConfigOC.OCPolarity 			= TIM_OCPOLARITY_HIGH;
   sConfigOC.OCFastMode 			= TIM_OCFAST_ENABLE;
   sConfigOC.OCIdleState			= TIM_OCIDLESTATE_SET;
   sConfigOC.OCNIdleState 			= TIM_OCNIDLESTATE_SET;

   if (HAL_TIM_PWM_ConfigChannel(&htim12, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
   {
     Error_Handler();
   }

   // Start the timer - it will generate one pulse initially ( as it was set to OnePulse before)
   if (HAL_TIM_PWM_Start(&htim12, TIM_CHANNEL_1) != HAL_OK)
   {
	  /* PWM Generation Error */
	  Error_Handler();
   }

    // TIM12 - STEP2
    htim8.Instance 					= TIM8;
    htim8.Init.Prescaler 			= (tim_clock/TIMER_PWM_PRESCALER_10MHZ);
    htim8.Init.CounterMode 			= TIM_COUNTERMODE_UP;
    htim8.Init.Period 				= pulse_01us;
    htim8.Init.ClockDivision 		= TIM_CLOCKDIVISION_DIV1;
    htim8.Init.AutoReloadPreload 	= TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_Base_Init(&htim8) != HAL_OK)
    {
      Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim8, &sClockSourceConfig) != HAL_OK)
    {
      Error_Handler();
    }

    // Select OnePulse mode
    htim8.Instance->CR1 |= TIM_CR1_OPM;

    // Configure the pulse as PWM - bastarized :-)
    if (HAL_TIM_PWM_Init(&htim8) != HAL_OK)
    {
      Error_Handler();
    }

    if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
    {
      Error_Handler();
    }

    // Start the timer - it will generate one pulse initially ( as it was set to OnePulse before)
    if (HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_3) != HAL_OK)
    {
 	  /* PWM Generation Error */
 	  Error_Handler();
    }


    // TIM17 - STEP3
	htim17.Instance 				= TIM17;
	htim17.Init.Prescaler 			= (tim_clock/TIMER_PWM_PRESCALER_10MHZ);
	htim17.Init.CounterMode 		= TIM_COUNTERMODE_UP;
	htim17.Init.Period 				= pulse_01us;
	htim17.Init.ClockDivision 		= TIM_CLOCKDIVISION_DIV1;
	htim17.Init.AutoReloadPreload 	= TIM_AUTORELOAD_PRELOAD_DISABLE;

	if (HAL_TIM_Base_Init(&htim17) != HAL_OK)
	{
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim17, &sClockSourceConfig) != HAL_OK)
	{
		Error_Handler();
	}

	// Select OnePulse mode
	htim17.Instance->CR1 |= TIM_CR1_OPM;


	// Configure the pulse as PWM - bastarized :-)
	if (HAL_TIM_PWM_Init(&htim17) != HAL_OK)
	{
		Error_Handler();
	}

	if (HAL_TIM_PWM_ConfigChannel(&htim17, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
	{
		Error_Handler();
	}

	// Start the timer - it will generate one pulse initially ( as it was set to OnePulse before)
	if (HAL_TIM_PWM_Start(&htim17, TIM_CHANNEL_1) != HAL_OK)
	{
		/* PWM Generation Error */
		Error_Handler();
	}




}

void srv_timer_pulse_once(void)
{

}



