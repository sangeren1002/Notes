
## cubemx配置定时器的输入捕获模式用来检测信号的周期或频率
### 1. 定时器输入捕获功能简介
在输入捕获模式下，当相应的 ICx 信号检测到跳变沿后，将使用捕获/比较寄存器(TIMx_CCRx) 来锁存计数器的值。发生捕获事件时，会将相应的 CCXIF 标志（TIMx_SR 寄存器）置 1， 并可发送中断或 DMA 请求（如果已使能）。如果发生捕获事件时 CCxIF 标志已处于高位， 则会将重复捕获标志 CCxOF（TIMx_SR 寄存器）置 1。可通过软件向 CCxIF 写入 0 来给 CCxIF 清零，或读取存储在 TIMx_CCRx 寄存器中的已捕获数据。向 CCxOF 写入 0 后会将 其清零。
以下示例说明了如何在 TI1 输入出现上升沿时将计数器的值捕获到 TIMx_CCR1 中。具体操 作步骤如下：
>1. 选择有效输入：TIMx_CCR1 必须连接到 TI1 输入，因此向 TIMx_CCMR1 寄存器中的CC1S 位写入 01。只要 CC1S 不等于 00，就会将通道配置为输入模式，并且 TIMx_CCR1寄存器将处于只读状态。
>2. 根据连接到定时器的信号，对所需的输入滤波时间进行编程（如果输入为 TIx 输入之一，则对 TIMx_CCMRx 寄存器中的 ICxF 位进行编程）。假设信号变化时，输入信号最多在 5 个内部时钟周期内发生抖动。因此，我们必须将滤波时间设置为大于 5 个内部时钟周期。在检测到 8 个具有新电平的连续采样（以 fDTS 频率采样）后，可以确认 TI1上的跳变沿。然后向 TIMx_CCMR1 寄存器中的 IC1F 位写入 0011。
>3. 通过向 TIMx_CCER 寄存器中的 CC1P 位和 CC1NP 位写入 0，选择 TI1 通道的有效转换边沿（本例中为上升沿）。
>4. 对输入预分频器进行编程。在本例中，我们希望每次有效转换时都执行捕获操作，因此需要禁止预分频器（向 TIMx_CCMR1 寄存器中的 IC1PS 位写入 00）。
>5. 通过将 TIMx_CCER 寄存器中的 CC1E 位置 1，允许将计数器的值捕获到捕获寄存器中。
>6. 如果需要，可通过将 TIMx_DIER 寄存器中的 CC1IE 位置 1 来使能相关中断请求，并且/或者通过将该寄存器中的 CC1DE 位置 1 来使能 DMA 请求。
>发生输入捕获时：
>  - 发生有效跳变沿时，TIMx_CCR1 寄存器会获取计数器的值。
>  - 将 CC1IF 标志置 1（中断标志）。如果至少发生了两次连续捕获，但 CC1IF 标志未被清零，这样CC1OF捕获溢出标志会被置 1。
>  - 根据 CC1IE 位生成中断。
>  - 根据 CC1DE 位生成 DMA 请求。

要处理重复捕获，建议在读出捕获溢出标志之前读取数据。这样可避免丢失在读取捕获溢出标志之后与读取数据之前可能出现的重复捕获信息。

***注意：通过软件将 TIMx_EGR 寄存器中的相应 CCxG 位置 1 可生成 IC 中断和/或 DMA 请求。***
### 2. 打开工程文件
工程目录`STM32Cube_FW_F4_V1.21.0\Projects\STM324xG_EVAL\Examples\TIM\TIM_InputCapture`
工程文件`STM32Cube_FW_F4_V1.21.0\Projects\STM324xG_EVAL\Examples\TIM\TIM_InputCapture\MDK-ARM\Project.uvprojx`
#### 2.1 源文件分析
源文件分为4部分
>1. Configure the TIM peripheral
>2. Configure the Input Capture channel
>3. Start the Input Capture in interrupt mode
>4. HAL_TIM_IC_CaptureCallback()回调函数内容实现

