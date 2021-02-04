/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#include "stm32h7xx.h"
#include "stm32h7xx_ll_system.h"
#include "stm32h7xx_ll_gpio.h"
#include "stm32h7xx_ll_exti.h"
#include "stm32h7xx_ll_bus.h"
#include "stm32h7xx_ll_cortex.h"
#include "stm32h7xx_ll_utils.h"
#include "stm32h7xx_ll_pwr.h"
#include "stm32h7xx_ll_dma.h"

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
#define WDG_KICK_Pin LL_GPIO_PIN_2
#define WDG_KICK_GPIO_Port GPIOE
#define ADDR2_Pin LL_GPIO_PIN_3
#define ADDR2_GPIO_Port GPIOE
#define ADDR3_Pin LL_GPIO_PIN_4
#define ADDR3_GPIO_Port GPIOE
#define OUT3_Pin LL_GPIO_PIN_0
#define OUT3_GPIO_Port GPIOC
#define OUT4_Pin LL_GPIO_PIN_1
#define OUT4_GPIO_Port GPIOC
#define ADC_S2_Pin LL_GPIO_PIN_3
#define ADC_S2_GPIO_Port GPIOC
#define IN1_Pin LL_GPIO_PIN_0
#define IN1_GPIO_Port GPIOA
#define IN2_Pin LL_GPIO_PIN_1
#define IN2_GPIO_Port GPIOA
#define IN3_Pin LL_GPIO_PIN_2
#define IN3_GPIO_Port GPIOA
#define IN4_Pin LL_GPIO_PIN_3
#define IN4_GPIO_Port GPIOA
#define IN_ADC1_Pin LL_GPIO_PIN_4
#define IN_ADC1_GPIO_Port GPIOA
#define IN_ADC2_Pin LL_GPIO_PIN_5
#define IN_ADC2_GPIO_Port GPIOA
#define IN_ADC3_Pin LL_GPIO_PIN_7
#define IN_ADC3_GPIO_Port GPIOA
#define IN_ADC4_Pin LL_GPIO_PIN_4
#define IN_ADC4_GPIO_Port GPIOC
#define ADC_S1_Pin LL_GPIO_PIN_5
#define ADC_S1_GPIO_Port GPIOC
#define OUT1_Pin LL_GPIO_PIN_0
#define OUT1_GPIO_Port GPIOB
#define OUT2_Pin LL_GPIO_PIN_1
#define OUT2_GPIO_Port GPIOB
#define LED_Pin LL_GPIO_PIN_12
#define LED_GPIO_Port GPIOE
#define REVISION_S1_Pin LL_GPIO_PIN_14
#define REVISION_S1_GPIO_Port GPIOE
#define REVISION_S2_Pin LL_GPIO_PIN_15
#define REVISION_S2_GPIO_Port GPIOE
#define USART3_CTRL_Pin LL_GPIO_PIN_12
#define USART3_CTRL_GPIO_Port GPIOB
#define LIGHT_ON_Pin LL_GPIO_PIN_8
#define LIGHT_ON_GPIO_Port GPIOD
#define OUT_EXT2_Pin LL_GPIO_PIN_10
#define OUT_EXT2_GPIO_Port GPIOD
#define OUT_EXT4_Pin LL_GPIO_PIN_12
#define OUT_EXT4_GPIO_Port GPIOD
#define OUT_EXT6_Pin LL_GPIO_PIN_14
#define OUT_EXT6_GPIO_Port GPIOD
#define CAN1_ENA_Pin LL_GPIO_PIN_9
#define CAN1_ENA_GPIO_Port GPIOC
#define ETH_INT_Pin LL_GPIO_PIN_10
#define ETH_INT_GPIO_Port GPIOC
#define ETH_RST_Pin LL_GPIO_PIN_3
#define ETH_RST_GPIO_Port GPIOD
#define USART2_CTRL_Pin LL_GPIO_PIN_7
#define USART2_CTRL_GPIO_Port GPIOD
#define ETH_CS_Pin LL_GPIO_PIN_4
#define ETH_CS_GPIO_Port GPIOB
#define I2C1_SCL_Pin LL_GPIO_PIN_8
#define I2C1_SCL_GPIO_Port GPIOB
#define I2C1_SDA_Pin LL_GPIO_PIN_9
#define I2C1_SDA_GPIO_Port GPIOB
#define ADDR0_Pin LL_GPIO_PIN_0
#define ADDR0_GPIO_Port GPIOE
#define ADDR1_Pin LL_GPIO_PIN_1
#define ADDR1_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
