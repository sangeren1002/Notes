/***********************************************************************************
 * 文 件 名   : bsp_iap.h
 * 负 责 人   : jishubao
 * 创建日期   : 2020年2月27日
 * 文件描述   : bsp_iap.c 的头文件
 * 版权说明   : Copyright (c) 2008-2020   杭州国辰机器人科技有限公司
 * 其    他   : 
 * 修改日志   : 
***********************************************************************************/

#ifndef __BSP_IAP_H__
#define __BSP_IAP_H__
#include "stm32f4xx_hal.h"
#include "string.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */
#define DEVICE_INFO_ADDR_START 0X080E0000

#define DEVICE_FLAG 1
#define DEVICE_SERIALNUMBER 200924002
#define DEVICE_DATA 20200924
#define DEVICE_AUTHOR "jishubao"
#define DEVICE_EMAIL "shubaoji@gmail.com"
#define FIRMWARE_VER "V1.0.1"
#define DEVICE_AUTHOR_MAXLEN 20
#define DEVICE_EMAIL_MAXLEN 24
#define FIRMWARE_VER_MAXLEN 12



typedef struct{
    uint32_t flag_update;               //是否烧写设备信息标志 1:已烧写 
    uint32_t serialNumber;              //设备序列号
    uint32_t DateProduction;            //设备生产日期 0x20200227 表示年月日
    char author[20];     				//作者
	char email[24];     				//电子邮件
	char firmware_ver[12];     			//固件版本
}DEVICE_INFO;


extern DEVICE_INFO device_info;
void Read_Device_info(void);
int8_t SetFirmwareInfo(uint32_t serialNumber, uint32_t DateProduction, char *author, char *email);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __BSP_IAP_H__ */