主要分析第2步和第4步的回调函数实现部分
```
/*##-2- Configure the Input Capture channel ################################*/
  /* Configure the Input Capture of channel 2 */
  sICConfig.ICPolarity  = TIM_ICPOLARITY_RISING;
  sICConfig.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sICConfig.ICPrescaler = TIM_ICPSC_DIV1;
  sICConfig.ICFilter    = 0;   
  if(HAL_TIM_IC_ConfigChannel(&TimHandle, &sICConfig, TIM_CHANNEL_2) != HAL_OK)
  {
    /* Configuration Error */
    Error_Handler();
  }
```
配置定时器的输入捕获极性为上升沿捕获，目的是通过捕获信号两个上升沿的时间差来计算信号频率或者周期。
在回调函数`HAL_TIM_IC_CaptureCallback()`里实现计算两个上升沿时间差
```
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) //检测是否为通道2，如果配置多个定时器还要判断是哪个定时器
  {
    if(uhCaptureIndex == 0)
    {
      /* Get the 1st Input Capture value */
      uwIC2Value1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);//第1次上升沿得到通道2的输入捕获值uwIC2Value1
      uhCaptureIndex = 1;
    }
    else if(uhCaptureIndex == 1)//第2次上升沿得到通道2的输入捕获值uwIC2Value2
    {
      /* Get the 2nd Input Capture value */
      uwIC2Value2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);

      /* Capture computation */
      if (uwIC2Value2 > uwIC2Value1)//定时器为向上计数，计数没有溢出时两次计数差值
      {
        uwDiffCapture = (uwIC2Value2 - uwIC2Value1);
      }
      else  /* (uwIC2Value2 <= uwIC2Value1) *///定时器为向上计数，计数溢出时两次计数差值为(0xFFFF - uwIC2Value1) + uwIC2Value2
      {
        uwDiffCapture = ((0xFFFF - uwIC2Value1) + uwIC2Value2);
      }

      /* Frequency computation: for this example TIMx (TIM1) is clocked by
         2xAPB2Clk */      
      uwFrequency = (2*HAL_RCC_GetPCLK2Freq()) / uwDiffCapture;//计算信号频率，周期为频率的倒数
      uhCaptureIndex = 0;
    }
  }
}
```
## cubemx配置定时器的PWM输入模式用来检测PWM的占空比和周期
### 1. 定时器的PWM输入模式功能简介
此模式是输入捕获模式的一个特例。其实现步骤与输入捕获模式基本相同，仅存在以下不同之处：
  - 两个 ICx 信号被映射至同一个 TIx 输入。
  - 这两个 ICx 信号在边沿处有效，但极性相反。
  - 选择两个 TIxFP 信号之一作为触发输入，并将从模式控制器配置为复位模式。
例如，可通过以下步骤对应用于 TI1 的 PWM 的周期（位于 TIMx_CCR1 寄存器中）和占空 比（位于 TIMx_CCR2 寄存器中）进行测量（取决于 CK_INT 频率和预分频器的值）：
  - 选择 TIMx_CCR1 的有效输入：向 TIMx_CCMR1 寄存器中的 CC1S 位写入 01（选择TI1）。
  - 选择 TI1FP1 的有效极性（用于 TIMx_CCR1 中的捕获和计数器清零）：向 CC1P 位和 CC1NP 位写入“0”（上升沿有效）。
  - 选择 TIMx_CCR2 的有效输入：向 TIMx_CCMR1 寄存器中的 CC2S 写入 10（选择TI1）。
  - 选择 TI1FP2 的有效极性（用于 TIMx_CCR2 中的捕获）：向 CC2P 位和 CC2NP 位写入“1”（下降沿有效）。
  - 选择有效触发输入：向 TIMx_SMCR 寄存器中的 TS 位写入 101（选择 TI1FP1）。
  - 将从模式控制器配置为复位模式：向 TIMx_SMCR 寄存器中的 SMS 位写入 100。
  - 使能捕获：向 TIMx_CCER 寄存器中的 CC1E 位和 CC2E 位写入“1”。

