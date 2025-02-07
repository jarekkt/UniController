#ifndef GPIO_SRV_H
#define GPIO_SRV_H


#include "main.h"

#define GPIO_PORT(name_) (name_##_GPIO_Port)
#define GPIO_PIN(name_)  (name_##_Pin)



#define GPIO_Set_Unc(name_)    HAL_GPIO_WritePin(GPIO_PORT(name_),GPIO_PIN(name_),GPIO_PIN_SET)
#define GPIO_Clr_Unc(name_)    HAL_GPIO_WritePin(GPIO_PORT(name_),GPIO_PIN(name_),GPIO_PIN_RESET)

#define GPIO_Set(name_)        if(gpio_test_mode==0)HAL_GPIO_WritePin(GPIO_PORT(name_),GPIO_PIN(name_),GPIO_PIN_SET)
#define GPIO_Clr(name_)        if(gpio_test_mode==0)HAL_GPIO_WritePin(GPIO_PORT(name_),GPIO_PIN(name_),GPIO_PIN_RESET)
#define GPIO_Tgl(name_)        if(gpio_test_mode==0)HAL_GPIO_TogglePin(GPIO_PORT(name_),GPIO_PIN(name_))
#define GPIO_Ctrl(name_,ctrl_) if(gpio_test_mode==0)HAL_GPIO_WritePin(GPIO_PORT(name_),GPIO_PIN(name_),ctrl_)
#define GPIO_Get(name_)        HAL_GPIO_ReadPin(GPIO_PORT(name_),GPIO_PIN(name_))
#define GPIO_TestMode(val_)    gpio_test_mode = val_;


// Step pin pulses - not these are negated in hardware
#define GPIO_STEP_SET_A()	   GPIO_Clr_Unc(OUT_STEP4)
#define GPIO_STEP_SET_B()	   GPIO_Clr_Unc(OUT_STEP5)
#define GPIO_STEP_SET_C()	   GPIO_Clr_Unc(OUT_DIR6)  // connection mismatch
#define GPIO_STEP_SET_D()	   GPIO_Clr_Unc(OUT_STEP7)

#define GPIO_STEP_CLR_A()	   GPIO_Set_Unc(OUT_STEP4)
#define GPIO_STEP_CLR_B()	   GPIO_Set_Unc(OUT_STEP5)
#define GPIO_STEP_CLR_C()      GPIO_Set_Unc(OUT_DIR6)  // connection mismatch
#define GPIO_STEP_CLR_D()      GPIO_Set_Unc(OUT_STEP7)



volatile extern int32_t gpio_test_mode;


void      srv_gpio_init(void);
void      srv_gpio_once(void);
void  	  srv_gpio_refresh(void);
uint32_t  srv_gpio_get_address(void);


uint32_t  srv_gpio_get_io(void);
void      srv_gpio_set_io(uint32_t mask,uint32_t value);
void      srv_gpio_tick_io(void);
uint32_t  srv_gpio_dump(char * buffer, uint32_t buffer_size);

#endif  

