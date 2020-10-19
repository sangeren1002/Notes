/***********************************************************************************
 * 文 件 名   : app.h
 * 负 责 人   : jishubao
 * 创建日期   : 2020年9月23日
 * 文件描述   : app.c 的头文件
 * 版权说明   : Copyright (c) 2008-2020   杭州国辰机器人科技有限公司
 * 其    他   : 
 * 修改日志   : 
***********************************************************************************/

#ifndef __APP_H__
#define __APP_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */
#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "sys_define.h"

#define LEDRUN_FRE  1   //系统运行指示灯频率 Hz
#define LEDRUN_CYCLE    1000/(LEDRUN_FRE*2)
typedef struct{
	uint32_t UID[3];
	uint8_t chip_id[12];
	uint16_t flashsize;
	uint16_t package;
}CHIP_INFO;

extern osThreadId defaultTaskHandle;
extern osThreadId DebugCmdTaskHandle;
extern osThreadId LedRunTaskHandle;
extern osThreadId Upload2PCTaskHandle;
extern osThreadId SysInitTaskHandle;
extern osMessageQId U1RecvQueueHandle;
extern osMessageQId U2RecvQueueHandle;
extern osMessageQId U2SendQueueHandle;
extern osMessageQId U1SendQueueHandle;
extern osMutexId PrintfMutexHandle;

extern void ProcessSysInit(void);
extern void ProcessDebugCmdTask(void);
extern void ProcessLedRun(void);

extern void StartSysInitTask(void const * argument);
extern void StartDebugCmdTask(void const * argument);
extern void StartLedRunTask(void const * argument);
extern void StartUpload2PCTask(void const * argument);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __APP_H__ */