PWM 输入模式时序
![tim_pwm](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/TIM/tim_pwm_1.png?raw=true)
### 2. 打开工程文件
工程目录`STM32Cube_FW_F4_V1.21.0\Projects\STM324xG_EVAL\Examples\TIM\TIM_PWMInput`
工程文件`STM32Cube_FW_F4_V1.21.0\Projects\STM324xG_EVAL\Examples\TIM\TIM_PWMInput\MDK-ARM\Project.uvprojx`
#### 2.1 源文件分析
主要分为6部分
>1. Configure the TIM peripheral
>2. Configure the Input Capture channels
>3. Configure the slave mode
>4. Start the Input Capture in interrupt mode
>5. Start the Input Capture in interrupt mode
>6. HAL_TIM_IC_CaptureCallback()回调函数内容实现

主要分析第2、3和第6步
  - 第2步：两个 ICx 信号被映射至同一个 TIx 输入，这两个 ICx 信号在边沿处有效，但极性相反。
  - 第3步：选择两个 TIxFP 信号之一作为触发输入，并将从模式控制器配置为复位模式；这里的触发输入选择上升沿的那个TIxFP，一个信号周期的测量开始在上升沿到下一个信号来临的上升沿结束。如果选择一个信号的下降沿作为信号测量的开始至下一个信号的下降沿结束，需要设置下降沿的那个TIxFP作为触发输入。计算周期方法与上一种相同，变量做相应调整。
  - 第6步：在定时器的输入捕获完成回调函数里分别得到两个通道的捕获值，通过两个值可以得到占空比和信号周期。

源代码如下：
```
/*##-2- Configure the Input Capture channels ###############################*/
/* Common configuration */
sConfig.ICPrescaler = TIM_ICPSC_DIV1;
sConfig.ICFilter = 0;  

/* Configure the Input Capture of channel 1 */
sConfig.ICPolarity = TIM_ICPOLARITY_FALLING;
sConfig.ICSelection = TIM_ICSELECTION_INDIRECTTI;    
if(HAL_TIM_IC_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1) != HAL_OK)
{
  /* Configuration Error */
  Error_Handler();
}

/* Configure the Input Capture of channel 2 */
sConfig.ICPolarity = TIM_ICPOLARITY_RISING;
sConfig.ICSelection = TIM_ICSELECTION_DIRECTTI;
if(HAL_TIM_IC_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_2) != HAL_OK)
{
  /* Configuration Error */
  Error_Handler();
}
/*##-3- Configure the slave mode ###########################################*/
/* Select the slave Mode: Reset Mode  */
sSlaveConfig.SlaveMode     = TIM_SLAVEMODE_RESET;
sSlaveConfig.InputTrigger  = TIM_TS_TI2FP2;
if(HAL_TIM_SlaveConfigSynchronization(&TimHandle, &sSlaveConfig) != HAL_OK)
{
  /* Configuration Error */
  Error_Handler();
}

/*##-4- Start the Input Capture in interrupt mode ##########################*/
if(HAL_TIM_IC_Start_IT(&TimHandle, TIM_CHANNEL_2) != HAL_OK)
{
  /* Starting Error */
  Error_Handler();
}

/*##-5- Start the Input Capture in interrupt mode ##########################*/
if(HAL_TIM_IC_Start_IT(&TimHandle, TIM_CHANNEL_1) != HAL_OK)
{
  /* Starting Error */
  Error_Handler();
}
```

## cubemx配置定时器的输出比较模式
### 1. 定时器的输出比较功能简介
此功能用于控制输出波形，或指示已经过某一时间段。
当捕获/比较寄存器与计数器之间相匹配时，输出比较功能：
  - 将为相应的输出引脚分配一个可编程值，该值由输出比较模式（TIMx_CCMRx 寄存器中的 OCxM 位）和输出极性（TIMx_CCER 寄存器中的 CCxP 位）定义。匹配时，输出引脚既可保持其电平 (OCXM=000)，也可设置为有效电平 (OCXM=001)、无效电平
(OCXM=010) 或进行翻转 (OCxM=011)。
  - 将中断状态寄存器中的标志置 1（TIMx_SR 寄存器中的 CCxIF 位）。
  - 如果相应中断使能位（TIMx_DIER 寄存器中的 CCXIE 位）置 1，将生成中断。
  - 如果相应DMA使能位（TIMx_DIER 寄存器的 CCxDE 位，TIMx_CR2 寄存器的 CCDS位，用来选择 DMA 请求）置 1，将发送 DMA 请求。使用 TIMx_CCMRx 寄存器中的 OCxPE 位，可将 TIMx_CCRx 寄存器配置为带或不带预装 载寄存器。
