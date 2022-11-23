#include "services.h"
#include "timer_srv.h"


#define TIMER_CALLBACK_CNT 2


typedef struct
{
  TIM_HandleTypeDef  hTim5;
  TIM_HandleTypeDef  hTim6;
  TIM_HandleTypeDef  hTim7;
  TIM_HandleTypeDef  hTim13;
  TIM_HandleTypeDef  hTim15;
  TIM_HandleTypeDef  hTim16;


  timer_callback_fn  time_fast_cb[TIMER_CALLBACK_CNT];
  uint32_t           time_fast_cnt;   

  timer_callback_fn  time_slow_cb[TIMER_CALLBACK_CNT];
  uint32_t           time_slow_cnt;

  timer_callback_fn  step_fn;


  uint32_t           tim_clock;

  uint32_t           tim_tick;
}timer_srv_t;


timer_srv_t tsrv;



void TIM15_IRQHandler()
{
  int ii;


  tsrv.tim_tick++;

  for(ii = 0; ii < tsrv.time_slow_cnt;ii++)
  {
    tsrv.time_slow_cb[ii]();
  }

  __HAL_TIM_CLEAR_IT(&tsrv.hTim15, TIM_IT_UPDATE);
  __DSB();
  //GPIO_Set_Unc(OUT_CPU9);

}

void  TIM6_DAC_IRQHandler()
{
  int ii;


  GPIO_Set_Unc(OUT_CPU10);

  for(ii = 0; ii < tsrv.time_fast_cnt;ii++)
  {
    tsrv.time_fast_cb[ii]();
  }


  //GPIO_Clr_Unc(OUT_CPU10);

  __HAL_TIM_CLEAR_IT(&tsrv.hTim6, TIM_IT_UPDATE);
  __DSB();


}



void TIM16_IRQHandler()
{
   GPIO_Set_Unc(OUT_CPU9);

   if(tsrv.step_fn != NULL) // TODO - make it better
   {
	   tsrv.step_fn();
   }

   //GPIO_Clr_Unc(OUT_CPU9);


  __HAL_TIM_CLEAR_IT(&tsrv.hTim16, TIM_IT_UPDATE);
  __DSB();

}

void srv_timer_once(void)
{
	srv_timer_pwm_once();
	srv_timer_pulse_once();
	srv_timer_quad_once();
}

void srv_timer_refresh(void)
{
	srv_timer_pwm_refresh();
}

void srv_timer_callback_slow_add(timer_callback_fn fn)
{
  if(tsrv.time_slow_cnt < TIMER_CALLBACK_CNT)
  {
    tsrv.time_slow_cb[tsrv.time_slow_cnt] = fn;
    tsrv.time_slow_cnt++;
  }
  

}

void srv_timer_callback_fast_add(timer_callback_fn fn )
{
  if(tsrv.time_fast_cnt < TIMER_CALLBACK_CNT)
  {
    tsrv.time_fast_cb[tsrv.time_fast_cnt] = fn;
    tsrv.time_fast_cnt++;
  }
}

void srv_timer_callback_step(uint32_t freq,timer_callback_fn fn)
{
	HAL_TIM_Base_Stop(&tsrv.hTim16);

	tsrv.step_fn = fn;

    tsrv.hTim16.Init.Period        = TIMER_SRV_10MHZ/freq;
    HAL_TIM_Base_Init(&tsrv.hTim16);

    HAL_TIM_Base_Start_IT(&tsrv.hTim16);

}

void  srv_timer_callback_step_disable(void)
{
	 HAL_TIM_Base_Stop_IT(&tsrv.hTim16);
}

