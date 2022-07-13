#include "services.h"
#include "timer_pwm.h"
#include "common.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim5;

#define TIMER_PWM_PRESCALER_20MHZ		20000000
#define TIMER_PWM_PERIOD_16KHZ			16000

#define TIMER_PWM_PERIOD ((TIMER_PWM_PRESCALER_20MHZ/TIMER_PWM_PERIOD_16KHZ)-1)

void srv_timer_pwm_init(void)
{
	uint32_t tim_clock;


	tim_clock = SystemCoreClock / 4;

	htim1.Init.Prescaler = tim_clock / TIMER_PWM_PRESCALER_20MHZ -1;
	htim1.Init.Period = TIMER_PWM_PERIOD;

	if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
	{
	    /* Initialization Error */
	    Error_Handler();
	}

	if (HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1) != HAL_OK)
	{
	  /* PWM Generation Error */
	  Error_Handler();
	}

	if (HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2) != HAL_OK)
	{
	  /* PWM Generation Error */
	  Error_Handler();
	}

	if (HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4) != HAL_OK)
	{
	  /* PWM Generation Error */
	  Error_Handler();
	}


	htim2.Init.Prescaler = tim_clock / TIMER_PWM_PRESCALER_20MHZ -1;
	htim2.Init.Period = TIMER_PWM_PERIOD;

	if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
	{
	    /* Initialization Error */
	    Error_Handler();
	}

	if (HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1) != HAL_OK)
	{
	  /* PWM Generation Error */
	  Error_Handler();
	}

	if (HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2) != HAL_OK)
	{
	  /* PWM Generation Error */
	  Error_Handler();
	}

	htim5.Init.Prescaler = tim_clock / TIMER_PWM_PRESCALER_20MHZ -1;
	htim5.Init.Period = TIMER_PWM_PERIOD;

	if (HAL_TIM_PWM_Init(&htim5) != HAL_OK)
	{
	    /* Initialization Error */
	    Error_Handler();
	}

	if (HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_4) != HAL_OK)
	{
	  /* PWM Generation Error */
	  Error_Handler();
	}


}

void srv_timer_pwm_once(void)
{

}

void srv_timer_pwm(uint32_t ch, uint32_t duty_percent)
{
	uint32_t duty;

	if(duty_percent > 100)
	{
		duty_percent = 100;
	}

	duty = TIMER_PWM_PERIOD * duty_percent / 100;


	switch(ch)
	{
		case PWM_OUT1:	htim1.Instance->CCR1 = duty; break;
		case PWM_OUT2:	htim1.Instance->CCR2 = duty; break;
		case PWM_OUT3:	htim5.Instance->CCR4 = duty; break;
		case PWM_OUT4:  htim1.Instance->CCR4 = duty; break;
		case PWM_OUT11:	htim2.Instance->CCR1 = duty; break;
		case PWM_OUT12: htim2.Instance->CCR2 = duty; break;
	}
}