在输出比较模式下，更新事件 UEV 对 ocxref 和 OCx 输出毫无影响。同步的精度可以达到计 数器的一个计数周期。输出比较模式也可用于输出单脉冲（在单脉冲模式下）。
> 步骤:
>1.选择计数器时钟（内部、外部、预分频器）。
>2. 在 TIMx_ARR 和 TIMx_CCRx 寄存器中写入所需数据。
>3. 如果要生成中断和/或 DMA 请求，将 CCxIE 位和/或 CCxDE 位置 1。
>4. 选择输出模式。例如，当 CNT 与 CCRx 匹配、未使用预装载 CCRx 并且 OCx 使能且为 高电平有效时，必须写入 OCxM=011、OCxPE=0、CCxP=0 和 CCxE=1 来翻转 OCx 输出引脚。
>5. 通过将 TIMx_CR1 寄存器中的 CEN 位置 1 来使能计数器。

可随时通过软件更新 TIMx_CCRx 寄存器以控制输出波形，前提是未使能预装载寄存器 （OCxPE=0，否则仅当发生下一个更新事件 UEV 时，才会更新 TIMx_CCRx 影子寄存器）。
输出比较模式翻转OC1

### 2. 打开工程文件
工程目录`STM32Cube_FW_F4_V1.21.0\Projects\STM324xG_EVAL\Examples\TIM\TIM_OCActive`
工程文件`STM32Cube_FW_F4_V1.21.0\Projects\STM324xG_EVAL\Examples\TIM\TIM_OCActive\MDK-ARM\Project.uvprojx`
#### 2.1 源文件分析
主要分为4部分
>1. Configure the TIM peripheral
>2. Configure the Output Compare channels
>3. Turn On LED1: use PG6 falling edge as reference
>4. Start signals generation

```
#define  PULSE1_VALUE       1000        /* Capture Compare 1 Value  */
#define  PULSE2_VALUE       500         /* Capture Compare 2 Value  */
#define  PULSE3_VALUE       250         /* Capture Compare 3 Value  */
#define  PULSE4_VALUE       125         /* Capture Compare 4 Value  */
...
/* Compute the prescaler value to have TIMx counter clock equal to 2 KHz */
uwPrescalerValue = ((SystemCoreClock /2) / 2000) - 1;


/*##-1- Configure the TIM peripheral #######################################*/
  /* ---------------------------------------------------------------------------
  TIM3 Configuration: Output Compare Active Mode:
  In this example TIM3 input clock (TIM3CLK) is set to 2 * APB1 clock (PCLK1),
  since APB1 prescaler is different from 1.   
    TIM3CLK = 2 * PCLK1  
    PCLK1 = HCLK / 4
    => TIM3CLK = HCLK / 2 = SystemCoreClock /2

  To get TIM3 counter clock at 2 KHz, the prescaler is computed as follows:
     Prescaler = (TIM3CLK / TIM3 counter clock) - 1
     Prescaler = ((SystemCoreClock /2) /1 KHz) - 1

  Generate 4 signals with 4 different delays:
  TIM3_CH1 delay = uhCCR1_Val/TIM3 counter clock = 500 ms
  TIM3_CH2 delay = uhCCR2_Val/TIM3 counter clock = 250 ms
  TIM3_CH3 delay = uhCCR3_Val/TIM3 counter clock = 125 ms
  TIM3_CH4 delay = uhCCR4_Val/TIM3 counter clock = 62.5 ms

  Note:
   SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f4xx.c file.
   Each time the core clock (HCLK) changes, user had to update SystemCoreClock
   variable value. Otherwise, any configuration based on this variable will be incorrect.
   This variable is updated in three ways:
    1) by calling CMSIS function SystemCoreClockUpdate()
    2) by calling HAL API function HAL_RCC_GetSysClockFreq()
    3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
--------------------------------------------------------------------------- */
...
/*##-2- Configure the Output Compare channels ##############################*/
/* Common configuration for all channels */
sConfig.OCMode     = TIM_OCMODE_ACTIVE;
sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;

/* Set the pulse (delay1)  value for channel 1 */
sConfig.Pulse = PULSE1_VALUE;  
if(HAL_TIM_OC_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1) != HAL_OK)
{
  /* Configuration Error */
  Error_Handler();
}

/* Set the pulse (delay2) value for channel 2 */
sConfig.Pulse = PULSE2_VALUE;
if(HAL_TIM_OC_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_2) != HAL_OK)
{
  /* Configuration Error */
  Error_Handler();
}

/* Set the pulse (delay3) value for channel 3 */
sConfig.Pulse = PULSE3_VALUE;
if(HAL_TIM_OC_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_3) != HAL_OK)
{
  /* Configuration Error */
  Error_Handler();
}

/* Set the pulse (delay4) value for channel 4 */
sConfig.Pulse = PULSE4_VALUE;
if(HAL_TIM_OC_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_4) != HAL_OK)
{
  /* Configuration Error */
  Error_Handler();
}

/*##-3- Turn On LED1: use PG6 falling edge as reference ####################*/
/* Turn on LED1 */
BSP_LED_On(LED1);

/*##-4- Start signals generation ###########################################*/
/* Start channel 1 in Output compare mode */
if(HAL_TIM_OC_Start(&TimHandle, TIM_CHANNEL_1) != HAL_OK)
{
  /* Starting Error */
  Error_Handler();
}
/* Start channel 2 in Output compare mode */
if(HAL_TIM_OC_Start(&TimHandle, TIM_CHANNEL_2) != HAL_OK)
{
  /* Starting Error */
  Error_Handler();
}
/* Start channel 3 in Output compare mode */
if(HAL_TIM_OC_Start(&TimHandle, TIM_CHANNEL_3) != HAL_OK)
{
  /* Starting Error */
  Error_Handler();
}
/* Start channel 4 in Output compare mode */
if(HAL_TIM_OC_Start(&TimHandle, TIM_CHANNEL_4) != HAL_OK)
{
  /* Starting Error */
  Error_Handler();
}
```

