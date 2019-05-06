# 硬件I2C操作流程

## 1.CUBEMX生成代码后I2C总线初始化一直出现 I2C_FLAG_BUSE
### 1.1 环境：
![packages版本](https://img-blog.csdnimg.cn/20190410162320933.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)
![cubemx版本](https://img-blog.csdnimg.cn/20190410161949166.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)
![KEIL5版本](https://img-blog.csdnimg.cn/2019041016220350.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)
### 1.2 硬件I2C配置为默认，管脚如下 
![硬件I2C管脚](https://img-blog.csdnimg.cn/20190410162451360.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)
### 1.3 时钟等其他 配置完成后生成工程

### 1.4 添加代码

在i2c.c文件中的的void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)函数中添加如下代码
```
	__I2C2_FORCE_RESET();
	__I2C2_RELEASE_RESET();
```
实际如下图
![](https://img-blog.csdnimg.cn/20190410163009916.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)
### 1.5 完美解决I2C总线置位 I2C_FLAG_BUSY
