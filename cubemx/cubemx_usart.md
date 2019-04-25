## **目录 (Table of Contents)**
<!-- TOC -->

- [一、cubemx配置工程](#%E4%B8%80cubemx%E9%85%8D%E7%BD%AE%E5%B7%A5%E7%A8%8B)
  - [1.1基于stm32f407开发板](#11%E5%9F%BA%E4%BA%8Estm32f407%E5%BC%80%E5%8F%91%E6%9D%BF)
  - [CAN1参数设置](#can1%E5%8F%82%E6%95%B0%E8%AE%BE%E7%BD%AE)
  - [USART1设置](#usart1%E8%AE%BE%E7%BD%AE)
  - [DMA设置](#dma%E8%AE%BE%E7%BD%AE)
  - [GPIO设置](#gpio%E8%AE%BE%E7%BD%AE)
  - [工程设置](#%E5%B7%A5%E7%A8%8B%E8%AE%BE%E7%BD%AE)
- [二、代码完善](#%E4%BA%8C%E4%BB%A3%E7%A0%81%E5%AE%8C%E5%96%84)
  - [2.1串口功能完善](#21%E4%B8%B2%E5%8F%A3%E5%8A%9F%E8%83%BD%E5%AE%8C%E5%96%84)

<!-- /TOC -->
# 一、cubemx配置工程

## 1.1基于stm32f407开发板
![开发板型号](https://img-blog.csdnimg.cn/20190410165831548.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70) ![STM32CUBEMX版本信息](https://img-blog.csdnimg.cn/20190410165519968.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70) ![STM32Cube mcu packages](https://img-blog.csdnimg.cn/20190410165414419.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70) ![MCU型号参数](https://img-blog.csdnimg.cn/20190410165244858.png) ![在这里插入图片描述](https://img-blog.csdnimg.cn/20190410171129497.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70) ![在这里插入图片描述](https://img-blog.csdnimg.cn/20190410171156214.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70) ![在这里插入图片描述](https://img-blog.csdnimg.cn/20190410171215423.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)

## CAN1参数设置

 ![在这里插入图片描述](https://img-blog.csdnimg.cn/20190410171243805.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70) ![在这里插入图片描述](https://img-blog.csdnimg.cn/20190410171255267.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)

 ![在这里插入图片描述](https://img-blog.csdnimg.cn/20190410171255267.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)

![在这里插入图片描述](https://img-blog.csdnimg.cn/20190410171255267.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)

 ![在这里插入图片描述](https://img-blog.csdnimg.cn/20190410171255267.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)

 ![在这里插入图片描述](https://img-blog.csdnimg.cn/20190410171255267.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70) ![在这里插入图片描述](https://img-blog.csdnimg.cn/2019041017132295.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70) ![在这里插入图片描述](https://img-blog.csdnimg.cn/20190410171307696.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)

## USART1设置

  ![在这里插入图片描述](https://img-blog.csdnimg.cn/20190410171414317.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)

 ![在这里插入图片描述](https://img-blog.csdnimg.cn/20190410171414317.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)

 ![在这里插入图片描述](https://img-blog.csdnimg.cn/20190410171414317.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)

 ![在这里插入图片描述](https://img-blog.csdnimg.cn/20190410171414317.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70) ![在这里插入图片描述](https://img-blog.csdnimg.cn/2019041017142929.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70) ![在这里插入图片描述](https://img-blog.csdnimg.cn/20190410171438251.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70) ![在这里插入图片描述](https://img-blog.csdnimg.cn/2019041017145220.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70) ![在这里插入图片描述](https://img-blog.csdnimg.cn/20190410171503421.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)

## DMA设置

  ![在这里插入图片描述](https://img-blog.csdnimg.cn/20190410171515843.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)

## GPIO设置

  ![在这里插入图片描述](https://img-blog.csdnimg.cn/2019041017155281.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)

 定时器设置 ![在这里插入图片描述](https://img-blog.csdnimg.cn/20190410171712285.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70) ![在这里插入图片描述](https://img-blog.csdnimg.cn/20190410171658653.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70) 中断分组设置

  ![在这里插入图片描述](https://img-blog.csdnimg.cn/20190410171637745.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)

## 工程设置

  ![在这里插入图片描述](https://img-blog.csdnimg.cn/20190410171830835.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)

 ![在这里插入图片描述](https://img-blog.csdnimg.cn/20190410171830835.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)

 ![在这里插入图片描述](https://img-blog.csdnimg.cn/20190410171830835.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)

 ![在这里插入图片描述](https://img-blog.csdnimg.cn/20190410171830835.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70) ![在这里插入图片描述](https://img-blog.csdnimg.cn/20190410171851387.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)

 生成工程 ![在这里插入图片描述](https://img-blog.csdnimg.cn/20190410171955717.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70) 工程生成成功打开工程

  ![在这里插入图片描述](https://img-blog.csdnimg.cn/20190410172017929.png)

# 二、代码完善

## 2.1串口功能完善

> 采用DMA+串口空闲中断方式实现串口收发不定长数
>
> ### 2.1.1 新添加工程文件bsp\_usart.c及bsp\_usart.h

```
/***********************************************************************************
 * 文 件 名   : bsp_usart.c
 * 负 责 人   : jishubao
 * 创建日期   : 2019年3月6日
 * 文件描述   : 板载串口功能文件
 * 版权说明   : Copyright (c) 2008-2019   
 * 其    他   :
 * 修改日志   :
***********************************************************************************/
#include "bsp_usart.h"
#include "usart.h"
#include "string.h"

USART_RECEIVETYPE UsartType;


/*****************************************************************************
 * 函 数 名  : HAL_UART_TxCpltCallback
 * 负 责 人  : jishubao
 * 创建日期  : 2019年2月27日
 * 函数功能  : 串口发送完成回调函数
 * 输入参数  : UART_HandleTypeDef *huart  串口操作句柄
 * 输出参数  : 无
 * 返 回 值  :
 * 调用关系  :
 * 其    它  :   使用DMA时需要将发送完成状态更新，否则DMA会出现bug

*****************************************************************************/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    huart->gState = HAL_UART_STATE_READY;
}

/*****************************************************************************
 * 函 数 名  : UsartReceive_IDLE
 * 负 责 人  : jishubao
 * 创建日期  : 2019年2月19日
 * 函数功能  : 串口接受空闲中断处理函数
 * 输入参数  : UART_HandleTypeDef *huart  串口句柄
 * 输出参数  : 无
 * 返 回 值  :
 * 调用关系  :
 * 其    它  :

*****************************************************************************/
void UsartReceive_IDLE(UART_HandleTypeDef *huart)  
{  
    uint32_t temp;  

    if((__HAL_UART_GET_FLAG(huart,UART_FLAG_IDLE) != RESET))  
    {   
        __HAL_UART_CLEAR_IDLEFLAG(huart);  
        HAL_UART_DMAStop(huart);  
        temp = (*huart).hdmarx->Instance->NDTR;
        if(huart->Instance==USART1)
        {
            UsartType.RX_Size = RX_LEN - temp;   
            UsartType.RX_flag = 1;  
            HAL_UART_Receive_DMA(huart,UsartType.RX_pData,RX_LEN);  
        }
    }

}

/*****************************************************************************
 * 函 数 名  : Analysis_Serial_Data
 * 负 责 人  : jishubao
 * 创建日期  : 2019年2月19日
 * 函数功能  : 串口接收数据直接转发函数
 * 输入参数  : void  无
 * 输出参数  : 无
 * 返 回 值  : void
 * 调用关系  :
 * 其    它  :

*****************************************************************************/
 void Analysis_Serial_Data(void)
 {
     if(UsartType.RX_flag)         // Receive flag
     {    
         UsartType.RX_flag=0;     // clean flag
         HAL_UART_Transmit_DMA(&huart1, UsartType.RX_pData, UsartType.RX_Size);
         HAL_GPIO_TogglePin(GPIOF,LED1_Pin);
     }
 }

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
```

```
/***********************************************************************************
 * 文 件 名   : bsp_usart.h
 * 负 责 人   : jishubao
 * 创建日期   : 2019年3月6日
 * 文件描述   : bsp_usart.c 的头文件
 * 版权说明   : Copyright (c) 2008-2019   
 * 其    他   :
 * 修改日志   :
***********************************************************************************/

#ifndef __BSP_USART_H__
#define __BSP_USART_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "main.h"
#include "usart.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */
/* BEGIN Private defines */
#define RX_LEN 1024  

typedef struct  
{  
    uint8_t  RX_flag:1;        //IDLE receive flag
    uint8_t  TX_flag:1;        //IDLE send flag
    uint16_t RX_Size;          //receive length
    uint8_t  RX_pData[RX_LEN]; //DMA receive buffer
}USART_RECEIVETYPE;  

extern USART_RECEIVETYPE UsartType;
/* END Private defines */


void Analysis_Serial_Data(void);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void UsartReceive_IDLE(UART_HandleTypeDef *huart);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif
```

在stm32f4xx\_it.c文件中添加如下代码  
![在这里插入图片描述](https://img-blog.csdnimg.cn/20190410173602816.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)  
![在这里插入图片描述](https://img-blog.csdnimg.cn/20190410173633270.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)  
在main.c中添加部分代码  
![在这里插入图片描述](https://img-blog.csdnimg.cn/20190410174314108.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)  
![在这里插入图片描述](https://img-blog.csdnimg.cn/20190410174346537.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)  
![在这里插入图片描述](https://img-blog.csdnimg.cn/2019041017441279.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)  
至此串口功能代码完善完成，下载到开发板后重启看到LED0闪烁（10Hz），可以看到通过串口工具发送给开发板数据，开发板会将原数据发送回串口工具，而且LED随着串口发送数据闪烁。  
[程序源码](https://pan.baidu.com/s/1uknCgtpOxWr5iOgaKMhFAw)   密码：kbmj
