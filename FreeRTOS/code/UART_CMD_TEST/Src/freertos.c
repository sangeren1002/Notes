/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2020 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#include "app.h"

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
volatile unsigned long  ulHighFrequencyTimerTicks;
osMutexId U2PrintfMutexHandle;
/* USER CODE END Variables */
osThreadId SysInitTaskHandle;
osThreadId DebugCmdTaskHandle;
osThreadId LedRunTaskHandle;
osThreadId Upload2PCTaskHandle;
osMessageQId U1RecvQueueHandle;
osMessageQId U2RecvQueueHandle;
osMessageQId U2SendQueueHandle;
osMessageQId U1SendQueueHandle;
osMutexId PrintfMutexHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
   
/* USER CODE END FunctionPrototypes */

void StartSysInitTask(void const * argument);
void StartDebugCmdTask(void const * argument);
void StartLedRunTask(void const * argument);
void StartUpload2PCTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
__weak void configureTimerForRunTimeStats(void)
{
	ulHighFrequencyTimerTicks = 0ul;
}

__weak unsigned long getRunTimeCounterValue(void)
{
	return ulHighFrequencyTimerTicks;
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
  /* definition and creation of PrintfMutex */
  osMutexDef(PrintfMutex);
  PrintfMutexHandle = osMutexCreate(osMutex(PrintfMutex));

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
	  osMutexDef(U2PrintfMutex);
  U2PrintfMutexHandle = osMutexCreate(osMutex(U2PrintfMutex));
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of SysInitTask */
  osThreadDef(SysInitTask, StartSysInitTask, osPriorityNormal, 0, 512);
  SysInitTaskHandle = osThreadCreate(osThread(SysInitTask), NULL);

  /* definition and creation of DebugCmdTask */
//  osThreadDef(DebugCmdTask, StartDebugCmdTask, osPriorityAboveNormal, 0, 512);
//  DebugCmdTaskHandle = osThreadCreate(osThread(DebugCmdTask), NULL);

//  /* definition and creation of LedRunTask */
//  osThreadDef(LedRunTask, StartLedRunTask, osPriorityNormal, 0, 128);
//  LedRunTaskHandle = osThreadCreate(osThread(LedRunTask), NULL);

//  /* definition and creation of Upload2PCTask */
//  osThreadDef(Upload2PCTask, StartUpload2PCTask, osPriorityAboveNormal, 0, 256);
//  Upload2PCTaskHandle = osThreadCreate(osThread(Upload2PCTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Create the queue(s) */
  /* definition and creation of U1RecvQueue */
/* what about the sizeof here??? cd native code */
  osMessageQDef(U1RecvQueue, 16, uint32_t);
  U1RecvQueueHandle = osMessageCreate(osMessageQ(U1RecvQueue), NULL);

  /* definition and creation of U2RecvQueue */
/* what about the sizeof here??? cd native code */
  osMessageQDef(U2RecvQueue, 16, uint32_t);
  U2RecvQueueHandle = osMessageCreate(osMessageQ(U2RecvQueue), NULL);

  /* definition and creation of U2SendQueue */
/* what about the sizeof here??? cd native code */
  osMessageQDef(U2SendQueue, 16, uint32_t);
  U2SendQueueHandle = osMessageCreate(osMessageQ(U2SendQueue), NULL);

  /* definition and creation of U1SendQueue */
/* what about the sizeof here??? cd native code */
  osMessageQDef(U1SendQueue, 16, uint32_t);
  U1SendQueueHandle = osMessageCreate(osMessageQ(U1SendQueue), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}

/* USER CODE BEGIN Header_StartSysInitTask */
/**
  * @brief  Function implementing the SysInitTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartSysInitTask */
void StartSysInitTask(void const * argument)
{

  /* USER CODE BEGIN StartSysInitTask */
  /* Infinite loop */
  for(;;)
  {
    ProcessSysInit();
  }
  /* USER CODE END StartSysInitTask */
}

/* USER CODE BEGIN Header_StartDebugCmdTask */
/**
* @brief Function implementing the DebugCmdTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDebugCmdTask */
void StartDebugCmdTask(void const * argument)
{
  /* USER CODE BEGIN StartDebugCmdTask */
  /* Infinite loop */
  for(;;)
  {
    ProcessDebugCmdTask();
  }
  /* USER CODE END StartDebugCmdTask */
}

/* USER CODE BEGIN Header_StartLedRunTask */
/**
* @brief Function implementing the LedRunTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartLedRunTask */
void StartLedRunTask(void const * argument)
{
  /* USER CODE BEGIN StartLedRunTask */
  /* Infinite loop */
  for(;;)
  {
	ProcessLedRun();
  }
  /* USER CODE END StartLedRunTask */
}

/* USER CODE BEGIN Header_StartUpload2PCTask */
/**
* @brief Function implementing the Upload2PCTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartUpload2PCTask */
void StartUpload2PCTask(void const * argument)
{
  /* USER CODE BEGIN StartUpload2PCTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartUpload2PCTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
