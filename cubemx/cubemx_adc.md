# cubemx 配置多通道ADC进行ADC采样
## AD的基础知识
AD很复杂，其实也不复杂，因为我们用的不多。
- AD：模拟量转换数字量（模拟信号转换数字信号）。
- ADC：模拟量转数字量的转换器。
- 为什么需要AD呢？
  > 自然界宏观的物理量都是连续的。而机器识别的信号0与1是离散的。为了让机器能够采集、分析、储存这些连续的量，所以要把需要被模拟的量转换为数字量。

- 主要的参数：采样精度与转换速度。
- 采样精度，也叫作分辨率。如果0-100摄氏度用8位AD来储存的话，精度计算？
> 0000 0000 —— 0摄氏度 ;  1111 1111 —— 100摄氏度;100/256 = 0.39摄氏度，0.39就是最小份的分辨率，也就是采样精度.没有办法表>现出0.2度。100/65536 = 0.0015，16位的AD，精度就很高了。

- 转换速度
>从启动到出结果用的时间，一般来说越快越好。与精度不可兼得。被单位时间采样数量影响。采样多，速度不会很快。

  STM32F4使用的AD是逐次逼近型ADC，它产生一系列比较电压VR，但它是逐个产生比较电压，逐次与输入电压分别比较，以逐渐逼近的方式进行模数转换的。它比并联比较型ADC的转换速度慢，比双分积型ADC要快得多，属于中速ADC器件。
###  多通道AD采集需要使用DMA
  在STM32的手册中，我们发现，不论是单次采集还是多次采集，转换完成的数据都会放在同一个地方。
![ADC](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/ADC/cubemx_adc_1.png?raw=true)
由于ADC_DR寄存器不是一个数组，而是一个16位的变量，所以只能保存最新的转换结果。例如，通道1和通道2都使用，通道1的转换结果放在DR寄存器。通道2转换完毕以后，就会覆盖通道1的结果。

程序里，当然可以通过一些处理，让通道1的结果在被覆盖之前就保存好。不过，运用STM32的DMA功能，可以更好地解决结果被覆盖的问题。
![ADC](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/ADC/cubemx_adc_2.png?raw=true)
重点：用于高速搬运数据，还无需CPU干预。 因此在多通道采集模拟量是，我们可以建立一个数组，用于储存AD转换的数据。一旦ADC_DR寄存器里有了新的数据，就把新数据放在数组里。一会儿ADC_DR有了一个新的数据，就放在数组下一位。数组装满以后？根据需求来。我们设置的是循环模式，也就是再来一遍，覆盖之前的数据。

STM32F4xx系列一般都有3个ADC，这些ADC可以独立使用，也可以使用双重/三重模式（提高采样率）。STM32F4的ADC是12位逐次逼近型的模拟数字转换器。它有19个通道，可测量16个外部源、2个内部源和Vbat通道的信号。这些通道的A/D转换可以单次、连续、扫描或间断模式执行。ADC的结果可以左对齐或右对齐方式存储在16位数据寄存器中。模拟看门狗特性允许应用程序检测输入电压是否超出用户定义的高/低阀值。
STM32F407ZGT6包含有3个ADC。STM32F4的ADC最大的转换速率为2.4Mhz，也就是转换时间为0.41us（在ADCCLK=36M,采样周期为3个ADC时钟下得到），不要让ADC的时钟超过36M，否则将导致结果准确度下降。
## cubemx 配置DMA+多通道ADC(ADC1的4个通道)
### 1. 环境：
- packages版本（STM32F4 1.21）
- cubemx版本（version4.27.0 && STM32Cube v1.0）
- MDK版本（KEIL5 V5.23.0.0）

