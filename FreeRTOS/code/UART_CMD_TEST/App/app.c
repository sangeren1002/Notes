#include "app.h"
#include "bsp_usart.h"
#include "bsp_gpio.h"
#include "app_debug.h"
#include "string.h"

USART_RECEIVETYPE Usart1AppRecvInit;
USART_RECEIVETYPE Usart2AppRecvInit;

USART_RECEIVETYPE *Usart1AppRecv_p = &Usart1AppRecvInit;
USART_RECEIVETYPE *Usart2AppRecv_p = &Usart2AppRecvInit;
void ProcessSysInit(void)
{
	/* definition and creation of DebugCmdTask */
	osThreadDef(DebugCmdTask, StartDebugCmdTask, osPriorityAboveNormal, 0, 512);
	DebugCmdTaskHandle = osThreadCreate(osThread(DebugCmdTask), NULL);

	/* definition and creation of LedRunTask */
	osThreadDef(LedRunTask, StartLedRunTask, osPriorityNormal, 0, 128);
	LedRunTaskHandle = osThreadCreate(osThread(LedRunTask), NULL);

	/* definition and creation of Upload2PCTask */
	osThreadDef(Upload2PCTask, StartUpload2PCTask, osPriorityAboveNormal, 0, 256);
	Upload2PCTaskHandle = osThreadCreate(osThread(Upload2PCTask), NULL);

    UsartInit();
	printf("[ OK ] 系统初始化完成,开始运行\r\n");
	osThreadSuspend(SysInitTaskHandle);
    osDelay(1);

}
void ProcessLedRun(void)
{
    while(1)
    {
        LedRunToggle();
        osDelay(LEDRUN_CYCLE);
    }
}
void ProcessDebugCmdTask(void)
{

    while(1)
	{
		if(xQueueReceive(U1RecvQueueHandle,&Usart1AppRecv_p,5) == pdTRUE)
		{
	        //AppPrintf("%s",(char*)Usart1AppRecv_p->RX_pData);
			//DebugMy_USART(Usart1MsgRecv_app->RX_pData,Usart1MsgRecv_app->RX_Size);
			
            DebugCmdArgAnalyze((char*)Usart1AppRecv_p->RX_pData, Usart1AppRecv_p->RX_Size);

		}
		osDelay(1);
	}
}


