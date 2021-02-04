#include "services.h"
#include "timer_pwm.h"


extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;

#define TIMER_PWM_PRESCALER_20MHZ		20000000
#define TIMER_PWM_PERIOD_20KHZ			20000

#define TIMER_PWM_PERIOD ((TIMER_PWM_PRESCALER_20MHZ/TIMER_PWM_PERIOD_20KHZ)-1)

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
}

void srv_timer_pwm_once(void)
{

}

void srv_timer_pwm(uint32_t ch, uint32_t duty)
{
	if(duty > 1000)
	{
		duty = 1000;
	}

	duty = TIMER_PWM_PERIOD * duty / 1000;


	switch(ch)
	{
		case TMR_PWM_CH_OUT5:	htim1.Instance->CCR1 = duty; break;
		case TMR_PWM_CH_OUT6:	htim1.Instance->CCR2 = duty; break;
		case TMR_PWM_CH_SERVO1:	htim2.Instance->CCR1 = duty; break;
		case TMR_PWM_CH_SERVO2: htim2.Instance->CCR2 = duty; break;
	}
}
