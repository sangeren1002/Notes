#ifndef _MYIIC_H
#define _MYIIC_H
#include "sys_define.h" 

#define I2C2_BOARD 
#define ADDR_FLOAT_Write 0x72	 
#define ADDR_FLOAT_Read	 0x73
#ifdef I2C3_BOARD  
//IO方向设置
#define SDA_IN()  {GPIOC->MODER&=~(3<<(9*2));GPIOC->MODER|=0<<9*2;}	//PB9输入模式
#define SDA_OUT() {GPIOC->MODER&=~(3<<(9*2));GPIOC->MODER|=1<<9*2;} //PB9输出模式
//IO操作
#define IIC_SCL   PAout(8) //SCL
#define IIC_SDA   PCout(9) //SDA
#define READ_SDA  PCin(9)  //输入SDA
#endif
#ifdef I2C1_BOARD  
//IO方向设置
#define SDA_IN()  {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=0<<9*2;}	//PB9输入模式
#define SDA_OUT() {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=1<<9*2;} //PB9输出模式
//IO操作
#define IIC_SCL   PBout(8) //SCL
#define IIC_SDA   PBout(9) //SDA
#define READ_SDA  PBin(9)  //输入SDA
#endif
#ifdef I2C2_BOARD  
//IO方向设置
#define SDA_IN()  {GPIOF->MODER&=~(3<<(0*2));GPIOF->MODER|=0<<0*2;}	//PB9输入模式
#define SDA_OUT() {GPIOF->MODER&=~(3<<(0*2));GPIOF->MODER|=1<<0*2;} //PB9输出模式
//IO操作
#define IIC_SCL   PFout(1) //SCL
#define IIC_SDA   PFout(0) //SDA
#define READ_SDA  PFin(0)  //输入SDA
#endif
//IIC所有操作函数
void IIC_Init(void);                //初始化IIC的IO口				 
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	 

u8 LightSensorIIC_WriteOneByte(u8 reg,u8 data);
void LightSensorIIC_MultibyteRead(u16 ReadAddr,u8 *pBuffer,u16 NumToRead);
u8 LightSensorIIC_ReadOneByte(u16 ReadAddr);
u32 LightSensorIIC_ReadLenByte(u16 ReadAddr,u8 Len);
u8 MPU_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf);
u8 MPU_Read_Byte(u8 reg);
u8 LightSensor_Read_Byte(u8 reg);
#endif

