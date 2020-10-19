#include "bsp_gpio.h"

void My_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */

  /*Configure GPIO pins : PDPin PDPin */
  GPIO_InitStruct.Pin = BJ_LED_YELLOW_Pin|BJ_LED_RED_Pin|BJ_LED_GREEN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
	
	BJ_Led_YellowOff();
	BJ_Led_RedOff();
	BJ_Led_GreenOff();

}