![packages版本](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/I2C/packages.png?raw=true)
![cubemx版本](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/I2C/cubemx_ver.png?raw=true)
![KEIL5版本](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/I2C/MDK%E7%89%88%E6%9C%AC.png?raw=true)
### 2. cubemx配置主要内容
#### ADC1引脚配置
![ADC1引脚配置](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/ADC/cubemx_adc_3.png?raw=true)
#### ADC1参数配置
![ADC1参数配置](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/ADC/cubemx_adc_4.png?raw=true)
#### ADC中断配置
![ADC中断配置](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/ADC/cubemx_adc_5.png?raw=true)
#### ADC1的DMA配置
![ADC1的DMA配置](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/ADC/cubemx_adc_6.png?raw=true)
>说明一个问题
这里ADC的DMA设置的是循环模式，数据宽度是字（32位），由于STM32F407存储ADC的结果为一个16位的数据寄存器，故这里可以设置为半字（16位）。已经实际测试，效果是一样的。


***生成代码***
### 3. 完善代码
#### 3.1 定义变量
```
/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
#define ADC_CHANNEL_CNT 4 	//采样通道数
#define ADC_CHANNEL_FRE 100	//单个通道采样次数，用来取平均值
uint32_t adc1_val_buf[400]; //传递给DMA存放多通道采样值的数组
uint16_t i,j;
uint32_t adc1_aver_val[ADC_CHANNEL_CNT] = {0}; //保存多通道的平均采样值的数组
uint32_t dma_cnt1 = 0;
/* USER CODE END PV */
```
#### 3.2 开启ADC1的DMA转换
```
/* USER CODE BEGIN 2 */
HAL_ADC_Start_DMA(&hadc1,(uint32_t*) &adc1_val_buf, (ADC_CHANNEL_CNT*ADC_CHANNEL_FRE));
HAL_Delay(100);
/* USER CODE END 2 */
```
这里解释一下`HAL_ADC_Start_DMA()`函数，第一个参数是ADC的操作句柄；第二个参数是用来保存ADC转换结果的数组的地址，是内容的copy所以要传递数组的地址；第三个参数是转换的数据长度。
#### 3.3 完善主函数
```
while (1)
{

/* USER CODE END WHILE */

/* USER CODE BEGIN 3 */
  /* 清除adc采样平均值变量 */
for(i=0;i<ADC_CHANNEL_CNT;i++)
{
  adc1_aver_val[i] = 0;
}
/* 在采样值数组中分别取出每个通道的采样值并求和 */
for(i=0;i<ADC_CHANNEL_FRE;i++)
{
  adc1_aver_val[0] +=  adc1_val_buf[i*4+0];
  adc1_aver_val[1] +=  adc1_val_buf[i*4+1];
  adc1_aver_val[2] +=  adc1_val_buf[i*4+2];
  adc1_aver_val[3] +=  adc1_val_buf[i*4+3];
}
/* 依次对每个通道采样值求平均值 */
for(i=0;i<ADC_CHANNEL_CNT;i++)
{
  adc1_aver_val[i] /=ADC_CHANNEL_FRE;
}
printf("\n\r ***** START PRINTF ADC INFO ******** \r\n\n");
for(i=0;i<ADC_CHANNEL_CNT;i++)
{
  printf("ADC1[%02d] Sampling voltage = %1.3f V Sampling value = %04d\r\n",i,adc1_aver_val[i]*3.30f/4095,adc1_aver_val[i]);
}
printf("DMA CNT = %06d\r\n",dma_cnt1);
dma_cnt1 = 0;
printf("\n\r END PRINTF ADC INFO \r\n\n");

HAL_Delay(1000);
HAL_GPIO_TogglePin(GPIOF,GPIO_PIN_10);  
}
```
#### 3.4 ADC转换完成回调函数完善
一次ADC转换完成回调函数，可以在里面操作记录DMA搬移数据的次数
```
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if(hadc==(&hadc1))
	{
		dma_cnt1++;
	}   
}
```
#### 3.5 串口打印效果：
![串口打印效果](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/ADC/cubemx_adc_printf.png?raw=true)
- [源代码上传github](https://github.com/sangeren1002/Notes/blob/master/cubemx/code/cubemx_Multiple_adc.zip)


![公众号二维码](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/I2C/gzh_ewm.jpg?raw=true)
