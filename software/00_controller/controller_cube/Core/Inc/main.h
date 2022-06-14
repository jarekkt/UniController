/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

#include "stm32h7xx_ll_usart.h"
#include "stm32h7xx_ll_rcc.h"
#include "stm32h7xx_ll_bus.h"
#include "stm32h7xx_ll_cortex.h"
#include "stm32h7xx_ll_system.h"
#include "stm32h7xx_ll_utils.h"
#include "stm32h7xx_ll_pwr.h"
#include "stm32h7xx_ll_gpio.h"
#include "stm32h7xx_ll_dma.h"

#include "stm32h7xx_ll_exti.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LWIP_SO_RCVTIMEO 1
#define IO_CPU1_Pin GPIO_PIN_3
#define IO_CPU1_GPIO_Port GPIOE
#define IN_CPU2_Pin GPIO_PIN_4
#define IN_CPU2_GPIO_Port GPIOE
#define IN_CPU1_Pin GPIO_PIN_5
#define IN_CPU1_GPIO_Port GPIOE
#define IN_CPU6_Pin GPIO_PIN_6
#define IN_CPU6_GPIO_Port GPIOE
#define IN_CPU6C13_Pin GPIO_PIN_13
#define IN_CPU6C13_GPIO_Port GPIOC
#define IN_CPU5_Pin GPIO_PIN_14
#define IN_CPU5_GPIO_Port GPIOC
#define IN_CPU4_Pin GPIO_PIN_15
#define IN_CPU4_GPIO_Port GPIOC
#define ADC_CH1_Pin GPIO_PIN_0
#define ADC_CH1_GPIO_Port GPIOC
#define IO_CPU3_Pin GPIO_PIN_2
#define IO_CPU3_GPIO_Port GPIOC
#define ADC_CH2_Pin GPIO_PIN_3
#define ADC_CH2_GPIO_Port GPIOC
#define ENET_RST_Pin GPIO_PIN_0
#define ENET_RST_GPIO_Port GPIOA
#define OUT_CPU3_PWM_Pin GPIO_PIN_3
#define OUT_CPU3_PWM_GPIO_Port GPIOA
#define DAC_CPU_Pin GPIO_PIN_4
#define DAC_CPU_GPIO_Port GPIOA
#define IN_CPU7_Pin GPIO_PIN_5
#define IN_CPU7_GPIO_Port GPIOA
#define ENC1_CH1_Pin GPIO_PIN_6
#define ENC1_CH1_GPIO_Port GPIOA
#define IN_CPU8_Pin GPIO_PIN_0
#define IN_CPU8_GPIO_Port GPIOB
#define IN_CPU9_Pin GPIO_PIN_1
#define IN_CPU9_GPIO_Port GPIOB
#define OUT_CPU9_Pin GPIO_PIN_7
#define OUT_CPU9_GPIO_Port GPIOE
#define OUT_CPU10_Pin GPIO_PIN_8
#define OUT_CPU10_GPIO_Port GPIOE
#define OUY_CPU1_PWM_Pin GPIO_PIN_9
#define OUY_CPU1_PWM_GPIO_Port GPIOE
#define OUT_DIR5_Pin GPIO_PIN_10
#define OUT_DIR5_GPIO_Port GPIOE
#define OUT_CPU2_PWM_Pin GPIO_PIN_11
#define OUT_CPU2_PWM_GPIO_Port GPIOE
#define OUT_STEP6_Pin GPIO_PIN_12
#define OUT_STEP6_GPIO_Port GPIOE
#define OUT_STEP4_Pin GPIO_PIN_13
#define OUT_STEP4_GPIO_Port GPIOE
#define OUT_CPU4_PWM_Pin GPIO_PIN_14
#define OUT_CPU4_PWM_GPIO_Port GPIOE
#define OUT_DIR6_Pin GPIO_PIN_15
#define OUT_DIR6_GPIO_Port GPIOE
#define IO_CPU9_Pin GPIO_PIN_15
#define IO_CPU9_GPIO_Port GPIOB
#define OUT_DIR3_Pin GPIO_PIN_8
#define OUT_DIR3_GPIO_Port GPIOD
#define OUT_CPU6_Pin GPIO_PIN_9
#define OUT_CPU6_GPIO_Port GPIOD
#define OUT_CPU5_Pin GPIO_PIN_10
#define OUT_CPU5_GPIO_Port GPIOD
#define OUT_DIR4_Pin GPIO_PIN_14
#define OUT_DIR4_GPIO_Port GPIOD
#define OUT_STEP5_Pin GPIO_PIN_15
#define OUT_STEP5_GPIO_Port GPIOD
#define OUT_STEP1_Pin GPIO_PIN_8
#define OUT_STEP1_GPIO_Port GPIOC
#define I2C3_SDA_Pin GPIO_PIN_9
#define I2C3_SDA_GPIO_Port GPIOC
#define I2C3_SCL_Pin GPIO_PIN_8
#define I2C3_SCL_GPIO_Port GPIOA
#define IO_CPU7_Pin GPIO_PIN_11
#define IO_CPU7_GPIO_Port GPIOA
#define OUT_CPU8_Pin GPIO_PIN_12
#define OUT_CPU8_GPIO_Port GPIOA
#define ENC3_PWM_CH1_Pin GPIO_PIN_15
#define ENC3_PWM_CH1_GPIO_Port GPIOA
#define LED_Pin GPIO_PIN_10
#define LED_GPIO_Port GPIOC
#define OUT_RS485_RE_Pin GPIO_PIN_11
#define OUT_RS485_RE_GPIO_Port GPIOC
#define IO_CPU4_Pin GPIO_PIN_12
#define IO_CPU4_GPIO_Port GPIOC
#define OUT_STEP7_Pin GPIO_PIN_2
#define OUT_STEP7_GPIO_Port GPIOD
#define OUT_CPU7_Pin GPIO_PIN_3
#define OUT_CPU7_GPIO_Port GPIOD
#define OUT_CPU8D4_Pin GPIO_PIN_4
#define OUT_CPU8D4_GPIO_Port GPIOD
#define OUT_DIR7_Pin GPIO_PIN_5
#define OUT_DIR7_GPIO_Port GPIOD
#define OUT_DIR1_Pin GPIO_PIN_6
#define OUT_DIR1_GPIO_Port GPIOD
#define OUT_DIR2_Pin GPIO_PIN_7
#define OUT_DIR2_GPIO_Port GPIOD
#define ENC_PWM_CH2_Pin GPIO_PIN_3
#define ENC_PWM_CH2_GPIO_Port GPIOB
#define IO_CPU6_Pin GPIO_PIN_4
#define IO_CPU6_GPIO_Port GPIOB
#define ENC1_CH2_Pin GPIO_PIN_5
#define ENC1_CH2_GPIO_Port GPIOB
#define ENC2_CH1_Pin GPIO_PIN_6
#define ENC2_CH1_GPIO_Port GPIOB
#define ENC2_CH2_Pin GPIO_PIN_7
#define ENC2_CH2_GPIO_Port GPIOB
#define OUT_STEP3_Pin GPIO_PIN_9
#define OUT_STEP3_GPIO_Port GPIOB
#define IO_CPU2_Pin GPIO_PIN_0
#define IO_CPU2_GPIO_Port GPIOE
#define IN_CPU10_Pin GPIO_PIN_1
#define IN_CPU10_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
