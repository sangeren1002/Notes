#include "app_debug.h"
#include "bsp_usart.h"
#include "string.h"
#include "bsp_iap.h"
#include <ctype.h>

/*命令表*/
const cmd_list_struct cmd_list[]={
/*   命令    参数数目    处理函数        帮助信息                         */   
{"hello",   		0,      printf_hello,   		"hello                          - 打印 HelloWorld!\r\n"},
{"arg",     		8,      handle_arg,     		"arg <arg1> <arg2> ...          - 测试用,打印输入的参数\r\n"},
{"help",    		0,      printf_help,    		"help                           - 输出功能菜单\r\n"},
{"taskinfo",		1,      DebugCmdTaskinfo,    	"taskinfo                       - 打印超声设备信息 arg1(1-6)大于6即为所有超声\r\n"},
{"ultprintf",   	1,      DebugCmdUltprintf,    	"ultprintf <arg1>               - 打印超声距离 arg1(1-6)大于6即为所有超声\r\n"},
{"ultmsg",   		1,      DebugCmdUltmsg,    		"ultmsg <arg1>                  - 打印超声设备信息 arg1(1-6)大于6即为所有超声\r\n"},
{"mov",   			2,      DebugCmdMov,    		"mov <arg1> <arg2>              - 控制运动,<arg1>为方向 1前进 2后退 3左转 4右转 <arg2>为整车速度单位cm/s\r\n"},
{"shutdown",   		0,      DebugCmdshutdown,    	"shutdown                       - 执行关机指令\r\n"},
{"reboot",   		0,      DebugCmdReboot,    		"reboot                         - 执行重启指令\r\n"},
{"ks103chaaddr",	2,      DebugCmdks103chaaddr,   "ks103chaaddr <arg1> <arg2>     - 执行修改KS103地址指令,<arg1>旧地址 <arg1>新地址\r\n"},
{"ks103addrprintf",	1,      DebugCmdks103addrprintf,"ks103addrprintf <arg1>         - 打印KS103地址指令,<arg1> id号 缺省则打印所有r\n"},
{"setfirmware",		2,      DebugCmdsetfirmware,	"setfirmware <arg1> <arg2>      - 设置固件信息,<arg1>序列号 <arg2>生产日期 r\n"},
{"readfirmware",	0,      DebugCmdreadfirmware,	"readfirmware <arg1> <arg2>     - 读取固件信息 \r\n"},
{"admin",ADMIN_PASSWORD_LEN,      DebugCmdAdmin,	"admin <arg1> <arg2>     		- 系统管理员权限 \r\n"}
};

cmd_analyze_struct cmd_analyze;
uint8_t adminMode=0;	//0非管理员模式 1管理员模式

void DebugCmdArgAnalyze(char *buff, uint16_t len)
{
   int32_t rec_arg_num;
   
    uint32_t rec_num;
    char cmd_buf[CMD_LEN]; 
	u8 i;
	rec_num = get_true_char_stream(cmd_analyze.processed_buf, (const char*)buff);

    
    /*从接收数据中提取命令*/
     for(i=0;i<CMD_LEN;i++)
     {
         if((i>0)&&((cmd_analyze.processed_buf[i]==' ')||(cmd_analyze.processed_buf[i]==0x0D)))
         {
              cmd_buf[i]='\0';        //字符串结束符
              break;
         }
         else
         {
              cmd_buf[i]=cmd_analyze.processed_buf[i];
         }
     }
     
	rec_arg_num = CmdArgNumAnalyze((char*)buff, rec_num);

            
    for(i=0;i<sizeof(cmd_list)/sizeof(cmd_list_struct);i++)
   {
       if(!strcmp(cmd_buf,cmd_list[i].cmd_name))       //字符串相等
       {
            if(rec_arg_num<0 || rec_arg_num>cmd_list[i].max_args)
            {
                AppPrintf("参数数目过多!\n");
            }
            else
            {
                cmd_list[i].handle(rec_arg_num,(void *)cmd_analyze.cmd_arg);
            }
            break;
       }
       
   }
   if(i>=sizeof(cmd_list)/sizeof(cmd_list_struct))
   {
       AppPrintf("不支持的指令!\n");
   }
}
/**
* 命令参数分析函数,以空格作为一个参数结束,支持输入十六进制数(如:0x15),支持输入负数(如-15)
* @param rec_buf   命令参数缓存区
* @param len       命令的最大可能长度
* @return -1:       参数个数过多,其它:参数个数
*/

