# cubemx配置freertos的消息队列
>消息队列的概念及其作用
消息队列就是通过 RTOS 内核提供的服务，任务或中断服务子程序可以将一个消息（注意，FreeRTOS
消息队列传递的是实际数据，并不是数据地址，RTX，uCOS-II 和 uCOS-III 是传递的地址）放入到队列。
同样，一个或者多个任务可以通过 RTOS 内核服务从队列中得到消息。通常，先进入消息队列的消息先传
给任务，也就是说，任务先得到的是最先进入到消息队列的消息，即先进先出的原则（FIFO），FreeRTOS
的消息队列支持 FIFO 和 LIFO 两种数据存取方式。

*需要说明的是，freertos消息队列是通过副本机制传递的，而不是引用。*

![xQueueSend](https://github.com/sangeren1002/Notes/blob/master/FreeRTOS/image/freertos_queue_1_cubemx.png?raw=true)
查看源码可以看到底层实现`xQueueSend`函数的宏定义如下
```
#define xQueueSend( xQueue, pvItemToQueue, xTicksToWait ) xQueueGenericSend( ( xQueue ), ( pvItemToQueue ), ( xTicksToWait ), queueSEND_TO_BACK )

```
`xQueueGenericSend()`函数内有这个函数
```
xYieldRequired = prvCopyDataToQueue( pxQueue, pvItemToQueue, xCopyPosition );
```
`prvCopyDataToQueue()`函数内可以看到
```
( void ) memcpy( ( void * ) pxQueue->pcWriteTo, pvItemToQueue, ( size_t ) pxQueue->uxItemSize ); /*lint !e961 !e418 MISRA exception as the casts are only redundant for some ports, plus previous logic ensures a null pointer can only be passed to memcpy() if the copy size is 0. */
```
*freertos的消息队列发送函数是通过使用memcpy复制的内容。*
以简单的数据元素为例：
```
uint8_t ucCount = 0;

xQueueSend(xQueue1,(void *) &ucCount,(TickType_t)10);
```

这里是发送队列消息函数，下面看接收：
```
uint8_t ucQueueMsgValue;

xQueueReceive(xQueue1, /* 消息队列句柄 */
(void *)&ucQueueMsgValue, /* 存储接收到的数据到变量ucQueueMsgValue中 */
(TickType_t)xMaxBlockTime)
```

这里是最简单的uint_8类型元素，要想把发送函数的uint_8定义的数据，包括该数据在发送函数之前被更改后的值发送给接收函数，我们需要传递给发送函数send一个uint_8定义数据的地址，这样可以通过地址传递到memcpy函数，实现复制，这也就是为什么上面说的freertos的消息队列不是引用而是复制，要是引用的话，可以直接传这个uint_8类型的数据，而我们此时在freertos操作系统上，是副本传递，通过memcpy，所以需要给uint_8类型数据的地址。
同样的在本次实验中采用了消息队列传结构体指针地址来实现大数据量的传输提高效率。
```
/* 发送函数 */
MSG * TXMSG;
TXMSG = &DATAMSG;//DATAMSG是一个结构实体而且是全局区定义

	/* 初始化结构体内容 */
	TXMSG->ucMessageID = 1;
	TXMSG->ucData[0] = 0;
/* 消息队列传递结构体指针的地址 */
  if(xQueueSend(myQueue02Handle,(void*)&TXMSG,0) ==errQUEUE_FULL)
		{
			printf("myQueue02Handle errQUEUE_FULL\r\n");
		}
```
```
/* 接收函数 */
  MSG *RXMSG;
	/* 初始化结构体内容 */
	TXMSG->ucMessageID = 1;
	TXMSG->ucData[0] = 0;
/* 接收消息队列结构体指针的地址 */
if(xQueueReceive( myQueue02Handle, (void*)&RXMSG, 10) == pdPASS)
{
  printf("\r\nRXMSG->ucMessageID = %d \r",RXMSG->ucMessageID);
  printf("RXMSG->ucData[0] = ");
  for(i=0;i<20;i++)
    printf(" %03d",RXMSG->ucData[i]);
  printf("\r\n");
  HAL_GPIO_TogglePin(LED0_GPIO_Port,LED0_Pin);
}
```
这里的关键就在第二个参数ptMsg，它已经是指针了，为什么还要取地址，这样不是一个二级指针了吗，而它的参数是`void *`，给人的感觉应该就是传一个地址，虽然二级指针也是地址，但是总觉得不应该设计成二级指针赋值给一个一级指针，哪怕你是void*。但是我们既然使用了freertos，就要遵循别人的设计，别人这样做，肯定有自己的道理，我们做到熟练应用即可。试想，消息发送函数，要发送数据，要得到这个数据的地址以给memcopy，如果传递的数据本身就是地址（指针），那么我们要把这个地址传到接收函数去，就应该得到此时指针的地址才行，也就是传递一个指针的值，注意不是指针指向的值。关键我们要通过memcpy函数，传递一个指针的值通过memcpy必然是需要二级指针的，这样才可以操作一级指针的值，这样也就可以理解为什么ptMsg已经是指针了，却还是要传递ptMsg的地址，因为只有这样，才可以通过memcpy函数把ptMsg指针的值给到接收函数的指针，这样在接收函数中操作这个结构体类型的指针，就可以得到发送端的数据。这样做的好处是，避免了大数据的拷贝，只拷贝指针，提高了效率，但是使用指针，一定不要在栈空间开辟，这也是为什么我们定义g_tMsg结构体实体在全局区。但是freertos任务中一直有while（1），元素生命周期一直都在，此时还是可以使用局部变量做数据传递工具，但是这样的编程模式应该摒弃，我们采用全局区开辟的空间。

## 1. 环境
- packages版本（STM32F4 1.21）
- cubemx版本（version4.27.0 && STM32Cube v1.0）
- MDK版本（KEIL6 V5.23.0.0）

![packages版本](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/I2C/packages.png?raw=true)
![cubemx版本](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/I2C/cubemx_ver.png?raw=true)
![KEIL5版本](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/I2C/MDK%E7%89%88%E6%9C%AC.png?raw=true)
### 2. cubemx配置
>主要内容：
1. 配置时钟
2. 配置串口
3. 使能freertos
4. 勾选任务信息相关宏定义（3个，方便查看任务信息）
5. 添加消息队列

STM32F07开发板实验，关键代码实现
```
/* cubemx自动生成的创建2个消息队列 */
/* 创建10个uint8_t型消息队列 */
  osMessageQDef(myQueue01, 10, uint8_t);
  myQueue01Handle = osMessageCreate(osMessageQ(myQueue01), NULL);

/* 创建10个存储指针变量的消息队列 */
  osMessageQDef(myQueue02, 1, MSG*);
  myQueue02Handle = osMessageCreate(osMessageQ(myQueue02), NULL);
```
freertos.c源文件
```
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
  * Copyright (c) 2019 STMicroelectronics International N.V.
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
#include "bsp_usart.h"
#include "bsp_gpio.h"
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
volatile unsigned long  ulHighFrequencyTimerTicks = 0ul;
typedef struct AMessage
{
    char ucMessageID;
    char ucData[ 20 ];
}MSG;

MSG DATAMSG;
/* USER CODE END Variables */
osThreadId PrintfTaskHandle;
osThreadId myTask02Handle;
osThreadId myTask03Handle;
osMessageQId myQueue01Handle;
osMessageQId myQueue02Handle;
osMessageQId myQueue03Handle;
osMessageQId myQueue04Handle;
osMessageQId myQueue05Handle;
osTimerId myTimer01Handle;
osTimerId myTimer02Handle;
osMutexId myMutex01Handle;
osSemaphoreId myBinarySem01Handle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
   void Show_SYS_INFO_Task(void);
/* USER CODE END FunctionPrototypes */

void StartPrintfTask(void const * argument);
void StartTask02(void const * argument);
void StartTask03(void const * argument);
void Callback01(void const * argument);
void Callback02(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);
uint8_t buff[50] ={0x01,0x02,0x03} ;
/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
void configureTimerForRunTimeStats(void)
{
	ulHighFrequencyTimerTicks = 0ul;
}

unsigned long getRunTimeCounterValue(void)
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
  /* definition and creation of myMutex01 */
  osMutexDef(myMutex01);
  myMutex01Handle = osMutexCreate(osMutex(myMutex01));

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of myBinarySem01 */
  osSemaphoreDef(myBinarySem01);
  myBinarySem01Handle = osSemaphoreCreate(osSemaphore(myBinarySem01), 1);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* definition and creation of myTimer01 */
  osTimerDef(myTimer01, Callback01);
  myTimer01Handle = osTimerCreate(osTimer(myTimer01), osTimerPeriodic, NULL);

  /* definition and creation of myTimer02 */
  osTimerDef(myTimer02, Callback02);
  myTimer02Handle = osTimerCreate(osTimer(myTimer02), osTimerPeriodic, NULL);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of PrintfTask */
  osThreadDef(PrintfTask, StartPrintfTask, osPriorityNormal, 0, 512);
  PrintfTaskHandle = osThreadCreate(osThread(PrintfTask), NULL);

  /* definition and creation of myTask02 */
  osThreadDef(myTask02, StartTask02, osPriorityNormal, 0, 256);
  myTask02Handle = osThreadCreate(osThread(myTask02), NULL);

  /* definition and creation of myTask03 */
  osThreadDef(myTask03, StartTask03, osPriorityNormal, 0, 256);
  myTask03Handle = osThreadCreate(osThread(myTask03), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Create the queue(s) */
  /* definition and creation of myQueue01 */
/* what about the sizeof here??? cd native code */
  osMessageQDef(myQueue01, 10, uint8_t);
  myQueue01Handle = osMessageCreate(osMessageQ(myQueue01), NULL);
//	myQueue01Handle = xQueueCreate(10,4);
  /* definition and creation of myQueue02 */
/* what about the sizeof here??? cd native code */
  osMessageQDef(myQueue02, 1, MSG*);
  myQueue02Handle = osMessageCreate(osMessageQ(myQueue02), NULL);
////	myQueue02Handle = xQueueCreate(10,sizeof(struct AMessage*));
//	myQueue02Handle = xQueueCreate(10,sizeof(MSG *));

//  /* definition and creation of myQueue03 */
///* what about the sizeof here??? cd native code */
//  osMessageQDef(myQueue03, 32, uint8_t);
//  myQueue03Handle = osMessageCreate(osMessageQ(myQueue03), NULL);

//  /* definition and creation of myQueue04 */
///* what about the sizeof here??? cd native code */
//  osMessageQDef(myQueue04, 32, uint8_t);
//  myQueue04Handle = osMessageCreate(osMessageQ(myQueue04), NULL);

//  /* definition and creation of myQueue05 */
///* what about the sizeof here??? cd native code */
//  osMessageQDef(myQueue05, 32, uint8_t);
//  myQueue05Handle = osMessageCreate(osMessageQ(myQueue05), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}

/* USER CODE BEGIN Header_StartPrintfTask */
/**
  * @brief  Function implementing the PrintfTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartPrintfTask */
void StartPrintfTask(void const * argument)
{

  /* USER CODE BEGIN StartPrintfTask */

	MSG * TXMSG;
	TXMSG = &DATAMSG;
	uint8_t j = 0;
	uint8_t i;

	TXMSG->ucMessageID = 1;
	TXMSG->ucData[0] = 0;
  /* Infinite loop */
  for(;;)
  {  	
	if(key0_value == KEY0_PRES)
	{
		osSemaphoreRelease(myBinarySem01Handle);
		key0_value = 0;
	}
	if(key1_value == KEY1_PRES)
	{
		TXMSG->ucMessageID++;
		for(i=0;i<20;i++,j++)
		{
			TXMSG->ucData[i]=j;
		}
		if(xQueueSend(myQueue02Handle,(void*)&TXMSG,0) ==errQUEUE_FULL)
		{
			printf("myQueue02Handle errQUEUE_FULL\r\n");
		}
		key1_value = 0;
	}
	if(key2_value == KEY2_PRES)
	{
		if(xQueueSend(myQueue01Handle,(void*)&key2_value,10) == errQUEUE_FULL)
		{
			printf("myQueue01Handle errQUEUE_FULL\r\n");
		}
		key2_value = 0;
	}
	if(keyup_value == WKUP_PRES)
	{
		Show_SYS_INFO_Task();
		keyup_value = 0;
	}
	j++;
	if(j%10 == 0)
	  HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);

    osDelay(10);
  }
  /* USER CODE END StartPrintfTask */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the myTask02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void const * argument)
{
  /* USER CODE BEGIN StartTask02 */
  /* Infinite loop */
  for(;;)
  {	 
	  osSemaphoreWait(myBinarySem01Handle, osWaitForever);
	  HAL_GPIO_TogglePin(LED0_GPIO_Port,LED0_Pin);
	  printf("StartTask02 osSemaphoreWait(myBinarySem01Handle);\r\n");     
	  osDelay(10);
  }
  /* USER CODE END StartTask02 */
}

/* USER CODE BEGIN Header_StartTask03 */
/**
* @brief Function implementing the myTask03 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask03 */
void StartTask03(void const * argument)
{
  /* USER CODE BEGIN StartTask03 */
	uint8_t pvBuffer ;
	MSG *RXMSG;
	uint16_t i = 0;
  /* Infinite loop */
  for(;;)
  {
	if(xQueueReceive( myQueue02Handle, (void*)&RXMSG, 10) == pdPASS)
	{
		printf("\r\nRXMSG->ucMessageID = %d \r",RXMSG->ucMessageID);
		printf("RXMSG->ucData[0] = ");
		for(i=0;i<20;i++)
			printf(" %03d",RXMSG->ucData[i]);
		printf("\r\n");
		HAL_GPIO_TogglePin(LED0_GPIO_Port,LED0_Pin);
	}
	if(xQueueReceive( myQueue01Handle, (void *) &pvBuffer, 10) == pdPASS)  
	{
		printf("myQueue01Handle %03d \r\n",pvBuffer);
		HAL_GPIO_TogglePin(LED0_GPIO_Port,LED0_Pin);
	}
	osDelay(10);
  }
  /* USER CODE END StartTask03 */
}

/* Callback01 function */
void Callback01(void const * argument)
{
  /* USER CODE BEGIN Callback01 */

  /* USER CODE END Callback01 */
}

/* Callback02 function */
void Callback02(void const * argument)
{
  /* USER CODE BEGIN Callback02 */

  /* USER CODE END Callback02 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void Show_SYS_INFO_Task(void)
{
	uint8_t pcWriteBuffer[200];
	printf("=================================================\r\n");
	printf("\r\ntask_name  \tstate\t prior\trtack\t Id\r\n");
	vTaskList((char *)&pcWriteBuffer);
	printf("%s\r\n", pcWriteBuffer);

	printf("\r\ntask_name     time_count(10us) usage_pec\r\n");
	vTaskGetRunTimeStats((char *)&pcWriteBuffer);
	printf("%s\r\n", pcWriteBuffer);

}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

```
FreeRTOSConfig.h头文件
```
/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

	***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
	***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
	the FAQ page "My application does not run, what could be wrong?".  Have you
	defined configASSERT()?

	http://www.FreeRTOS.org/support - In return for receiving this top quality
	embedded software for free we request you assist our global community by
	participating in the support forum.

	http://www.FreeRTOS.org/training - Investing in training allows your team to
	be as productive as possible as early as possible.  Now you can receive
	FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
	Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/

/* USER CODE BEGIN Includes */   	      
/* Section where include file can be added */
/* USER CODE END Includes */

/* Ensure stdint is only used by the compiler, and not the assembler. */
#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
    #include <stdint.h>
    extern uint32_t SystemCoreClock;
/* USER CODE BEGIN 0 */   	      
    extern void configureTimerForRunTimeStats(void);
    extern unsigned long getRunTimeCounterValue(void);  
/* USER CODE END 0 */       
#endif

#define configUSE_PREEMPTION                     1
#define configSUPPORT_STATIC_ALLOCATION          0
#define configSUPPORT_DYNAMIC_ALLOCATION         1
#define configUSE_IDLE_HOOK                      0
#define configUSE_TICK_HOOK                      0
#define configCPU_CLOCK_HZ                       ( SystemCoreClock )
#define configTICK_RATE_HZ                       ((TickType_t)1000)
#define configMAX_PRIORITIES                     ( 7 )
#define configMINIMAL_STACK_SIZE                 ((uint16_t)64)
#define configTOTAL_HEAP_SIZE                    ((size_t)15360)
#define configMAX_TASK_NAME_LEN                  ( 16 )
#define configGENERATE_RUN_TIME_STATS            1
#define configUSE_TRACE_FACILITY                 1
#define configUSE_STATS_FORMATTING_FUNCTIONS     1
#define configUSE_16_BIT_TICKS                   0
#define configUSE_MUTEXES                        1
#define configQUEUE_REGISTRY_SIZE                8
#define configUSE_PORT_OPTIMISED_TASK_SELECTION  1

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES                    0
#define configMAX_CO_ROUTINE_PRIORITIES          ( 2 )

/* Software timer definitions. */
#define configUSE_TIMERS                         1
#define configTIMER_TASK_PRIORITY                ( 2 )
#define configTIMER_QUEUE_LENGTH                 10
#define configTIMER_TASK_STACK_DEPTH             128

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
#define INCLUDE_vTaskPrioritySet            1
#define INCLUDE_uxTaskPriorityGet           1
#define INCLUDE_vTaskDelete                 1
#define INCLUDE_vTaskCleanUpResources       0
#define INCLUDE_vTaskSuspend                1
#define INCLUDE_vTaskDelayUntil             0
#define INCLUDE_vTaskDelay                  1
#define INCLUDE_xTaskGetSchedulerState      1
#define INCLUDE_pcTaskGetTaskName           1
#define INCLUDE_xTaskGetCurrentTaskHandle   1
#define INCLUDE_eTaskGetState               1

/* Cortex-M specific definitions. */
#ifdef __NVIC_PRIO_BITS
 /* __BVIC_PRIO_BITS will be specified when CMSIS is being used. */
 #define configPRIO_BITS         __NVIC_PRIO_BITS
#else
 #define configPRIO_BITS         4
#endif

/* The lowest interrupt priority that can be used in a call to a "set priority"
function. */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY   15

/* The highest interrupt priority that can be used by any interrupt service
routine that makes calls to interrupt safe FreeRTOS API functions.  DO NOT CALL
INTERRUPT SAFE FREERTOS API FUNCTIONS FROM ANY INTERRUPT THAT HAS A HIGHER
PRIORITY THAN THIS! (higher priorities are lower numeric values. */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5

/* Interrupt priorities used by the kernel port layer itself.  These are generic
to all Cortex-M ports, and do not rely on any particular library functions. */
#define configKERNEL_INTERRUPT_PRIORITY 		( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

/* Normal assert() semantics without relying on the provision of an assert.h
header file. */
/* USER CODE BEGIN 1 */
#define configASSERT( x ) if ((x) == 0) {taskDISABLE_INTERRUPTS(); for( ;; );}
/* USER CODE END 1 */

/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS
standard names. */
#define vPortSVCHandler    SVC_Handler
#define xPortPendSVHandler PendSV_Handler

/* IMPORTANT: This define MUST be commented when used with STM32Cube firmware,
              to prevent overwriting SysTick_Handler defined within STM32Cube HAL */
/* #define xPortSysTickHandler SysTick_Handler */

/* USER CODE BEGIN 2 */    
/* Definitions needed when configGENERATE_RUN_TIME_STATS is on */
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS configureTimerForRunTimeStats
#define portGET_RUN_TIME_COUNTER_VALUE getRunTimeCounterValue    
/* USER CODE END 2 */

/* USER CODE BEGIN Defines */   	      
/* Section where parameter definitions can be added (for instance, to override default ones in FreeRTOS.h) */
/* USER CODE END Defines */

#endif /* FREERTOS_CONFIG_H */

```
*FreeRTOS使用queue的注意事项*
1. FreeRTOS消息队列最大传递数据为一个32位值，且底层实现是memcpy，所以需要传递变量地址；如果只是简单的变量传输可以直接传递变量地址，`osMessageQDef(myQueue01, 32, uint8_t);`中的最后一个参数type可以为uint8_t或uint16_t或uint32_t等。
2. 如果想要传输多字节数据，我们只要将指向数据的地址的指针传递过去即可（指针本身大小为32位），如我们定义一个结构体指针`TXMSG`用来存放发送数据，数据完成打包后，使用`在xQueueSend(myQueue02Handle,(void*)&TXMSG,0)`将结构体指针的地址用消息队列发送出去。在消息队列接收端同样定义一个相同类型的结构体指针`RXMSG`来接收数据`xQueueReceive( myQueue02Handle, (void*)&RXMSG, 10)`,这样即可以实现多字节大量数据的传输。
```
typedef struct AMessage
{
    char ucMessageID;
    char ucData[ 20 ];
}MSG;
```
3. 多字节的大数据传输时，最好在全局区定义并赋初始值以便程序运行时即可分配空间。


[源代码1上传github](https://github.com/sangeren1002/Notes/blob/master/FreeRTOS/code/cubemx_queue.zip)

[源代码2上传github](https://github.com/sangeren1002/Notes/blob/master/FreeRTOS/code/cubemx_queue1.zip)

### 串口打印效果：
![串口打印效果](https://github.com/sangeren1002/Notes/blob/master/FreeRTOS/image/freertos_queue_1_printf.png?raw=true)


![公众号二维码](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/I2C/gzh_ewm.jpg?raw=true)
