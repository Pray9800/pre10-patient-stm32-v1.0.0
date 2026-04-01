/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#define PS4_Pin GPIO_PIN_5
#define PS4_GPIO_Port GPIOA
#define PS3_Pin GPIO_PIN_6
#define PS3_GPIO_Port GPIOA
#define PS2_Pin GPIO_PIN_7
#define PS2_GPIO_Port GPIOA
#define PS1_Pin GPIO_PIN_4
#define PS1_GPIO_Port GPIOC
#define KEY8_Pin GPIO_PIN_7
#define KEY8_GPIO_Port GPIOE
#define KEY7_Pin GPIO_PIN_8
#define KEY7_GPIO_Port GPIOE
#define KEY6_Pin GPIO_PIN_9
#define KEY6_GPIO_Port GPIOE
#define KEY5_Pin GPIO_PIN_10
#define KEY5_GPIO_Port GPIOE
#define KEY4_Pin GPIO_PIN_11
#define KEY4_GPIO_Port GPIOE
#define KEY3_Pin GPIO_PIN_12
#define KEY3_GPIO_Port GPIOE
#define KEY2_Pin GPIO_PIN_13
#define KEY2_GPIO_Port GPIOE
#define KEY1_Pin GPIO_PIN_14
#define KEY1_GPIO_Port GPIOE
#define LED0_Pin GPIO_PIN_14
#define LED0_GPIO_Port GPIOB
#define ETH_TX_Pin GPIO_PIN_6
#define ETH_TX_GPIO_Port GPIOC
#define ETH_RX_Pin GPIO_PIN_7
#define ETH_RX_GPIO_Port GPIOC
#define DEBUG_TX_Pin GPIO_PIN_9
#define DEBUG_TX_GPIO_Port GPIOA
#define DEBUG_RX_Pin GPIO_PIN_10
#define DEBUG_RX_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
