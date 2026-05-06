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
#define ADC_ArmHeight_Pin GPIO_PIN_4
#define ADC_ArmHeight_GPIO_Port GPIOA
#define PS4_Pin GPIO_PIN_5
#define PS4_GPIO_Port GPIOA
#define PS3_Pin GPIO_PIN_6
#define PS3_GPIO_Port GPIOA
#define PS2_Pin GPIO_PIN_7
#define PS2_GPIO_Port GPIOA
#define PS1_Pin GPIO_PIN_4
#define PS1_GPIO_Port GPIOC
#define STOP_Pin GPIO_PIN_5
#define STOP_GPIO_Port GPIOC
#define KEY11_Pin GPIO_PIN_0
#define KEY11_GPIO_Port GPIOB
#define KEY10_Pin GPIO_PIN_1
#define KEY10_GPIO_Port GPIOB
#define KEY9_Pin GPIO_PIN_2
#define KEY9_GPIO_Port GPIOB
#define KEY5_Pin GPIO_PIN_10
#define KEY5_GPIO_Port GPIOE
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
#define FOOTP_0_Pin GPIO_PIN_0
#define FOOTP_0_GPIO_Port GPIOD
#define FOOTP_1_Pin GPIO_PIN_1
#define FOOTP_1_GPIO_Port GPIOD
#define FOOTP_2_Pin GPIO_PIN_3
#define FOOTP_2_GPIO_Port GPIOD
#define FOOTP_3_Pin GPIO_PIN_4
#define FOOTP_3_GPIO_Port GPIOD
#define UPS_OFF_Pin GPIO_PIN_5
#define UPS_OFF_GPIO_Port GPIOD
#define UPS_ON_Pin GPIO_PIN_6
#define UPS_ON_GPIO_Port GPIOD
#define CTL_M1_0_Pin GPIO_PIN_7
#define CTL_M1_0_GPIO_Port GPIOD
#define CTL_M1_1_Pin GPIO_PIN_3
#define CTL_M1_1_GPIO_Port GPIOB
#define CTL_M2_0_Pin GPIO_PIN_4
#define CTL_M2_0_GPIO_Port GPIOB
#define CTL_M2_1_Pin GPIO_PIN_5
#define CTL_M2_1_GPIO_Port GPIOB
#define CTL_M3_0_Pin GPIO_PIN_6
#define CTL_M3_0_GPIO_Port GPIOB
#define CTL_M3_1_Pin GPIO_PIN_7
#define CTL_M3_1_GPIO_Port GPIOB
#define UPDOWN_0_Pin GPIO_PIN_8
#define UPDOWN_0_GPIO_Port GPIOB
#define UPDOWN_1_Pin GPIO_PIN_9
#define UPDOWN_1_GPIO_Port GPIOB
#define CTL_BK_0_Pin GPIO_PIN_0
#define CTL_BK_0_GPIO_Port GPIOE
#define CTL_BK_1_Pin GPIO_PIN_1
#define CTL_BK_1_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
