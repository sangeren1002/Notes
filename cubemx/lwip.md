# cubemx 配置无操作系统下的LWIP
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

***注意：LAN8720A 可以通过 PHYAD0 引脚来配置，该引脚与 RXER 引脚复用，芯片内部自带下拉电阻，当硬复位结束后， LAN8720A 会读取该引脚电平，作为器件的 SMI 地址，接下拉电阻时（浮空也可以，因为芯片内部自带了下拉电阻），设置 SMI 地址为 0，当外接上拉电阻后，可以设置为 1。本硬件RXER引脚浮空，其PHY芯片地址为0。***
## 2. cubemx配置主要内容
### 2.1 RCC配置
![RCC配置](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/LWIP/cubemx_lwip_rcc.png?raw=true)
### 2.2 SYS配置
![SYS配置](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/LWIP/cubemx_lwip_sys.png?raw=true)
### 2.3 USART1配置
![USART1配置](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/LWIP/cubemx_lwip_usart1.png?raw=true)
### 2.4 ETH配置
![ETH配置](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/LWIP/cubemx_lwip_eth.png?raw=true)

![ETH配置](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/LWIP/cubemx_lwip_eth_config_1.png?raw=true)

![ETH配置](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/LWIP/cubemx_lwip_eth_config_2.png?raw=true)

![ETH配置](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/LWIP/cubemx_lwip_eth_config_3.png?raw=true)

**ETH参数设置需要根据芯片手册来确定部分数据**

![ETH参数](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/LWIP/cubemx_lwip_hardware6.png?raw=true)
![ETH参数](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/LWIP/cubemx_lwip_hardware7.png?raw=true)
- PHY芯片的配置需要注意以下几项：
> 1. ETH配置中的高级参数配置
>   - PHY参数`user PHY`,CUBEMX默认选择的是`LAN8742A_PHY_ADDRESS`。
>   - PHY参数`PHY Address Value`,设置为`0`，这个值需要根据硬件连接来确定（LAN8720A 可以通过 PHYAD0 引脚来配置，该引脚与 RXER 引脚复用，芯片内部自带下拉电阻，当硬复位结束后， LAN8720A 会读取该引脚电平，作为器件的 SMI 地址，接下拉电阻时（浮空也可以，因为芯片内部自带了下拉电阻），设置 SMI 地址为 0，当外接上拉电阻后，可以设置为 1。本硬件RXER引脚浮空，其PHY芯片地址为0）。
>   - PHY参数`PHY name`,是一个标识符，可以随便设置，我这里设置的是`LAN8720A`。
> 2. 高级参数配置的最后三项参数设置需要根据芯片手册来设定
>   - 参数`PHY special control/status register Offset`,根据芯片手册看到是十进制31。
>   - 参数`PHY Speed mask`，根据芯片手册看到是[bit3:bit2]表示速度掩码。
>   - 参数`PHY Duplex mask`，根据芯片手册看到是[bit4]表示工作方式是全双工还是半双工掩码。

### 2.5 LWIP配置
#### LWIP使能
![LWIP使能](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/LWIP/cubemx_lwip_lwip.png?raw=true)
#### LWIP参数配置
![LWIP配置](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/LWIP/cubemx_lwip_lwip_config_1.png?raw=true)

![LWIP配置](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/LWIP/cubemx_lwip_lwip_config_2.png?raw=true)
### 2.6 时钟配置
![时钟配置](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/LWIP/cubemx_lwip_clock.png?raw=true)
## 3. 代码完善
### 3.1 添加对PHY硬件复位的代码
![PHY硬件复位的代码](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/LWIP/cubemx_lwip_code1.png?raw=true)
### 3.2 主函数添加LWIP线程函数
![PHY硬件复位的代码](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/LWIP/cubemx_lwip_code2.png?raw=true)
## 4. 实验效果
### 4.1 路由器端查看
是否存在已经在工程中设置ip地址为`192.168.0.100`，MAC地址为`00:80:E1:00:00:00`的设备。

![路由端](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/LWIP/cubemx_lwip_tplink.png?raw=true)

**可以看到IP地址和MAC地址符合我们代码中设置的。**
### 4.2 ping开发板
ping设置的IP地址`192.168.0.100`看能否ping通

![ping](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/LWIP/cubemx_lwip_cmd.png?raw=true)

**可以看到IP地址可以ping通，延时不超过1ms。**

- [代码上传github](https://github.com/sangeren1002/Notes/blob/master/cubemx/code/lwip_cubemx.zip)
-
 ![公众号二维码](https://github.com/sangeren1002/Notes/blob/master/cubemx/image/I2C/gzh_ewm.jpg?raw=true)