## cubemx配置定时器的PWM输出模式
### 2. 打开工程文件
工程目录`STM32Cube_FW_F4_V1.21.0\Projects\STM324xG_EVAL\Examples\TIM\TIM_7PWMOutput\`
工程文件`STM32Cube_FW_F4_V1.21.0\Projects\STM324xG_EVAL\Examples\TIM\TIM_7PWMOutput\MDK-ARM\Project.uvprojx`
#### 2.1 源文件分析
主要分为3步
>1. Configure the TIM peripheral
>2. Configure the PWM channels
>3. Start PWM signals generation

```
/* Compute the Timer period to generate a signal frequency at 17.57 Khz */
  uwPeriod = (SystemCoreClock / 17570 ) - 1;

  /* Compute Pulse1 value to generate a duty cycle at 50% for channel 1 and 1N */
  uwPulse1 = (5 * (uwPeriod - 1)) / 10;
  /* Compute Pulse2 value to generate a duty cycle at 37.5%  for channel 2 and 2N */
  uwPulse2 = (375 * (uwPeriod - 1)) / 1000;
  /* Compute Pulse3 value to generate a duty cycle at 25%  for channel 3 and 3N */
  uwPulse3 = (25 * (uwPeriod - 1)) / 100;
  /* Compute Pulse4 value to generate a duty cycle at 12.5%  for channel 4 */
  uwPulse4 = (125 * (uwPeriod- 1)) / 1000;

  ...

  /*##-1- Configure the TIM peripheral #######################################*/
    /*----------------------------------------------------------------------------
     Generate 7 PWM signals with 4 different duty cycles:
     TIM1 input clock (TIM1CLK) is set to 2 * APB2 clock (PCLK2), since APB2
      prescaler is different from 1.   
      TIM1CLK = 2 * PCLK2  
      PCLK2 = HCLK / 2
      => TIM1CLK = 2 * (HCLK / 2) = HCLK = SystemCoreClock
     TIM1CLK = SystemCoreClock, Prescaler = 0, TIM1 counter clock = SystemCoreClock
     SystemCoreClock is set to 168 MHz for STM32F4xx devices

     The objective is to generate 7 PWM signal at 17.57 KHz:
       - TIM1_Period = (SystemCoreClock / 17570) - 1
     The channel 1 and channel 1N duty cycle is set to 50%
     The channel 2 and channel 2N duty cycle is set to 37.5%
     The channel 3 and channel 3N duty cycle is set to 25%
     The channel 4 duty cycle is set to 12.5%
     The Timer pulse is calculated as follows:
       - ChannelxPulse = DutyCycle * (TIM1_Period - 1) / 100

     Note:
      SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f4xx.c file.
      Each time the core clock (HCLK) changes, user had to call SystemCoreClockUpdate()
      function to update SystemCoreClock variable value. Otherwise, any configuration
      based on this variable will be incorrect.
    ----------------------------------------------------------------------- */

    /* Initialize TIMx peripheral as follow:
         + Prescaler = 0
         + Period = uwPeriod  (to have an output frequency equal to 17.57 KHz)
         + ClockDivision = 0
         + Counter direction = Up
    */
    TimHandle.Instance = TIM1;

    TimHandle.Init.Period            = uwPeriod;
    TimHandle.Init.Prescaler         = 0;
    TimHandle.Init.ClockDivision     = 0;
    TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
    TimHandle.Init.RepetitionCounter = 0;

    if(HAL_TIM_PWM_Init(&TimHandle) != HAL_OK)
    {
      /* Initialization Error */
      Error_Handler();
    }

    /*##-2- Configure the PWM channels #########################################*/
    /* Common configuration for all channels */
    sConfig.OCMode      = TIM_OCMODE_PWM2;
    sConfig.OCFastMode  = TIM_OCFAST_DISABLE;
    sConfig.OCPolarity  = TIM_OCPOLARITY_LOW;
    sConfig.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfig.OCIdleState = TIM_OCIDLESTATE_SET;
    sConfig.OCNIdleState= TIM_OCNIDLESTATE_RESET;

    /* Set the pulse value for channel 1 */
    sConfig.Pulse = uwPulse1;  
    if(HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1) != HAL_OK)
    {
      /* Configuration Error */
      Error_Handler();
    }

    /* Set the pulse value for channel 2 */
    sConfig.Pulse = uwPulse2;
    if(HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_2) != HAL_OK)
    {
      /* Configuration Error */
      Error_Handler();
    }

    /* Set the pulse value for channel 3 */
    sConfig.Pulse = uwPulse3;
    if(HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_3) != HAL_OK)
    {
      /* Configuration Error */
      Error_Handler();
    }

    /* Set the pulse value for channel 4 */
    sConfig.Pulse = uwPulse4;
    if(HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_4) != HAL_OK)
    {
      /* Configuration Error */
      Error_Handler();
    }

    /*##-3- Start PWM signals generation #######################################*/
    /* Start channel 1 */
    if(HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1) != HAL_OK)
    {
      /* Starting Error */
      Error_Handler();
    }  
    /* Start channel 1N */
    if(HAL_TIMEx_PWMN_Start(&TimHandle, TIM_CHANNEL_1) != HAL_OK)
    {
      /* Starting Error */
      Error_Handler();
    }

    /* Start channel 2 */
    if(HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_2) != HAL_OK)
    {
      /* Starting Error */
      Error_Handler();
    }
    /* Start channel 2N */
    if(HAL_TIMEx_PWMN_Start(&TimHandle, TIM_CHANNEL_2) != HAL_OK)
    {
      /* Starting Error */
      Error_Handler();
    }

    /* Start channel 3 */
    if(HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_3) != HAL_OK)
    {
      /* Starting Error */
      Error_Handler();
    }
    /* Start channel 3N */
    if(HAL_TIMEx_PWMN_Start(&TimHandle, TIM_CHANNEL_3) != HAL_OK)
    {
      /* Starting Error */
      Error_Handler();
    }

    /* Start channel 4 */
    if(HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_4) != HAL_OK)
    {
      /* Starting Error */
      Error_Handler();
    }  
```

**PWM输出模式与输出比较模式的不同**
>1. 定时器的输出比较模式可以在多个通道上产生频率不同，占空比也不同的输出信号;定时器的PWM输出模式智能在多个通道上产生频率相同单占空比不同的输出信号。
























![公众号二维码](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/I2C/gzh_ewm.jpg?raw=true)
