/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins
     PH0-OSC_IN (PH0)   ------> RCC_OSC_IN
     PH1-OSC_OUT (PH1)   ------> RCC_OSC_OUT
     PA13 (JTMS/SWDIO)   ------> DEBUG_JTMS-SWDIO
     PA14 (JTCK/SWCLK)   ------> DEBUG_JTCK-SWCLK
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, KEY2_Pin|CTL_BK_0_Pin|CTL_BK_1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED0_Pin|CTL_M1_1_Pin|CTL_M2_0_Pin|CTL_M2_1_Pin
                          |CTL_M3_0_Pin|CTL_M3_1_Pin|UPDOWN_0_Pin|UPDOWN_1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, FOOTP_0_Pin|FOOTP_1_Pin|FOOTP_2_Pin|FOOTP_3_Pin
                          |UPS_OFF_Pin|UPS_ON_Pin|CTL_M1_0_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PS4_Pin PS3_Pin PS2_Pin */
  GPIO_InitStruct.Pin = PS4_Pin|PS3_Pin|PS2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PS1_Pin */
  GPIO_InitStruct.Pin = PS1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(PS1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : STOP_Pin */
  GPIO_InitStruct.Pin = STOP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(STOP_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : KEY11_Pin KEY10_Pin KEY9_Pin */
  GPIO_InitStruct.Pin = KEY11_Pin|KEY10_Pin|KEY9_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : KEY8_Pin KEY7_Pin KEY6_Pin KEY5_Pin
                           KEY4_Pin KEY3_Pin */
  GPIO_InitStruct.Pin = KEY8_Pin|KEY7_Pin|KEY6_Pin|KEY5_Pin
                          |KEY4_Pin|KEY3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : KEY2_Pin */
  GPIO_InitStruct.Pin = KEY2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(KEY2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : KEY1_Pin */
  GPIO_InitStruct.Pin = KEY1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(KEY1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LED0_Pin */
  GPIO_InitStruct.Pin = LED0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED0_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : FOOTP_0_Pin FOOTP_1_Pin FOOTP_2_Pin FOOTP_3_Pin
                           UPS_OFF_Pin UPS_ON_Pin CTL_M1_0_Pin */
  GPIO_InitStruct.Pin = FOOTP_0_Pin|FOOTP_1_Pin|FOOTP_2_Pin|FOOTP_3_Pin
                          |UPS_OFF_Pin|UPS_ON_Pin|CTL_M1_0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : CTL_M1_1_Pin CTL_M2_0_Pin CTL_M2_1_Pin CTL_M3_0_Pin
                           CTL_M3_1_Pin UPDOWN_0_Pin UPDOWN_1_Pin */
  GPIO_InitStruct.Pin = CTL_M1_1_Pin|CTL_M2_0_Pin|CTL_M2_1_Pin|CTL_M3_0_Pin
                          |CTL_M3_1_Pin|UPDOWN_0_Pin|UPDOWN_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : CTL_BK_0_Pin CTL_BK_1_Pin */
  GPIO_InitStruct.Pin = CTL_BK_0_Pin|CTL_BK_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
