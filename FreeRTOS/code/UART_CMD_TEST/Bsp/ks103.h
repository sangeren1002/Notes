/***********************************************************************************
 * 文 件 名   : ks103.h
 * 负 责 人   : jishubao
 * 创建日期   : 2020年1月3日
 * 文件描述   : ks103.c 的头文件
 * 版权说明   : Copyright (c) 2008-2020   杭州国辰机器人科技有限公司
 * 其    他   : 
 * 修改日志   : 
***********************************************************************************/

#ifndef __KS103_H__
#define __KS103_H__
#include "stm32f4xx_hal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define KS103_ADDR_7BIT 0xE0 
#define Read_KS103_SCL() HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_1)
#define ULTRASONIC_NUM 6
#define HW_NUM 2
#define HW_threshold 150 //红外防跌落200mm,红外本身距离地面102mm
#define KS103_START_ADDR_BASE 0xd0
#define KS103_START_ADDR_SPACING 2
#define KS103_START_ADDR_SIZE 20

#define KS103_DEVICE_YEAR_DEFAULT 19
#define KS103_DEVICE_WEEK_DEFAULT 22
typedef struct{
	uint8_t year;
	uint8_t week;
}Ultrasonic_ProductionData_T;
  typedef struct{
	uint8_t addr_7bit;
	uint8_t init_status;
	Ultrasonic_ProductionData_T productionData;
	uint8_t current_status;
	uint8_t recv_control_status;
	uint16_t distance;
}Ultrasonic_STATUS_T;
    typedef struct{
	uint8_t addr_7bit;
	uint8_t init_status;
	Ultrasonic_ProductionData_T productionData;
	uint8_t current_status;
	uint8_t recv_control_status;
	uint16_t distance;
}HW_STATUS_T;
extern Ultrasonic_STATUS_T ultrasonic_sensor[ULTRASONIC_NUM];  
extern HW_STATUS_T hw_sensor[HW_NUM];  
uint8_t KS103_Init(uint8_t *p_year, uint8_t *p_week);
uint8_t KS103_Read_Distance(uint8_t dev_addr,uint16_t *distance);
uint8_t KS103IIC_WriteOneByte(uint8_t dev_write_addr, uint8_t reg,uint8_t data);
uint8_t KS103IIC_ReadOneByte(uint8_t dev_write_addr, uint16_t RegAddr);
uint8_t KS103_AddressLegalJudgment(uint8_t addr);
void PrintfKS103Msg(uint8_t id);
void Change_i2c_addr_KS103(uint8_t old_addr, uint8_t new_addr);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __KS103_H__ */
