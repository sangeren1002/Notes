#include "bsp_iap.h"
#include "bsp_stm32flash.h"
#include "string.h"


DEVICE_INFO device_info;
DEVICE_INFO deviceInfoSet;

void Read_Device_info(void)
{
    uint32_t infobuff[sizeof(device_info)];

    STMFLASH_Read(DEVICE_INFO_ADDR_START,infobuff,sizeof(device_info));
    if(infobuff[0] == 1)//已经写入设备信息
    {
		printf("读取固件信息\r\n");
        device_info.flag_update = 1;
        device_info.serialNumber = infobuff[1];
        device_info.DateProduction = infobuff[2];
        memcpy((uint8_t*)device_info.author,(uint8_t*)&infobuff[3],DEVICE_AUTHOR_MAXLEN);
		memcpy((uint8_t*)device_info.email,(uint8_t*)&infobuff[3]+DEVICE_AUTHOR_MAXLEN,DEVICE_EMAIL_MAXLEN);
		memcpy((uint8_t*)device_info.firmware_ver,(uint8_t*)&infobuff[3]+DEVICE_AUTHOR_MAXLEN+DEVICE_EMAIL_MAXLEN,FIRMWARE_VER_MAXLEN);		
        
    }
    else//没有写入设备信息，进行首次信息写入
    {
        printf("未进行出厂设置,请尽快设置固件信息\r\n");
		//SetFirmwareInfo(DEVICE_SERIALNUMBER, DEVICE_DATA, DEVICE_AUTHOR, DEVICE_EMAIL); 
    }
    if(device_info.flag_update)
    {
        printf("\r\n**********      Device Info       **********\r\n");
		device_info.flag_update?(printf("** 已经出厂设置\r\n")):(printf("**未进行出厂设置\r\n"));
        printf("** serialnumber   : %d\r\n", device_info.serialNumber);
        printf("** dateProduction : %d\r\n", device_info.DateProduction);
		printf("** firmware_ver : %s\r\n", device_info.firmware_ver);
        printf("** Author : %s\r\n", (char*)device_info.author);
		printf("** Email : %s\r\n", (char*)device_info.email);
		printf("**********        End             **********\r\n\r\n");
     }
}
int8_t SetFirmwareInfo(uint32_t serialNumber, uint32_t DateProduction, char *author, char *email)
{
	uint32_t buff[sizeof(DEVICE_INFO)];
	
	if(strlen(author) <= DEVICE_AUTHOR_MAXLEN)
	{
		memcpy(deviceInfoSet.author, author,strlen(author));
	}
	else
	{
		printf("[ Warning ] DEVICE_AUTHOR(%d byte) string is out of DEVICE_AUTHOR_MAXLEN(%d byte)\r\n",strlen(author),DEVICE_AUTHOR_MAXLEN); 
		return -1;
	}
	if(strlen(email) <= DEVICE_EMAIL_MAXLEN)
	{
		memcpy(deviceInfoSet.email, email,strlen(email));
	}
	else
	{
		printf("[ Warning ] DEVICE_EMAIL(%d byte) string is out of DEVICE_EMAIL_MAXLEN(%d byte)\r\n",strlen(email),DEVICE_EMAIL_MAXLEN); 
		return -1;
	}
	if(strlen(FIRMWARE_VER) <= FIRMWARE_VER_MAXLEN)
	{
		memcpy(deviceInfoSet.firmware_ver, FIRMWARE_VER,strlen(FIRMWARE_VER));
	}
	else
	{
		printf("[ Warning ] DEVICE_EMAIL(%d byte) string is out of DEVICE_EMAIL_MAXLEN(%d byte)\r\n",strlen(email),DEVICE_EMAIL_MAXLEN); 
		return -1;
	}
	deviceInfoSet.flag_update = DEVICE_FLAG;
	deviceInfoSet.serialNumber = serialNumber;
	deviceInfoSet.DateProduction = DateProduction;
	STMFLASH_Write(DEVICE_INFO_ADDR_START,(uint32_t*)&deviceInfoSet,sizeof(DEVICE_INFO));
	HAL_Delay(10);
	
	STMFLASH_Read(DEVICE_INFO_ADDR_START,buff,sizeof(DEVICE_INFO));
	device_info.flag_update = 1;
	device_info.serialNumber = buff[1];
	device_info.DateProduction = buff[2];

	memcpy((uint8_t*)device_info.author,(uint8_t*)&buff[3],DEVICE_AUTHOR_MAXLEN);
	memcpy((uint8_t*)device_info.email,(uint8_t*)&buff[3]+DEVICE_AUTHOR_MAXLEN,DEVICE_EMAIL_MAXLEN);
	memcpy((uint8_t*)device_info.firmware_ver,(uint8_t*)&buff[3]+DEVICE_AUTHOR_MAXLEN+DEVICE_EMAIL_MAXLEN,FIRMWARE_VER_MAXLEN);		
        
	if(deviceInfoSet.serialNumber == device_info.serialNumber && device_info.DateProduction && deviceInfoSet.DateProduction)
	{
		if(strncmp(deviceInfoSet.author,device_info.author,strlen(deviceInfoSet.author)) ==0 )
		{
			if(strncmp(deviceInfoSet.email,device_info.email,strlen(deviceInfoSet.email)) ==0 )
			{
				if(strncmp(deviceInfoSet.firmware_ver,device_info.firmware_ver,strlen(deviceInfoSet.firmware_ver)) ==0 )
				{
					printf("[ OK ] SetFirmwareInfo is Sucess!\r\n");
					return 0;
				}
				else
				{
					printf("[ Error ] firmware_ver Write Failed!\r\n");
					return -2;
				}
			}
			else
			{
				printf("[ Error ] email Write Failed!\r\n");
				return -3;
			}	
		}
		else
		{
			printf("[ Error ] author Write Failed!\r\n");
			return -4;
		}
	}
	else
	{
		printf("[ Error ] serialNumber or DateProduction Write Failed!\r\n");
		return -5;
	}
}