s32 CmdArgNumAnalyze(char * rec_buf, u16 len)
{
    uint32_t i;
    uint32_t blank_space_flag=0;    //空格标志
    uint32_t arg_num=0;             //参数数目
    uint32_t index[ARG_NUM];        //有效参数首个数字的数组索引
    
     /*先做一遍分析,找出参数的数目,以及参数段的首个数字所在rec_buf数组中的下标*/
     for(i=0;i<len;i++)
     {
        if(rec_buf[i]==0x20)        //为空格
        {
            blank_space_flag=1;              
            continue;
        }
         else if(rec_buf[i]==0x0D)   //换行
        {
            break;
        }
        else
        {
            if(blank_space_flag==1)
            {
                 blank_space_flag=0; 
                 if(arg_num < ARG_NUM)
                 {
                    index[arg_num]=i;
                     arg_num++;         
                 }
                 else
                 {
                     return -1;      //参数个数太多
                 }
            }
        }
     }
    
     for(i=0;i<arg_num;i++)
     {
         cmd_analyze.cmd_arg[i]=string_to_dec((unsigned char *)(rec_buf+index[i]),len-index[i]);
     }
     return arg_num;

}
/*字符串转10/16进制数*/
int32_t string_to_dec(uint8_t *buf,uint32_t len)
{
   uint32_t i=0;
   uint32_t base=10;       //基数
   int32_t  neg=1;         //表示正负,1=正数
   int32_t  result=0;
   
    if((buf[0]=='0')&&(buf[1]=='x'))
    {
       base=16;
       neg=1;
       i=2;
    }
    else if(buf[0]=='-')
    {
       base=10;
       neg=-1;
       i=1;
    }
    for(;i<len;i++)
    {
       if(buf[i]==0x20 || buf[i]==0x0D)    //为空格
       {
           break;
       }
       
       result *= base;
       if(isdigit(buf[i]))                 //是否为0~9
       {
           result += buf[i]-'0';
       }
       else if(isxdigit(buf[i]))           //是否为a~f或者A~F
       {
            result+=tolower(buf[i])-87;
       }
       else
       {
           result += buf[i]-'0';
       }                                        
    }
   result *= neg;
   
    return result ;
}
/**
* 使用SecureCRT串口收发工具,在发送的字符流中可能带有不需要的字符以及控制字符,
* 比如退格键,左右移动键等等,在使用命令行工具解析字符流之前,需要将这些无用字符以
* 及控制字符去除掉.
* 支持的控制字符有:
*   上移:1B 5B 41
*   下移:1B 5B 42
*   右移:1B 5B 43
*   左移:1B 5B 44
*   回车换行:0D 0A
*  Backspace:08
*  Delete:7F
*/
uint32_t get_true_char_stream(char *dest,const char *src)
{
   uint32_t dest_count=0;
   uint32_t src_count=0;
   
    while(src[src_count]!=0x0D && src[src_count+1]!=0x0A)
    {
       if(isprint(src[src_count]))
       {
           dest[dest_count++]=src[src_count++];
       }
       else
       {
           switch(src[src_count])
           {
                case    0x08:                          //退格键键值
                {
                    if(dest_count>0)
                    {
                        dest_count --;
                    }
                    src_count ++;
                }break;
                case    0x1B:
                {
                    if(src[src_count+1]==0x5B)
                    {
                        if(src[src_count+2]==0x41 || src[src_count+2]==0x42)
                        {
                            src_count +=3;              //上移和下移键键值
                        }
                        else if(src[src_count+2]==0x43)
                        {
                            dest_count++;               //右移键键值
                            src_count+=3;
                        }
                        else if(src[src_count+2]==0x44)
                        {
                            if(dest_count >0)           //左移键键值
                            {
                                dest_count --;
                            }
                           src_count +=3;
                        }
                        else
                        {
                            src_count +=3;
                        }
                    }
                    else
                    {
                        src_count ++;
                    }
                }break;
                default:
                {
                    src_count++;
                }break;
           }
       }
    }
   dest[dest_count++]=src[src_count++];
    dest[dest_count++]=src[src_count++];
    return dest_count;
}

