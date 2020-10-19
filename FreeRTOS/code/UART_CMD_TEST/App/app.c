#include "app.h"
#include "bsp_usart.h"
#include "bsp_gpio.h"
#include "app_debug.h"
#include "string.h"
#include "bsp_iap.h"
#include "bsp_timer.h"
#include "ks103.h"


CHIP_INFO chip_info;

USART_RECEIVETYPE Usart1AppRecvInit;
USART_RECEIVETYPE Usart2AppRecvInit;

USART_RECEIVETYPE *Usart1AppRecv_p = &Usart1AppRecvInit;
USART_RECEIVETYPE *Usart2AppRecv_p = &Usart2AppRecvInit;
void ProcessSysInit(void)
{
	uint8_t year,week;
    
	My_GPIO_Init();
	KS103_Init(&year,&week);
	/* definition and creation of DebugCmdTask */
	osThreadDef(DebugCmdTask, StartDebugCmdTask, osPriorityAboveNormal, 0, 512);
	DebugCmdTaskHandle = osThreadCreate(osThread(DebugCmdTask), NULL);

	/* definition and creation of LedRunTask */
	osThreadDef(LedRunTask, StartLedRunTask, osPriorityNormal, 0, 128);
	LedRunTaskHandle = osThreadCreate(osThread(LedRunTask), NULL);

	/* definition and creation of Upload2PCTask */
	osThreadDef(Upload2PCTask, StartUpload2PCTask, osPriorityAboveNormal, 0, 256);
	Upload2PCTaskHandle = osThreadCreate(osThread(Upload2PCTask), NULL);
	
	TimerInit();
    UsartInit();
	
	HAL_GetUID(chip_info.UID);
	chip_info.flashsize = (uint16_t)(READ_REG(*((uint32_t *)FLASHSIZE_BASE)));

	AppPrintf("GetUID:%x %x %x\r\n",chip_info.UID[0],chip_info.UID[1],chip_info.UID[2]);
	AppPrintf("FlashSize:%d K\r\n",chip_info.flashsize );
	
	AppPrintf("[ OK ] 系统初始化完成,开始运行\r\n");
	osThreadSuspend(SysInitTaskHandle);
    osDelay(1);

}
void ProcessLedRun(void)
{
	uint32_t i=0;
    while(1)
    {
        LedRunToggle();
		if(i == 1)
		{
			BJ_Led_RedOn();
			BJ_Led_GreenOff();
			BJ_Led_YellowOff();
		}
		else if(i == 2)
		{
			BJ_Led_YellowOn();
			BJ_Led_RedOff();
			BJ_Led_GreenOff();
		}
		else if( i== 3)
		{
			BJ_Led_GreenOn();
			BJ_Led_RedOff();
			BJ_Led_YellowOff();
		}
		else
			i=0;
		i++;

        osDelay(LEDRUN_CYCLE);
    }
}
void ProcessDebugCmdTask(void)
{
	char buff[100];
	static uint32_t cnt=0;
    while(1)
	{
		if(xQueueReceive(U1RecvQueueHandle,&Usart1AppRecv_p,5) == pdTRUE)
		{
	        //AppPrintf("%s",(char*)Usart1AppRecv_p->RX_pData);
			//DebugMy_USART(Usart1MsgRecv_app->RX_pData,Usart1MsgRecv_app->RX_Size);		
            DebugCmdArgAnalyze((char*)Usart1AppRecv_p->RX_pData, Usart1AppRecv_p->RX_Size);
		}
		osDelay(5);
		sprintf(buff,(char*)"test info usart2 send cnt %d\r\n",cnt);
			/* 互斥操作 */
		osMutexWait(U2PrintfMutexHandle,osWaitForever);
		USART2_Send_DMA((uint8_t*)buff,strlen(buff));
		osMutexRelease(U2PrintfMutexHandle);
		cnt++;
	}
}
void ProcessStartUpload2PCTask(void)
{
	
    while(1)
	{
		if(xQueueReceive(U2RecvQueueHandle,&Usart2AppRecv_p,5) == pdTRUE)
		{
	        AppPrintf("%s",(char*)Usart1AppRecv_p->RX_pData);
			osMutexWait(U2PrintfMutexHandle,osWaitForever);
			USART2_Send_DMA(Usart2AppRecv_p->RX_pData, Usart2AppRecv_p->RX_Size);
			osMutexRelease(U2PrintfMutexHandle);
		}
		osDelay(1);
	}
}