void srv_timer_init(void)
{
	srv_timer_pwm_init();
	srv_timer_pulse_init();
	srv_timer_quad_init();


    memset(&tsrv,0,sizeof(tsrv));

    tsrv.tim_clock = SystemCoreClock / 2;

    /* Free running hardware tick counter  - 1ms */

    __HAL_RCC_TIM7_CLK_ENABLE();

    tsrv.hTim7.Instance           = TIM7;
    tsrv.hTim7.Init.Prescaler     = (tsrv.tim_clock/ 1000) - 1;
    tsrv.hTim7.Init.CounterMode   = TIM_COUNTERMODE_UP;
    tsrv.hTim7.Init.Period        = -1;
    tsrv.hTim7.Init.ClockDivision = 0;
    HAL_TIM_Base_Init(&tsrv.hTim7);

    HAL_TIM_Base_Start(&tsrv.hTim7); // Trying to start the base counter



   /* Free running hardware tick counter  - 1us */

    __HAL_RCC_TIM13_CLK_ENABLE();

    tsrv.hTim13.Instance           = TIM13;
    tsrv.hTim13.Init.Prescaler     = (tsrv.tim_clock/ TIMER_SRV_1MHZ) - 1;
    tsrv.hTim13.Init.CounterMode   = TIM_COUNTERMODE_UP;
    tsrv.hTim13.Init.Period        = -1;
    tsrv.hTim13.Init.ClockDivision = 0;
    HAL_TIM_Base_Init(&tsrv.hTim13);

    HAL_TIM_Base_Start(&tsrv.hTim13); // Trying to start the fast base counter



    /*
     * Service callback timer - slow ( 1kHz)
     *
     * FreeRtos calls supported in callbacks
     *
     * */

    __HAL_RCC_TIM15_CLK_ENABLE();

    tsrv.hTim15.Instance           = TIM15;
    tsrv.hTim15.Init.Prescaler     = (tsrv.tim_clock/ TIMER_SRV_1MHZ) - 1;
    tsrv.hTim15.Init.CounterMode   = TIM_COUNTERMODE_UP;
    tsrv.hTim15.Init.Period        = TIMER_SRV_1MHZ / TIMER_SRV_SLOW_HZ;
    tsrv.hTim15.Init.ClockDivision = 0;
    HAL_TIM_Base_Init(&tsrv.hTim15);

    HAL_TIM_Base_Start_IT(&tsrv.hTim15);

    HAL_NVIC_SetPriority(TIM15_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(TIM15_IRQn);


    /* Service callback timer - fast ( 10 kHz)
     *
     * FreeRtos calls supported in callbacks
     *
     */

    __HAL_RCC_TIM6_CLK_ENABLE();

    tsrv.hTim6.Instance           = TIM6;
    tsrv.hTim6.Init.Prescaler     = (tsrv.tim_clock/ TIMER_SRV_1MHZ) - 1;
    tsrv.hTim6.Init.CounterMode   = TIM_COUNTERMODE_UP;
    tsrv.hTim6.Init.Period        = TIMER_SRV_1MHZ / TIMER_SRV_FAST_HZ;
    tsrv.hTim6.Init.ClockDivision = 0;
    HAL_TIM_Base_Init(&tsrv.hTim6);

    HAL_TIM_Base_Start_IT(&tsrv.hTim6);

    HAL_NVIC_SetPriority(TIM6_DAC_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);



    /* Step generating timer */

    __HAL_RCC_TIM16_CLK_ENABLE();

    tsrv.hTim16.Instance           = TIM16;
    tsrv.hTim16.Init.Prescaler     = (tsrv.tim_clock/ TIMER_SRV_10MHZ) - 1;
    tsrv.hTim16.Init.CounterMode   = TIM_COUNTERMODE_UP;
    tsrv.hTim16.Init.Period        = 65535;
    tsrv.hTim16.Init.ClockDivision = 0;
    HAL_TIM_Base_Init(&tsrv.hTim16);

    HAL_TIM_Base_Start_IT(&tsrv.hTim16);

    HAL_NVIC_SetPriority(TIM16_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM16_IRQn);

}




void ATTRIBUTE_IN_RAM srv_hwio_delay_ms(uint32_t ms) 
{
   uint32_t  tick;
   uint32_t  diff;

   tick = __HAL_TIM_GET_COUNTER(&tsrv.hTim7);

   do
   {
      diff = __HAL_TIM_GET_COUNTER(&tsrv.hTim7) - tick;
   }while( diff < ms);
}

void ATTRIBUTE_IN_RAM srv_hwio_delay_us(uint32_t us) 
{
   uint32_t  tick;
   uint32_t  diff;

   tick = __HAL_TIM_GET_COUNTER(&tsrv.hTim13);

   do
   {
      diff = __HAL_TIM_GET_COUNTER(&tsrv.hTim13) - tick;
   }while( diff < us);
}

__INLINE uint32_t ATTRIBUTE_IN_RAM srv_hwio_timestamp_ms_tick(void)
{
  return tsrv.tim_tick;
}


__INLINE srv_hwio_timestamp_t ATTRIBUTE_IN_RAM srv_hwio_timestamp_ms(void)
{
  return __HAL_TIM_GET_COUNTER(&tsrv.hTim7);
}

__INLINE srv_hwio_timestamp_t ATTRIBUTE_IN_RAM srv_hwio_timestamp_us(void)
{
  return __HAL_TIM_GET_COUNTER(&tsrv.hTim13);
}


__INLINE uint32_t ATTRIBUTE_IN_RAM srv_hwio_timestamp_diff(srv_hwio_timestamp_t t_new,srv_hwio_timestamp_t t_old)
{
    return (uint32_t)((uint16_t)(t_new-t_old));
}


__INLINE void ATTRIBUTE_IN_RAM srv_udelay(uint32_t cnt)
{
  volatile int ii;

  do
  {  
    __DSB();
    if(cnt == 0)
    {
      break;
    }

    for (ii = 0; ii < 100;ii++)
    {
      __NOP();
    };
   
    cnt --;
  }while(1);

}
