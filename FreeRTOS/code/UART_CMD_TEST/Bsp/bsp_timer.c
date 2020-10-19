#include "bsp_timer.h"

void TimerInit(void)
{
	HAL_TIM_Base_Start_IT(&htim6);
}