/*打印字符串:Hello world!*/
void printf_hello(int32_t argc,void *cmd_arg)
{
   AppPrintf("Hello world!\n");
}
/*打印每个参数*/
void handle_arg(int32_t argc,void * cmd_arg)
{
   uint32_t i;
   int32_t  *arg=(int32_t *)cmd_arg;
   
    if(argc==0)
    {
       AppPrintf("无参数\n");
    }
    else
    {
       for(i=0;i<argc;i++)
       {
           AppPrintf("第%d个参数:%d\n",i+1,arg[i]);
       }
    }
}
void printf_help(int32_t argc,void * cmd_arg)
{
	uint16_t i;
	
	AppPrintf("\r\n\r\n命令      ******             帮助信息  **********\r\n");
	for(i=0;i<sizeof(cmd_list)/sizeof(cmd_list_struct);i++)
    {
        AppPrintf(cmd_list[i].help);
    }   
}
void DebugCmdTaskinfo(int32_t argc,void * cmd_arg)
{
	 Show_SYS_INFO_Task();
}
void DebugCmdUltprintf(int32_t argc,void * cmd_arg)
{
    uint32_t i;
    int32_t  *arg=(int32_t *)cmd_arg;
    
     if(argc==0)
     {
        AppPrintf("无参数\n");
     }
     else
     {
        for(i=0;i<argc;i++)
        {
            AppPrintf("第%d个参数:%d\n",i+1,arg[i]);
        }
     }

}
void DebugCmdUltmsg(int32_t argc,void * cmd_arg)
{
    uint32_t i;
    int32_t  *arg=(int32_t *)cmd_arg;
    
     if(argc==0)
     {
        AppPrintf("无参数\n");
     }
     else
     {
        for(i=0;i<argc;i++)
        {
            AppPrintf("第%d个参数:%d\n",i+1,arg[i]);
        }
     }

}
void DebugCmdMov(int32_t argc,void * cmd_arg)
{
    uint32_t i;
    int32_t  *arg=(int32_t *)cmd_arg;
    
     if(argc==0)
     {
        AppPrintf("无参数\n");
     }
     else
     {
        for(i=0;i<argc;i++)
        {
            AppPrintf("第%d个参数:%d\n",i+1,arg[i]);
        }
     }

}
void DebugCmdshutdown(int32_t argc,void * cmd_arg)
{
    uint32_t i;
    int32_t  *arg=(int32_t *)cmd_arg;
    
     if(argc==0)
     {
        AppPrintf("无参数\n");
     }
     else
     {
        for(i=0;i<argc;i++)
        {
            AppPrintf("第%d个参数:%d\n",i+1,arg[i]);
        }
     }

}
void DebugCmdReboot(int32_t argc,void * cmd_arg)
{
    uint32_t i;
    int32_t  *arg=(int32_t *)cmd_arg;
    
     if(argc==0)
     {
        AppPrintf("无参数\n");
     }
     else
     {
        for(i=0;i<argc;i++)
        {
            AppPrintf("第%d个参数:%d\n",i+1,arg[i]);
        }
     }

}
void DebugCmdks103chaaddr(int32_t argc,void * cmd_arg)
{
    uint32_t i;
    int32_t  *arg=(int32_t *)cmd_arg;
    
     if(argc==0)
     {
        AppPrintf("无参数\n");
     }
     else
     {
        for(i=0;i<argc;i++)
        {
            AppPrintf("第%d个参数:%d\n",i+1,arg[i]);
        }
     }

}
void DebugCmdks103addrprintf(int32_t argc,void * cmd_arg)
{
    uint32_t i;
    int32_t  *arg=(int32_t *)cmd_arg;
    
     if(argc==0)
     {
        AppPrintf("无参数\n");
     }
     else
     {
        for(i=0;i<argc;i++)
        {
            AppPrintf("第%d个参数:%d\n",i+1,arg[i]);
        }
     }

}
void DebugCmdsetfirmware(int32_t argc,void * cmd_arg)
{
    uint32_t i;
    int32_t  *arg=(int32_t *)cmd_arg;
    
     if(argc==0)
     {
        AppPrintf("无参数\n");
     }
     else
     {
        for(i=0;i<argc;i++)
        {
            AppPrintf("第%d个参数:%d\n",i+1,arg[i]);
        }
     }
	if(adminMode)
		SetFirmwareInfo(arg[0], arg[1], DEVICE_AUTHOR, DEVICE_EMAIL);
	else
		AppPrintf("[ ERROR ] 仅管理员权限可以设置固件信息\n",i+1,arg[i]);
}

