/***********************************************************************************
 * 文 件 名   : ks103.c
 * 负 责 人   : jishubao
 * 创建日期   : 2020年1月3日
 * 文件描述   : 导向机电的KS103 iic超声波模块驱动文件
 * 版权说明   : Copyright (c) 2008-2020   杭州国辰机器人科技有限公司
 * 其    他   : 
 * 修改日志   : 
***********************************************************************************/
/*
 *KS101B/KS103/KS103S 默认地址为 0xe8
 *本模块将 I2C 从机地址共 8 位，高 7 位为地址，低 1 位为读写位。
 *控制本模块的主机设备可能只支持 7 位的 I2C 从机地址，此时需要将 8 位地址右移 1 位作为地址来
 *使用。例如，本模块默认地址 0xe8，对应 7 位的地址 0x74
 *
 */
#include "ks103.h"
//#include "bsp_iic.h"
#include "myiic.h"
#include "bsp_usart.h"
#include "delay.h"

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

Ultrasonic_STATUS_T ultrasonic_sensor[ULTRASONIC_NUM];
HW_STATUS_T hw_sensor[HW_NUM];

uint8_t KS103_Init(uint8_t *p_year, uint8_t *p_week)
{
    uint8_t i;
    uint8_t temp_addr=0;
	IIC_Init();
#if 0 //此为硬件IIC2
    *p_year = I2C2_DEV_Read(KS103_ADDR_7BIT, 0x00);
	*p_week = I2C2_DEV_Read(KS103_ADDR_7BIT, 0x01);
#else  //此为软件模拟IIC2
//    Change_i2c_addr_KS103(0xe8, 0xea);
     for(i=0;i<ULTRASONIC_NUM;i++)
    {
            temp_addr = KS103_ADDR_7BIT+i*2;
        	*p_year = KS103IIC_ReadOneByte(temp_addr, 0x00);
            *p_week = KS103IIC_ReadOneByte(temp_addr, 0x01);
            if(*p_year == KS103_DEVICE_YEAR_DEFAULT && *p_week == KS103_DEVICE_WEEK_DEFAULT)
                ultrasonic_sensor[i].init_status = 1;
            else
                 ultrasonic_sensor[i].init_status = 0;
			ultrasonic_sensor[i].productionData.year = *p_year;
			ultrasonic_sensor[i].productionData.week = *p_week;
			ultrasonic_sensor[i].addr_7bit = temp_addr;
			if(ultrasonic_sensor[i].init_status == 1)
				AppPrintf("[ OK ] KS103 manufactured on addr %#02x %d year of %d week is sucess\r\n",temp_addr,*p_year,*p_week);
			else
				AppPrintf("[ Warning ] KS103 manufactured on addr %#02x %d year of %d week is failed\r\n",temp_addr,*p_year,*p_week);
    }
#endif
     return 0;
}

