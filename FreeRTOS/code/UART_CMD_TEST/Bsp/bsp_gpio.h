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

#define BJ_LED_YELLOW_CLK_ENABLE()  __HAL_RCC_GPIOG_CLK_ENABLE()
#define BJ_LED_YELLOW_Pin GPIO_PIN_3
#define BJ_LED_YELLOW_GPIO_Port GPIOG

#define BJ_LED_RED_CLK_ENABLE()  __HAL_RCC_GPIOG_CLK_ENABLE()
#define BJ_LED_RED_Pin GPIO_PIN_4
#define BJ_LED_RED_GPIO_Port GPIOG

#define BJ_LED_GREEN_CLK_ENABLE()  __HAL_RCC_GPIOG_CLK_ENABLE()
#define BJ_LED_GREEN_Pin GPIO_PIN_5
#define BJ_LED_GREEN_GPIO_Port GPIOG

#define BJ_Led_YellowToggle()  HAL_GPIO_TogglePin(BJ_LED_YELLOW_GPIO_Port, BJ_LED_YELLOW_Pin)
#define BJ_Led_RedToggle()  HAL_GPIO_TogglePin(BJ_LED_RED_GPIO_Port, BJ_LED_RED_Pin)
#define BJ_Led_GreenToggle()  HAL_GPIO_TogglePin(BJ_LED_GREEN_GPIO_Port, BJ_LED_GREEN_Pin)

#define BJ_Led_YellowOn()  HAL_GPIO_WritePin(BJ_LED_YELLOW_GPIO_Port, BJ_LED_YELLOW_Pin, GPIO_PIN_SET)
#define BJ_Led_RedOn()  HAL_GPIO_WritePin(BJ_LED_RED_GPIO_Port, BJ_LED_RED_Pin, GPIO_PIN_SET)
#define BJ_Led_GreenOn()  HAL_GPIO_WritePin(BJ_LED_GREEN_GPIO_Port, BJ_LED_GREEN_Pin, GPIO_PIN_SET)

#define BJ_Led_YellowOff()  HAL_GPIO_WritePin(BJ_LED_YELLOW_GPIO_Port, BJ_LED_YELLOW_Pin, GPIO_PIN_RESET)
#define BJ_Led_RedOff()  HAL_GPIO_WritePin(BJ_LED_RED_GPIO_Port, BJ_LED_RED_Pin, GPIO_PIN_RESET)
#define BJ_Led_GreenOff()  HAL_GPIO_WritePin(BJ_LED_GREEN_GPIO_Port, BJ_LED_GREEN_Pin, GPIO_PIN_RESET)


extern void My_GPIO_Init(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __BSP_GPIO_H__ */