void DebugCmdreadfirmware(int32_t argc,void * cmd_arg)
{
    uint32_t i;
    int32_t  *arg=(int32_t *)cmd_arg;
    
     if(argc==0)
     {
        AppPrintf("无参数\n");
     }
     else
     {
        for(i=0;i<argc;i++)
        {
            AppPrintf("第%d个参数:%d\n",i+1,arg[i]);
        }
     }
	Read_Device_info();
}
void DebugCmdAdmin(int32_t argc,void * cmd_arg)
{
    uint32_t i;
	uint8_t pass_cnt = 0;
    int32_t  *arg=(int32_t *)cmd_arg;
	char* password= ADMIN_PASSWORD;
	memcpy(password,ADMIN_PASSWORD,ADMIN_PASSWORD_LEN);
    
     if(argc==0)
     {
        AppPrintf("无参数\n");
     }
     else
     {
        for(i=0;i<argc;i++)
        {
            AppPrintf("第%d个参数:%d\n",i+1,arg[i]);
        }
     }
	 if(argc == ADMIN_PASSWORD_LEN)
	 {
		for(i=0;(i<argc)&&(i<ADMIN_PASSWORD_LEN);i++)
		{
			if(password[i]-'0' == arg[i])
				pass_cnt++;
		}
		if(pass_cnt==ADMIN_PASSWORD_LEN)
		{
			AppPrintf("[ OK ] 进入管理员模式\r\n");
			adminMode=1;
		}
		else
		{
			AppPrintf("[ Error ] 进入管理员模式失败 \r\n");
			adminMode=0;
		}
	 }
	 

}

void Show_SYS_INFO_Task(void)
{
	uint8_t pcWriteBuffer[400];
	AppPrintf("============= START SYSTEM TASK INFO TASK ==========\r\n"); 
	AppPrintf("\r\ntask_name\t\tstate\tprior\tstack\tId\r\n");
	vTaskList((char *)&pcWriteBuffer);
	AppPrintf("%s\r\n", pcWriteBuffer);

	AppPrintf("\r\ntask_name\t\tcnt(10us)\tusage_pec\r\n");
	vTaskGetRunTimeStats((char *)&pcWriteBuffer);
	AppPrintf("pcWriteBuffer len %d\r\n",strlen((char*)pcWriteBuffer));
	AppPrintf("%s\r\n", pcWriteBuffer);
	AppPrintf("============= END SYSTEM TASK INFO TASK ============\r\n"); 

}
