/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32h7xx_nucleo.h"

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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define OUT12_SPARE6_Pin GPIO_PIN_3
#define OUT12_SPARE6_GPIO_Port GPIOE
#define DIR_U_Pin GPIO_PIN_6
#define DIR_U_GPIO_Port GPIOE
#define IN6_SPARE2_Pin GPIO_PIN_6
#define IN6_SPARE2_GPIO_Port GPIOF
#define OUT11_SPARE5_Pin GPIO_PIN_8
#define OUT11_SPARE5_GPIO_Port GPIOF
#define OUT10_SPARE4_Pin GPIO_PIN_9
#define OUT10_SPARE4_GPIO_Port GPIOF
#define ANA2_Pin GPIO_PIN_0
#define ANA2_GPIO_Port GPIOC
#define IN3_Z_MIN_Pin GPIO_PIN_2
#define IN3_Z_MIN_GPIO_Port GPIOC
#define IN1_X_MIN_Pin GPIO_PIN_3
#define IN1_X_MIN_GPIO_Port GPIOC
#define OUT3_LED_UP_T2C1_Pin GPIO_PIN_0
#define OUT3_LED_UP_T2C1_GPIO_Port GPIOA
#define ANA1_Pin GPIO_PIN_3
#define ANA1_GPIO_Port GPIOA
#define OUT9_SPARE3_Pin GPIO_PIN_6
#define OUT9_SPARE3_GPIO_Port GPIOA
#define IN2_Y_MIN_Pin GPIO_PIN_1
#define IN2_Y_MIN_GPIO_Port GPIOB
#define IN5_SPARE1_Pin GPIO_PIN_11
#define IN5_SPARE1_GPIO_Port GPIOF
#define STEP_W_Pin GPIO_PIN_7
#define STEP_W_GPIO_Port GPIOE
#define DIR_W_Pin GPIO_PIN_8
#define DIR_W_GPIO_Port GPIOE
#define DIR_V_Pin GPIO_PIN_10
#define DIR_V_GPIO_Port GPIOE
#define OUT1_VALVE2_Pin GPIO_PIN_11
#define OUT1_VALVE2_GPIO_Port GPIOE
#define STEP_V_Pin GPIO_PIN_12
#define STEP_V_GPIO_Port GPIOE
#define OUT4_LED_DN_T1C3_Pin GPIO_PIN_13
#define OUT4_LED_DN_T1C3_GPIO_Port GPIOE
#define STEP_Z_Pin GPIO_PIN_14
#define STEP_Z_GPIO_Port GPIOE
#define STEP_U_Pin GPIO_PIN_15
#define STEP_U_GPIO_Port GPIOE
#define OUT6_PUMP_PRESS_T2C3_Pin GPIO_PIN_10
#define OUT6_PUMP_PRESS_T2C3_GPIO_Port GPIOB
#define OUT5_PUMP_VACUUM_T2C4_Pin GPIO_PIN_11
#define OUT5_PUMP_VACUUM_T2C4_GPIO_Port GPIOB
#define USB_OTG_FS_PWR_EN_Pin GPIO_PIN_10
#define USB_OTG_FS_PWR_EN_GPIO_Port GPIOD
#define ENC2_T4C1_Pin GPIO_PIN_12
#define ENC2_T4C1_GPIO_Port GPIOD
#define ENC2_T4C2_Pin GPIO_PIN_13
#define ENC2_T4C2_GPIO_Port GPIOD
#define OUT7_SAPRE1_Pin GPIO_PIN_14
#define OUT7_SAPRE1_GPIO_Port GPIOD
#define OUT8_SPARE2_Pin GPIO_PIN_15
#define OUT8_SPARE2_GPIO_Port GPIOD
#define OUT2_VALVE1_Pin GPIO_PIN_6
#define OUT2_VALVE1_GPIO_Port GPIOG
#define USB_OTG_FS_OVCR_Pin GPIO_PIN_7
#define USB_OTG_FS_OVCR_GPIO_Port GPIOG
#define ENC1_T3C1_Pin GPIO_PIN_6
#define ENC1_T3C1_GPIO_Port GPIOC
#define ENC1_T3C2_Pin GPIO_PIN_7
#define ENC1_T3C2_GPIO_Port GPIOC
#define CAN_RX_Pin GPIO_PIN_0
#define CAN_RX_GPIO_Port GPIOD
#define CAN_TX_Pin GPIO_PIN_1
#define CAN_TX_GPIO_Port GPIOD
#define DIR_Y_Pin GPIO_PIN_3
#define DIR_Y_GPIO_Port GPIOD
#define STEP_Y_Pin GPIO_PIN_4
#define STEP_Y_GPIO_Port GPIOD
#define TX_UVZ_Pin GPIO_PIN_5
#define TX_UVZ_GPIO_Port GPIOD
#define RX_UVZ_Pin GPIO_PIN_6
#define RX_UVZ_GPIO_Port GPIOD
#define IN4_ESTOP_Pin GPIO_PIN_7
#define IN4_ESTOP_GPIO_Port GPIOD
#define DIR_Z_Pin GPIO_PIN_12
#define DIR_Z_GPIO_Port GPIOG
#define TMC_ENA_Pin GPIO_PIN_14
#define TMC_ENA_GPIO_Port GPIOG
#define TX_XYW_Pin GPIO_PIN_6
#define TX_XYW_GPIO_Port GPIOB
#define RX_XYW_Pin GPIO_PIN_7
#define RX_XYW_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
