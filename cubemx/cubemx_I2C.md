# cubemx配置硬件I2C操作流程
由于ST的I2C需要规避专利问题，进行多层封装。显得有点臃肿和复杂，没有软件模拟来的直观和快捷。而且如果是新手使用硬件I2C还存在无以名状的bug导致通信失败，最致命的问题就是总线总是出现`I2C_FLAG_BUSY`标志位置位。网上找了很多资料，发现这个问题不是不能解决，而是我们需要进行常规配置外的其他操作才能规避这个问题。
## 1.CUBEMX生成代码后I2C总线初始化一直出现 I2C_FLAG_BUSY
### 1.1 环境：
- packages版本（STM32F4 1.21）
- cubemx版本（version4.27.0 && STM32Cube v1.0）
- MDK版本（KEIL6 V5.23.0.0）

![packages版本](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/I2C/packages.png?raw=true)
![cubemx版本](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/I2C/cubemx_ver.png?raw=true)
![KEIL5版本](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/I2C/MDK%E7%89%88%E6%9C%AC.png?raw=true)
### 1.2 硬件I2C配置为常规配置，参数及管脚如下
**参数配置为默认**

![硬件I2C参数](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/I2C/I2C_PAR.png?raw=true)

**管脚配置为默认**

![硬件I2C管脚](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/I2C/%E7%AE%A1%E8%84%9A%E9%85%8D%E7%BD%AE.png?raw=true)
### 1.3 时钟等其他 配置完成后生成工程
时钟这些都是常规配置了

![rcc配置](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/I2C/rcc.png?raw=true)           

![时钟配置](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/I2C/clock.png?raw=true)
### 1.4 添加代码
如果按照其他外设常规做法生成代码后就可以进行与硬件设备通信，测试一下功能正常与否。但是，你会发现与硬件通信出现问题，debug中你会发现总线的有一个标志位总是被置位。这个标志位`I2C_FLAG_BUSY`标志总线在忙状态，导致无法通信。
在i2c.c文件中的的void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)函数中添加如下代码
```
__I2C2_FORCE_RESET();
__I2C2_RELEASE_RESET();
```
实际如下图
![修改代码实例图](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/I2C/%E4%BB%A3%E7%A0%81%E4%BF%AE%E6%94%B91.png?raw=true)
这里采用读取板载的MPU6050的设备地址来确认硬件I2C是否工作正常。
主函数里main.c添加程序如下：
```
/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
uint8_t device_id_reg = 0x75;	//存放MPU6050设备地址的寄存器
uint8_t device_id = 0x68;		//MPU6050设备地址，0x75寄存器读取的地址与此相同
uint8_t device_id_reg_ret;	//用来存放读取的设备地址
/* USER CODE END PV */
```
在while循环里添加读取设备地址代码，效果如下：
```
while (1)
 {

 /* USER CODE END WHILE */

 /* USER CODE BEGIN 3 */
 uint32_t cnt = 0;
 HAL_Delay(10);
 if(cnt%10==0)
 {
	 HAL_GPIO_TogglePin(LED0_GPIO_Port,LED0_Pin | LED1_Pin);
 }
 if(cnt%20==0)
 {
	 HAL_I2C_Mem_Read(&hi2c1,device_id<<1 , 0X75, I2C_MEMADD_SIZE_8BIT,&device_id_reg_ret,1, 1000);//读MPU6050的ID
	 printf("MPU6050device_id %x\r\n",device_id_reg_ret);
 }
	 cnt++;
 }
```
编译下载后在串口可以看到打印信息：
![串口打印](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/I2C/usart_printf.png?raw=true)

**与给定的设备地址相同，说明代码配置的硬件i2c可用，如果有问题可以留言。**
### 1.5 完美解决I2C总线置位 I2C_FLAG_BUSY

![公众号二维码](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/I2C/gzh_ewm.jpg?raw=true)
