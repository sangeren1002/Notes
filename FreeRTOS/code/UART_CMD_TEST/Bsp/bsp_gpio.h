/***********************************************************************************
 * 文 件 名   : bsp_gpio.h
 * 负 责 人   : jishubao
 * 创建日期   : 2020年9月23日
 * 文件描述   : bsp_gpio.c 的头文件
 * 版权说明   : Copyright (c) 2008-2020   杭州国辰机器人科技有限公司
 * 其    他   : 
 * 修改日志   : 
***********************************************************************************/

#ifndef __BSP_GPIO_H__
#define __BSP_GPIO_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "stm32f4xx_hal.h"
#include "gpio.h"
#include "sys_define.h"

#define LedRunToggle()  HAL_GPIO_TogglePin(LED_RUN_GPIO_Port, LED_RUN_Pin)
#define LedAlarmToggle()  HAL_GPIO_TogglePin(LED_ALARM_GPIO_Port, LED_ALARM_Pin)





#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __BSP_GPIO_H__ */
