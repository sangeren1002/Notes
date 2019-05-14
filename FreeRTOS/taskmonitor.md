# cubemx配置FreeRTOS串口输出任务信息
```
关键步骤：
1. cubemx中使能任务信息相关宏（3个）;
2. 配置2个宏定义
3. 开启一个定时器，作为计算任务信息的时钟；
4. 调用函数输出任务信息
```
## 1.环境：
- packages版本（STM32F4 1.21）
- cubemx版本（version4.27.0 && STM32Cube v1.0）
- MDK版本（KEIL6 V5.23.0.0）

![packages版本](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/I2C/packages.png?raw=true)
![cubemx版本](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/I2C/cubemx_ver.png?raw=true)
![KEIL5版本](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/I2C/MDK%E7%89%88%E6%9C%AC.png?raw=true)
### 2. cubemx配置
```
主要配置
1. 配置时钟源
2. 配置系统debug与系统时钟
3. 配置定时器5
4. 配置串口1
5. 使能FreeRTOS,配置相关宏,添加3个任务
6. 生成工程代码
```
![cubemx配置1](https://github.com/sangeren1002/Notes/blob/master/FreeRTOS/image/freertos_task_monitor_1_cubemx.png?raw=true)
![cubemx配置2](https://github.com/sangeren1002/Notes/blob/master/FreeRTOS/image/freertos_task_monitor_2_cubemx.png?raw=true)
![cubemx配置3](https://github.com/sangeren1002/Notes/blob/master/FreeRTOS/image/freertos_task_monitor_3_cubemx.png?raw=true)
![cubemx配置4](https://github.com/sangeren1002/Notes/blob/master/FreeRTOS/image/freertos_task_monitor_4_cubemx.png?raw=true)
![cubemx配置5](https://github.com/sangeren1002/Notes/blob/master/FreeRTOS/image/freertos_task_monitor_5_cubemx.png?raw=true)
### 3. 代码修改
```
主要工作：
1. 定义定时器5的溢出次数计算任务信息
2. 在相关宏定义函数内添加定时器溢出次数变量
3. 修改定时器中断函数
4. 启动定时器5
5. 修改串口1到标准输出
6. 布置led闪烁与打印任务信息
```
在`freertos.c`定义定时器5的溢出次数变量`ulHighFrequencyTimerTicks`
```
volatile unsigned long  ulHighFrequencyTimerTicks = 0ul;
```
在cubemx勾选图示三个宏后会在`freertos.c`中生成两个函数,在其中添加信息如下
![cubemx宏定义](https://github.com/sangeren1002/Notes/blob/master/FreeRTOS/image/freertos_task_monitor_4_cubemx.png?raw=true)
```
void configureTimerForRunTimeStats(void)
{
	ulHighFrequencyTimerTicks = 0ul;
}

unsigned long getRunTimeCounterValue(void)
{
	return ulHighFrequencyTimerTicks;
}
```
在中断管理文件中`stm32f4xx_it.c`修改定时器5的中断函数,
声明定时器溢出次数变量为外部定义变量，
```
/* USER CODE BEGIN 0 */
 extern volatile unsigned long  ulHighFrequencyTimerTicks;
/* USER CODE END 0 */
```
在中断函数里添加`ulHighFrequencyTimerTicks`自加操作
```
void TIM5_IRQHandler(void)
{
  /* USER CODE BEGIN TIM5_IRQn 0 */

  /* USER CODE END TIM5_IRQn 0 */
  HAL_TIM_IRQHandler(&htim5);
  /* USER CODE BEGIN TIM5_IRQn 1 */
	ulHighFrequencyTimerTicks++;
  /* USER CODE END TIM5_IRQn 1 */
}
```
主函数里启动定时器5
```
/* Initialize all configured peripherals */
MX_GPIO_Init();
MX_USART1_UART_Init();
MX_TIM5_Init();
/* USER CODE BEGIN 2 */
HAL_TIM_Base_Start_IT(&htim5);
/* USER CODE END 2 */
```
`usart.c`文件中修改串口1到标准输出
```
/* USER CODE BEGIN 1 */
//实现printf函数重定向到串口1，即支持printf信息到USART1
 #ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}
/* USER CODE END 1 */
```
添加任务内代码实现任务信息输出
```
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
	uint8_t pcWriteBuffer[200];
  /* Infinite loop */
  for(;;)
  {
	printf("=================================================\r\n");
	printf("\r\ntask_name  \tstate\t prior\trtack\t Id\r\n");
	vTaskList((char *)&pcWriteBuffer);
	printf("%s\r\n", pcWriteBuffer);

	printf("\r\ntask_name     time_count(10us) usage_pec\r\n");
	vTaskGetRunTimeStats((char *)&pcWriteBuffer);
	printf("%s\r\n", pcWriteBuffer);

    osDelay(1000);
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
	HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
    osDelay(500);
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
  /* Infinite loop */
  for(;;)
  {
	  HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
    osDelay(500);
  }
  /* USER CODE END StartTask03 */
}

```
实际串口输出效果如下
![实际串口输出效果]((https://github.com/sangeren1002/Notes/blob/master/FreeRTOS/image/freertos_task_monitor_1_printf.png?raw=true))

代码修改效果如下：
![代码修改1](https://github.com/sangeren1002/Notes/blob/master/FreeRTOS/image/freertos_task_monitor_1_code.png?raw=true)
![代码修改2](https://github.com/sangeren1002/Notes/blob/master/FreeRTOS/image/freertos_task_monitor_2_code.png?raw=true)
![代码修改3](https://github.com/sangeren1002/Notes/blob/master/FreeRTOS/image/freertos_task_monitor_3_code.png?raw=true)
![代码修改4](https://github.com/sangeren1002/Notes/blob/master/FreeRTOS/image/freertos_task_monitor_4_code.png?raw=true)
![代码修改5](https://github.com/sangeren1002/Notes/blob/master/FreeRTOS/image/freertos_task_monitor_5_code.png?raw=true)
![代码修改6](https://github.com/sangeren1002/Notes/blob/master/FreeRTOS/image/freertos_task_monitor_6_code.png?raw=true)
[源代码上传github](https://github.com/sangeren1002/Notes/blob/master/FreeRTOS/code/)
