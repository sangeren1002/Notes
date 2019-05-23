## cubemx 配置无操作系统下的LWIP
## 1. 环境
### 1.1 软件环境：
- packages版本（STM32F4 1.21）
- cubemx版本（version4.27.0 && STM32Cube v1.0）
- MDK版本（KEIL5 V5.23.0.0）

![packages版本](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/I2C/packages.png?raw=true)
![cubemx版本](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/I2C/cubemx_ver.png?raw=true)
![KEIL5版本](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/I2C/MDK%E7%89%88%E6%9C%AC.png?raw=true)
### 1.2 硬件环境
-  正点原子探索者开发板(STM32F407ZGT6)
- 网络接口原理图部分

![网络接口原理图](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/LWIP/cubemx_lwip_hardware1.png?raw=true)
![网络接口原理图](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/LWIP/cubemx_lwip_hardware2.png?raw=true)
![网络接口原理图](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/LWIP/cubemx_lwip_hardware3.png?raw=true)
![网络接口原理图](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/LWIP/cubemx_lwip_hardware4.png?raw=true)
![网络接口原理图](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/LWIP/cubemx_lwip_hardware5.png?raw=true)
![网络接口原理图](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/LWIP/cubemx_lwip_hardware6.png?raw=true)

## 2. cubemx配置主要内容
### 2.1 RCC配置
![RCC配置](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/LWIP/cubemx_lwip_rcc.png?raw=true)
### 2.2 SYS配置
![SYS配置](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/LWIP/cubemx_lwip_sys.png?raw=true)
### 2.3 USART1配置
![USART1配置](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/LWIP/cubemx_lwip_usart1.png?raw=true)
### 2.4 ETH配置
![ETH配置](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/LWIP/cubemx_lwip_eth.png?raw=true)
### 2.5 LWIP配置
![LWIP使能](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/LWIP/cubemx_lwip_lwip.png?raw=true)

### 2.6 时钟配置
![时钟配置](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/LWIP/cubemx_lwip_clock.png?raw=true)
## 3. 代码完善
