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
#define IN_CPU1_Pin GPIO_PIN_7
#define IN_CPU1_GPIO_Port GPIOE
#define IN_CPU2_Pin GPIO_PIN_8
#define IN_CPU2_GPIO_Port GPIOE
#define IN_CPU3_Pin GPIO_PIN_10
#define IN_CPU3_GPIO_Port GPIOE
#define IN_CPU4_Pin GPIO_PIN_12
#define IN_CPU4_GPIO_Port GPIOE
#define MID_IO1_Pin GPIO_PIN_14
#define MID_IO1_GPIO_Port GPIOE
#define MID_IO2_Pin GPIO_PIN_15
#define MID_IO2_GPIO_Port GPIOE
#define OUT_DIR3_Pin GPIO_PIN_8
#define OUT_DIR3_GPIO_Port GPIOD
#define OUT_CPU4_Pin GPIO_PIN_9
#define OUT_CPU4_GPIO_Port GPIOD
#define OUT_CPU3_Pin GPIO_PIN_10
#define OUT_CPU3_GPIO_Port GPIOD
#define OUT_CPU2_Pin GPIO_PIN_11
#define OUT_CPU2_GPIO_Port GPIOD
#define OUT_CPU1_Pin GPIO_PIN_3
#define OUT_CPU1_GPIO_Port GPIOD
#define OUT_DIR1_Pin GPIO_PIN_6
#define OUT_DIR1_GPIO_Port GPIOD
#define OUT_DIR2_Pin GPIO_PIN_7
#define OUT_DIR2_GPIO_Port GPIOD
#define IO_LED_Pin GPIO_PIN_0
#define IO_LED_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