uint8_t KS103_Read_Distance(uint8_t dev_addr,uint16_t *distance)
{
	uint16_t range = 0;
#if 1 //此为软件模拟IIC2
	KS103IIC_WriteOneByte((KS103_ADDR_7BIT+dev_addr*2), 0x02, 0xb0);
	osDelay(40);
	range = KS103IIC_ReadOneByte((KS103_ADDR_7BIT+dev_addr*2), 0x02);
    osDelay(2);
	range<<=8;
	range += KS103IIC_ReadOneByte((KS103_ADDR_7BIT+dev_addr*2), 0x03);
    *distance = range; 
#else  //此为硬件IIC2
	I2C2_DEV_Write(KS103_ADDR_7BIT, 0X02, 0XB0);
	osDelay(40);
	range = I2C2_DEV_Read(KS103_ADDR_7BIT, 0x02);
	range<<=8;
	osDelay(1);
	range += I2C2_DEV_Read(KS103_ADDR_7BIT, 0x03);

//    while (!Read_KS103_SCL())
//    {
//        HAL_Delay(1);
//        cnt++;
//        if(cnt>100)
//            return 1;
//    }
//    *distance = I2C2_DEV_Read(KS103_ADDR_7BIT, 0x02);
//    *distance = (*distance<<8) | I2C2_DEV_Read(KS103_ADDR_7BIT, 0x03);
#endif
//	printf("dev_addr %d range %d mm\r\n",dev_addr,range);


    return 0;
}
void Change_i2c_addr_KS103(uint8_t old_addr, uint8_t new_addr)
{
	HAL_Delay(200);
	KS103IIC_WriteOneByte(old_addr,2,0x9a);
	HAL_Delay(10);
	KS103IIC_WriteOneByte(old_addr,2,0x92);
	HAL_Delay(10);
	KS103IIC_WriteOneByte(old_addr,2,0x9e);
	HAL_Delay(10);
	KS103IIC_WriteOneByte(old_addr,2,new_addr);
}
uint8_t KS103IIC_WriteOneByte(uint8_t dev_write_addr, uint8_t reg,uint8_t data) 				 
{ 
    taskENTER_CRITICAL();
    IIC_Start(); 
	IIC_Send_Byte(dev_write_addr);//发送器件地址+写命令	
	if(IIC_Wait_Ack())	//等待应答
	{
		IIC_Stop();		
        taskEXIT_CRITICAL();        
		return 1;		
	}
    IIC_Send_Byte(reg);	//写寄存器地址
    IIC_Wait_Ack();		//等待应答 
	IIC_Send_Byte(data);//发送数据
	if(IIC_Wait_Ack())	//等待ACK
	{
		IIC_Stop();	 
        taskEXIT_CRITICAL();
		return 1;		 
	}		 
    IIC_Stop();	 
    taskEXIT_CRITICAL();
	return 0;
}
uint8_t KS103IIC_ReadOneByte(uint8_t dev_write_addr, uint16_t RegAddr)
{				  
	uint8_t temp=0;		
	taskENTER_CRITICAL();    
    IIC_Start();  
	IIC_Send_Byte(dev_write_addr);   //发送器件地址0XA0,写数据 	   
	IIC_Wait_Ack(); 
    IIC_Send_Byte(RegAddr);   //发送低地址
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(dev_write_addr+1);           //进入接收模式			   
	IIC_Wait_Ack();	 
	delay_us(50);
    temp=IIC_Read_Byte(0);		   
    IIC_Stop();//产生一个停止条件	
	taskEXIT_CRITICAL();    
	return temp;
}
uint8_t KS103_AddressLegalJudgment(uint8_t addr)
{
	uint8_t cnt = 0;
	for(uint8_t i=0;i<KS103_START_ADDR_SIZE;i++)
		if(addr == KS103_START_ADDR_BASE+i*KS103_START_ADDR_SPACING)
			cnt++;
	return cnt;
}
void PrintfKS103Msg(uint8_t id)
{
	uint8_t i = id -1;
	if(i<ULTRASONIC_NUM&&i>=0)
	{
		if(ultrasonic_sensor[i].init_status == 1)
			AppPrintf("[ OK ] KS103 manufactured on addr %#02x %d year of %d week is sucess\r\n",ultrasonic_sensor[i].addr_7bit,ultrasonic_sensor[i].productionData.year,ultrasonic_sensor[i].productionData.week);
		else
			AppPrintf("[ Warning ] KS103 manufactured on addr %#02x %d year of %d week is failed\r\n",ultrasonic_sensor[i].addr_7bit,ultrasonic_sensor[i].productionData.year,ultrasonic_sensor[i].productionData.week);
	}
	else
	{
		for(i=0;i<ULTRASONIC_NUM;i++)
		{
			if(ultrasonic_sensor[i].init_status == 1)
				AppPrintf("[ OK ] KS103 manufactured on addr %#02x %d year of %d week is sucess\r\n",ultrasonic_sensor[i].addr_7bit,ultrasonic_sensor[i].productionData.year,ultrasonic_sensor[i].productionData.week);
			else
				AppPrintf("[ Warning ] KS103 manufactured on addr %#02x %d year of %d week is failed\r\n",ultrasonic_sensor[i].addr_7bit,ultrasonic_sensor[i].productionData.year,ultrasonic_sensor[i].productionData.week);
		}
	
	}
}
