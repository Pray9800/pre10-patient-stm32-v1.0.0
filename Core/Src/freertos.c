/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "FreeRTOS.h"
#include "cmsis_os2.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for comTask */
osThreadId_t comTaskHandle;
const osThreadAttr_t comTask_attributes = {
  .name = "comTask",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for keyTask */
osThreadId_t keyTaskHandle;
const osThreadAttr_t keyTask_attributes = {
  .name = "keyTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityLow1,
};
/* Definitions for torqueMoveTask */
osThreadId_t torqueMoveTaskHandle;
const osThreadAttr_t torqueMoveTask_attributes = {
  .name = "torqueMoveTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for armTask */
osThreadId_t armTaskHandle;
const osThreadAttr_t armTask_attributes = {
  .name = "armTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for parkTask */
osThreadId_t parkTaskHandle;
const osThreadAttr_t parkTask_attributes = {
  .name = "parkTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for upsTask */
osThreadId_t upsTaskHandle;
const osThreadAttr_t upsTask_attributes = {
  .name = "upsTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for breakTask */
osThreadId_t breakTaskHandle;
const osThreadAttr_t breakTask_attributes = {
  .name = "breakTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal7,
};
/* Definitions for printfMutex */
osMutexId_t printfMutexHandle;
const osMutexAttr_t printfMutex_attributes = {
  .name = "printfMutex"
};
/* Definitions for keyEvent */
osEventFlagsId_t keyEventHandle;
const osEventFlagsAttr_t keyEvent_attributes = {
  .name = "keyEvent"
};
/* Definitions for comEvent */
osEventFlagsId_t comEventHandle;
const osEventFlagsAttr_t comEvent_attributes = {
  .name = "comEvent"
};
/* Definitions for ArmBKEvent */
osEventFlagsId_t ArmBKEventHandle;
const osEventFlagsAttr_t ArmBKEvent_attributes = {
  .name = "ArmBKEvent"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartComTask(void *argument);
void StartKeyTask(void *argument);
void StartTorqueMove(void *argument);
void StartArmTask(void *argument);
void StartParkTask(void *argument);
void StartUpsTask(void *argument);
void StartBreakTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
__weak void configureTimerForRunTimeStats(void)
{

}

__weak unsigned long getRunTimeCounterValue(void)
{
return 0;
}
/* USER CODE END 1 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of printfMutex */
  printfMutexHandle = osMutexNew(&printfMutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of comTask */
  comTaskHandle = osThreadNew(StartComTask, NULL, &comTask_attributes);

  /* creation of keyTask */
  keyTaskHandle = osThreadNew(StartKeyTask, NULL, &keyTask_attributes);

  /* creation of torqueMoveTask */
  torqueMoveTaskHandle = osThreadNew(StartTorqueMove, NULL, &torqueMoveTask_attributes);

  /* creation of armTask */
  armTaskHandle = osThreadNew(StartArmTask, NULL, &armTask_attributes);

  /* creation of parkTask */
  parkTaskHandle = osThreadNew(StartParkTask, NULL, &parkTask_attributes);

  /* creation of upsTask */
  upsTaskHandle = osThreadNew(StartUpsTask, NULL, &upsTask_attributes);

  /* creation of breakTask */
  breakTaskHandle = osThreadNew(StartBreakTask, NULL, &breakTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* creation of keyEvent */
  keyEventHandle = osEventFlagsNew(&keyEvent_attributes);

  /* creation of comEvent */
  comEventHandle = osEventFlagsNew(&comEvent_attributes);

  /* creation of ArmBKEvent */
  ArmBKEventHandle = osEventFlagsNew(&ArmBKEvent_attributes);

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
__weak void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartComTask */
/**
* @brief Function implementing the comTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartComTask */
__weak void StartComTask(void *argument)
{
  /* USER CODE BEGIN StartComTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartComTask */
}

/* USER CODE BEGIN Header_StartKeyTask */
/**
* @brief Function implementing the keyTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartKeyTask */
__weak void StartKeyTask(void *argument)
{
  /* USER CODE BEGIN StartKeyTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartKeyTask */
}

/* USER CODE BEGIN Header_StartTorqueMove */
/**
* @brief Function implementing the torqueMoveTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTorqueMove */
__weak void StartTorqueMove(void *argument)
{
  /* USER CODE BEGIN StartTorqueMove */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartTorqueMove */
}

/* USER CODE BEGIN Header_StartArmTask */
/**
* @brief Function implementing the armTask05 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartArmTask */
__weak void StartArmTask(void *argument)
{
  /* USER CODE BEGIN StartArmTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartArmTask */
}

/* USER CODE BEGIN Header_StartParkTask */
/**
* @brief Function implementing the parkTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartParkTask */
__weak void StartParkTask(void *argument)
{
  /* USER CODE BEGIN StartParkTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartParkTask */
}

/* USER CODE BEGIN Header_StartUpsTask */
/**
* @brief Function implementing the upsTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartUpsTask */
__weak void StartUpsTask(void *argument)
{
  /* USER CODE BEGIN StartUpsTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartUpsTask */
}

/* USER CODE BEGIN Header_StartBreakTask */
/**
* @brief Function implementing the breakTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartBreakTask */
__weak void StartBreakTask(void *argument)
{
  /* USER CODE BEGIN StartBreakTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartBreakTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

