#include "adc_srv.h"





volatile uint16_t adc_vars[ADC_CH_CNT];

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc3;

void      srv_adc_init(void)
{
  if(HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_vars, ADC_S2) != HAL_OK)
  {
	 /* Start Error */
	 fw_assert(0);
  }

  if(HAL_ADC_Start_DMA(&hadc3, (uint32_t*)&adc_vars[ADC_S2], ADC_CH_CNT-ADC_S2) != HAL_OK)
  {
	 /* Start Error */
	 fw_assert(0);
  }
}

void      srv_adc_once(void)
{

}


uint32_t  srv_adc_get(uint32_t ch)
{
    if(ch < ADC_CH_CNT)
    {
        return (uint32_t)adc_vars[ch];
    }
    else
    {
        return 0;
    }

}

uint32_t srv_adc_dump(char * buffer, uint32_t buffer_size)
{
  uint32_t length;

  length = snprintf(buffer,buffer_size,"ADC: IN1 %4d IN2 %4d IN3 %d IN4 %4d S1 %4d S2 %4d\r\n",
      adc_vars[ADC_IN1],
      adc_vars[ADC_IN1],
      adc_vars[ADC_IN1],
      adc_vars[ADC_IN1],
      adc_vars[ADC_S1],
	  adc_vars[ADC_S2]
     );


  return length;
}
