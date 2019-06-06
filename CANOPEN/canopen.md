

## 一、基础理论知识
1. 用户手册与其他文档对比阅读，不下于3遍再尝试    
2. 文档分享于 [百度云链接](https://pan.baidu.com/s/18NJEB3HTK29bpUXLuS1F8w) 密码：1ufg
## 二、 软件代码配置

> **分三步走：**
> - 第一步：先分析资料文档，通过CAN报文分析软件发送响应报文配置电机能工作
> - 第二步：将CAN报文分析软件发送报文用软件实现
> - 第三步：将电机上报方式由主动获取更换为通过主机发送SYNC报文而驱动器配置不同数据不同频率主动上传

### 1. 第1步：

按照示例代码在CANTest软件里发送指令实现对电机的控制
- CANTest软件截图

![在这里插入图片描述](https://img-blog.csdnimg.cn/20190606135834871.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)

> 使用步骤
> - 1、选择设备
> - 2、选择当前要用到的设备（我这是USBCAN1）
> - 3、弹出对话框的波特率选择需要和驱动器设置一致，其他默认即可
>
> ![在这里插入图片描述](https://img-blog.csdnimg.cn/20190606135921621.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)
> - 4、单击“启动”按钮，即可以收到驱动器上报信息（我这是已经配置好SYNC报文的信息）
>
> ![在这里插入图片描述](https://img-blog.csdnimg.cn/20190606135935266.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)
> - 5、在面板左下角的高级操作里就可以发送示例代码报文到CANopen网络里
>
> ![在这里插入图片描述](https://img-blog.csdnimg.cn/20190606135945147.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)

注意选择为标准帧，帧ID即为COBID
示例代码如下：

     **** Enable Motor Power - CiA 402 State Machine ****
     ID DLC Data
     $0603 $8 $2B $40 $60 $00 $06 $00 $00 $00  Ready to Switch on
     $0603 $8 $2B $40 $60 $00 $07 $00 $00 $00  Switched on
     $0603 $8 $2B $40 $60 $00 $0F $01 $00 $00  Operation Enabled; Motion Halted
     **** Set to Profile Velocity Mode ****
     $0603 $8 $2F $60 $60 $00 $03 $00 $00 $00  Set to Profile Velocity Mode
     **** Set Motion Parameters ****
     $0603 $8 $23 $FF $60 $00 $F0 $00 $00 $00  Set Target Velocity to 1 rps
     $0603 $8 $23 $83 $60 $00 $58 $02 $00 $00  Set Acceleration to 100 rps/s
     $0603 $8 $23 $84 $60 $00 $58 $02 $00 $00  Set Deceleration to 100 rps/s
     **** Start/Stop Motion ****
     $0603 $8 $2B $40 $60 $00 $0F $00 $00 $00  Motion Starts
     $0603 $8 $23 $FF $60 $00 $60 $09 $00 $00  Change Target Velocity to 10 rps
     $0603 $8 $2B $40 $60 $00 $0F $01 $00 $00  Motion Halts

### 2. 第2步：
1. 需要将报文信息再数组里定义好

	    //运行数据报文
	    UNS8 data_cmd_ReadSwitch[]      = {0x2B,0x40,0x60,0x00,0x06,0x00,0x00,0x00};   //Ready to Switch on
	    UNS8 data_cmd_SwitchOn[]        = {0x2B,0x40,0x60,0x00,0x07,0x00,0x00,0x00};   //Switched on
	    UNS8 data_cmd_OperationEn[]     = {0x2B,0x40,0x60,0x00,0x0F,0x01,0x00,0x00};    //Operation Enabled; Motion Halted
	    UNS8 data_cmd_VelocityMode[]    = {0x2F,0x60,0x60,0x00,0x03,0x00,0x00,0x00};    //Set to Profile Velocity Mode
	    UNS8 data_cmd_TargetVelocity[]  = {0x23,0xFF,0x60,0x00,0x00,0x00,0x00,0x00};    // Set Target Velocity to 10 rps
	    UNS8 data_cmd_SetAcc[]          = {0x23,0x83,0x60,0x00,0x58,0x02,0x00,0x00};    //Set Acceleration to 100 rps/s
	    UNS8 data_cmd_SetDec[]          = {0x23,0x84,0x60,0x00,0x58,0x02,0x00,0x00};    //Set Deceleration to 100 rps/s
	    UNS8 data_cmd_MotionStart[]     = {0x2B,0x40,0x60,0x00,0x0F,0x00,0x00,0x00};   //Motion Starts
	    UNS8 data_cmd_MotionHalts[]     = {0x2B,0x40,0x60,0x00,0x0F,0x01,0x00,0x00};   //Motion Halts
	    UNS8 data_cmd_GetPosition[]     = {0x43,0x0A,0x70,0x00,0x00,0x00,0x00,0x00};   //Request encoder value
	    UNS8 data_cmd_GetVelActVal[]    = {0x4B,0x09,0x70,0x00,0x00,0x00,0x00,0x00};   //Request velocity_actual_value; The value reading from driver should be                                                                                    //divided 240 to change to rps unit.
	    UNS8 data_cmd_GetCurActValue[]  = {0x4B,0x78,0x60,0x00,0x00,0x00,0x00,0x00};  //current_actual_value;This object is only available on servo/step-servo                                                                                    //drivers.The unit of this object is 0.01Amps
	    UNS8 data_cmd_MotionStatus[]    = {0x4B,0x41,0x60,0x00,0x00,0x00,0x00,0x00};  //Motion_Status

2. 定义不同报文信息的CANMessage结构体



	     Message Can_Msg_ReadSwitch_NO[2];
	     Message Can_Msg_SwitchOn_NO[2];
	     Message Can_Msg_OperationEn_NO[2];
	     Message Can_Msg_VelocityMode_NO[2];
	     Message Can_Msg_TargetVelocity_NO[2];
	     Message Can_Msg_SetAcc_NO[2];
	     Message Can_Msg_SetDec_NO[2];
	     Message Can_Msg_MotionStart_NO[2];
	     Message Can_Msg_MotionHalts_NO[2];


3. 填充CANMessage结构体


	     void SetCanopenParameter(u8 id)
	    	 {
	    	     u8 i;
	    	     /* 填充read switch结构体 */
	    	    Can_Msg_ReadSwitch_NO[id].cob_id = id_motor_no[id];
	    	    Can_Msg_ReadSwitch_NO[id].rtr = rtr_my;
	    	    Can_Msg_ReadSwitch_NO[id].len = len;
	    	    for(i=0;i<Can_Msg_ReadSwitch_NO[id].len;i++)
	    	        Can_Msg_ReadSwitch_NO[id].data[i]  =  data_cmd_ReadSwitch[i];
	    	​
	    	    /* 填充switch on结构体 */
	    	    Can_Msg_SwitchOn_NO[id].cob_id = id_motor_no[id];
	    	    Can_Msg_SwitchOn_NO[id].rtr = rtr_my;
	    	    Can_Msg_SwitchOn_NO[id].len = len;
	    	    for(i=0;i<Can_Msg_SwitchOn_NO[id].len;i++)
	    	        Can_Msg_SwitchOn_NO[id].data[i]  =  data_cmd_SwitchOn[i];
	    	​
	    	    /* 填充OPeration enable 结构体 */
	    	    Can_Msg_OperationEn_NO[id].cob_id = id_motor_no[id];
	    	    Can_Msg_OperationEn_NO[id].rtr = rtr_my;
	    	    Can_Msg_OperationEn_NO[id].len = len;
	    	    for(i=0;i<Can_Msg_OperationEn_NO[id].len;i++)
	    	        Can_Msg_OperationEn_NO[id].data[i]  =  data_cmd_OperationEn[i];
	    	​
	    	    /* 填充 模式选择为速度模式 结构体 */
	    	    Can_Msg_VelocityMode_NO[id].cob_id = id_motor_no[id];
	    	    Can_Msg_VelocityMode_NO[id].rtr = rtr_my;
	    	    Can_Msg_VelocityMode_NO[id].len = len;
	    	    for(i=0;i<Can_Msg_VelocityMode_NO[id].len;i++)
	    	        Can_Msg_VelocityMode_NO[id].data[i] =  data_cmd_VelocityMode[i];
	    	​
	    	    /* 填充 设置加速度 结构体 */
	    	    Can_Msg_SetAcc_NO[id].cob_id = id_motor_no[id];
	    	    Can_Msg_SetAcc_NO[id].rtr = rtr_my;
	    	    Can_Msg_SetAcc_NO[id].len = len;
	    	    for(i=0;i<Can_Msg_SetAcc_NO[id].len;i++)
	    	        Can_Msg_SetAcc_NO[id].data[i] =  data_cmd_SetAcc[i];
	    	​
	    	    /* 填充 设置减速度 结构体 */
	    	    Can_Msg_SetDec_NO[id].cob_id = id_motor_no[id];
	    	    Can_Msg_SetDec_NO[id].rtr = rtr_my;
	    	    Can_Msg_SetDec_NO[id].len = len;
	    	    for(i=0;i<Can_Msg_SetDec_NO[id].len;i++)
	    	        Can_Msg_SetDec_NO[id].data[i] =  data_cmd_SetDec[i];
	    	​
	    	    /* 填充 设置目标速度 结构体 */
	    	    Can_Msg_TargetVelocity_NO[id].cob_id = id_motor_no[id];
	    	    Can_Msg_TargetVelocity_NO[id].rtr = rtr_my;
	    	    Can_Msg_TargetVelocity_NO[id].len = len;
	    	    for(i=0;i<Can_Msg_TargetVelocity_NO[id].len;i++)
	    	        Can_Msg_TargetVelocity_NO[id].data[i] =  data_cmd_TargetVelocity[i];
	    	​
	    	    /* 填充 设置开始运动 结构体 */
	    	    Can_Msg_MotionStart_NO[id].cob_id = id_motor_no[id];
	    	    Can_Msg_MotionStart_NO[id].rtr = rtr_my;
	    	    Can_Msg_MotionStart_NO[id].len = len;
	    	    for(i=0;i<Can_Msg_MotionStart_NO[id].len;i++)
	    	        Can_Msg_MotionStart_NO[id].data[i] =  data_cmd_MotionStart[i];
	    	​
	    	    /* 填充 设置停止 结构体 */
	    	    Can_Msg_MotionHalts_NO[id].cob_id = id_motor_no[id];
	    	    Can_Msg_MotionHalts_NO[id].rtr = rtr_my;
	    	    Can_Msg_MotionHalts_NO[id].len = len;
	    	    for(i=0;i<Can_Msg_MotionHalts_NO[id].len;i++)
	    	        Can_Msg_MotionHalts_NO[id].data[i] =  data_cmd_MotionHalts[i];
	    	​
	    	    /* 填充 请求实际编码器值 结构体 */
	    	    Can_Msg_ActPosition_NO[id].cob_id = id_motor_no[id];
	    	    Can_Msg_ActPosition_NO[id].rtr = rtr_my;
	    	    Can_Msg_ActPosition_NO[id].len = len;
	    	    for(i=0;i<Can_Msg_ActPosition_NO[id].len;i++)
	    	        Can_Msg_ActPosition_NO[id].data[i] =  data_cmd_GetPosition[i];
	    	​
	    	    /* 填充 请求实际速度值 结构体 */
	    	    Can_Msg_ActVelocity_NO[id].cob_id = id_motor_no[id];
	    	    Can_Msg_ActVelocity_NO[id].rtr = rtr_my;
	    	    Can_Msg_ActVelocity_NO[id].len = len;
	    	    for(i=0;i<Can_Msg_ActVelocity_NO[id].len;i++)
	    	        Can_Msg_ActVelocity_NO[id].data[i] =  data_cmd_GetVelActVal[i];

	    	    /* 填充 请求实际电流值 结构体 */
	    	    Can_Msg_ActCurrent_NO[id].cob_id = id_motor_no[id];
	    	    Can_Msg_ActCurrent_NO[id].rtr = rtr_my;
	    	    Can_Msg_ActCurrent_NO[id].len = len;
	    	    for(i=0;i<Can_Msg_ActCurrent_NO[id].len;i++)
	    	        Can_Msg_ActCurrent_NO[id].data[i] =  data_cmd_GetCurActValue[i];
    	​
    	    /* 填充 驱动器状态 结构体 */
    	    Can_Msg_MotionStatus_NO[id].cob_id = id_motor_no[id];
    	    Can_Msg_MotionStatus_NO[id].rtr = rtr_my;
    	    Can_Msg_MotionStatus_NO[id].len = len;
    	    for(i=0;i<Can_Msg_MotionStatus_NO[id].len;i++)
    	        Can_Msg_MotionStatus_NO[id].data[i] =  data_cmd_MotionStatus[i];
    	​

    	    /* 填充 驱动器id的TPDO传输类型 结构体 */
    	     Can_Msg_TPDOTRANTYPE_NO[id][0].cob_id = id_motor_no[id];
    	     Can_Msg_TPDOTRANTYPE_NO[id][0].rtr = rtr_my;
    	     Can_Msg_TPDOTRANTYPE_NO[id][0].len = len;
    	     for(i=0;i<Can_Msg_TPDOTRANTYPE_NO[id][0].len;i++)
    	         Can_Msg_TPDOTRANTYPE_NO[id][0].data[i] = data_cmd_ChangeTPDOTransType180x[0][i];

    	        /* 填充 驱动器 id 的TPDO传输类型 结构体 */
    	     Can_Msg_TPDOTRANTYPE_NO[id][1].cob_id = id_motor_no[id];
    	     Can_Msg_TPDOTRANTYPE_NO[id][1].rtr = rtr_my;
    	     Can_Msg_TPDOTRANTYPE_NO[id][1].len = len;
    	     for(i=0;i<Can_Msg_TPDOTRANTYPE_NO[id][1].len;i++)
    	         Can_Msg_TPDOTRANTYPE_NO[id][1].data[i] =data_cmd_ChangeTPDOTransType180x[1][i];
    	​
    	         /* 填充 驱动器 id 的状态切换到运行状态 结构体 */
    	     Can_Msg_ChangeMotionStatusToOperation[id].cob_id = 0x00;
    	     Can_Msg_ChangeMotionStatusToOperation[id].rtr = rtr_my;
    	     Can_Msg_ChangeMotionStatusToOperation[id].len = 2;
    	     for(i=0;i<Can_Msg_TPDOTRANTYPE_NO[id][1].len;i++)
    	         Can_Msg_ChangeMotionStatusToOperation[id].data[i] =data_cmd_ChangeMotionStatusToOperation[id][i];
    	​
    	         /* 填充 驱动器 id 关闭TPDO1 结构体 */
    	     Can_Msg_Turn_Off_TPDOX[id][0].cob_id = id_motor_no[id];
    	     Can_Msg_Turn_Off_TPDOX[id][0].rtr = rtr_my;
    	     Can_Msg_Turn_Off_TPDOX[id][0].len = len;
    	     for(i=0;i<Can_Msg_Turn_Off_TPDOX[id][0].len;i++)
    	         Can_Msg_Turn_Off_TPDOX[id][0].data[i] =data_cmd_Turn_Off_TPDOX[0][i];  

    	      /* 填充 驱动器 id 关闭TPDO2 结构体 */
    	     Can_Msg_Turn_Off_TPDOX[id][1].cob_id = id_motor_no[id];
    	     Can_Msg_Turn_Off_TPDOX[id][1].rtr = rtr_my;
    	     Can_Msg_Turn_Off_TPDOX[id][1].len = len;
    	     for(i=0;i<Can_Msg_Turn_Off_TPDOX[id][1].len;i++)
    	         Can_Msg_Turn_Off_TPDOX[id][1].data[i] =data_cmd_Turn_Off_TPDOX[1][i];   
    	​
    	         /* 填充 驱动器 id 设置0X1A00映射对象个数为0 结构体 */
    	     Can_Msg_Set_Number_Mapped_objectsTo0[id][0].cob_id = id_motor_no[id];
    	     Can_Msg_Set_Number_Mapped_objectsTo0[id][0].rtr = rtr_my;
    	     Can_Msg_Set_Number_Mapped_objectsTo0[id][0].len = len;
    	     for(i=0;i<Can_Msg_Set_Number_Mapped_objectsTo0[id][0].len;i++)
    	         Can_Msg_Set_Number_Mapped_objectsTo0[id][0].data[i] =data_cmd_Set_Number_Mapped_objectsTo0[0][i];  

    	      /* 填充 驱动器 id 设置0X1A01映射对象个数为0 结构体 */
    	     Can_Msg_Set_Number_Mapped_objectsTo0[id][1].cob_id = id_motor_no[id];
    	     Can_Msg_Set_Number_Mapped_objectsTo0[id][1].rtr = rtr_my;
    	     Can_Msg_Set_Number_Mapped_objectsTo0[id][1].len = len;
    	     for(i=0;i<Can_Msg_Set_Number_Mapped_objectsTo0[id][1].len;i++)
    	         Can_Msg_Set_Number_Mapped_objectsTo0[id][1].data[i] =data_cmd_Set_Number_Mapped_objectsTo0[1][i];

    	    /* 填充 驱动器 id 设置编码器值映射到TPDO1的子索引01 4Bytes 结构体 */
    	     Can_Msg_Map_Actual_value_to_TPDOX_objectX[id][0].cob_id = id_motor_no[id];
    	     Can_Msg_Map_Actual_value_to_TPDOX_objectX[id][0].rtr = rtr_my;
    	     Can_Msg_Map_Actual_value_to_TPDOX_objectX[id][0].len = len;
    	     for(i=0;i<Can_Msg_Map_Actual_value_to_TPDOX_objectX[id][0].len;i++)
    	         Can_Msg_Map_Actual_value_to_TPDOX_objectX[id][0].data[i] =data_cmd_Map_Actual_value_to_TPDOX_objectX[0][i];    

    	      /* 填充 驱动器 id //设置速度值映射到TPDO1的子索引02                  2Bytes 结构体 */
    	     Can_Msg_Map_Actual_value_to_TPDOX_objectX[id][1].cob_id = id_motor_no[id];
    	     Can_Msg_Map_Actual_value_to_TPDOX_objectX[id][1].rtr = rtr_my;
    	     Can_Msg_Map_Actual_value_to_TPDOX_objectX[id][1].len = len;
    	     for(i=0;i<Can_Msg_Map_Actual_value_to_TPDOX_objectX[id][1].len;i++)
    	         Can_Msg_Map_Actual_value_to_TPDOX_objectX[id][1].data[i] =data_cmd_Map_Actual_value_to_TPDOX_objectX[1][i];

    	     /* 填充 驱动器 id 设置状态字映射到TPDO2的子索引01 4Bytes 结构体 */
    	      Can_Msg_Map_Actual_value_to_TPDOX_objectX[id][2].cob_id = id_motor_no[id];
    	      Can_Msg_Map_Actual_value_to_TPDOX_objectX[id][2].rtr = rtr_my;
    	      Can_Msg_Map_Actual_value_to_TPDOX_objectX[id][2].len = len;
    	      for(i=0;i<Can_Msg_Map_Actual_value_to_TPDOX_objectX[id][2].len;i++)
    	          Can_Msg_Map_Actual_value_to_TPDOX_objectX[id][2].data[i] =data_cmd_Map_Actual_value_to_TPDOX_objectX[2][i];    

    	       /* 填充 驱动器 id //设置电流值映射到TPDO2的子索引02                  2Bytes 结构体 */
    	      Can_Msg_Map_Actual_value_to_TPDOX_objectX[id][3].cob_id = id_motor_no[id];
    	      Can_Msg_Map_Actual_value_to_TPDOX_objectX[id][3].rtr = rtr_my;
    	      Can_Msg_Map_Actual_value_to_TPDOX_objectX[id][3].len = len;
    	      for(i=0;i<Can_Msg_Map_Actual_value_to_TPDOX_objectX[id][3].len;i++)
    	          Can_Msg_Map_Actual_value_to_TPDOX_objectX[id][3].data[i] =data_cmd_Map_Actual_value_to_TPDOX_objectX[3][i];
    	​
    	      /* 填充 驱动器 id 设置0X1A00映射对象个数为2 结构体 */
    	      Can_Msg_Set_Number_Mapped_objectsTo2[id][0].cob_id = id_motor_no[id];
    	      Can_Msg_Set_Number_Mapped_objectsTo2[id][0].rtr = rtr_my;
    	      Can_Msg_Set_Number_Mapped_objectsTo2[id][0].len = len;
    	      for(i=0;i<Can_Msg_Set_Number_Mapped_objectsTo2[id][0].len;i++)
    	          Can_Msg_Set_Number_Mapped_objectsTo2[id][0].data[i] =data_cmd_Set_Number_Mapped_objectsTo2[0][i];  

    	       /* 填充 驱动器 id 设置0X1A01映射对象个数为2 结构体 */
    	      Can_Msg_Set_Number_Mapped_objectsTo2[id][1].cob_id = id_motor_no[id];
    	      Can_Msg_Set_Number_Mapped_objectsTo2[id][1].rtr = rtr_my;
    	      Can_Msg_Set_Number_Mapped_objectsTo2[id][1].len = len;
    	      for(i=0;i<Can_Msg_Set_Number_Mapped_objectsTo2[id][1].len;i++)
    	          Can_Msg_Set_Number_Mapped_objectsTo2[id][1].data[i] =data_cmd_Set_Number_Mapped_objectsTo2[1][i];
    	​
    	          /* 填充 驱动器 id 打开TPDO1 结构体 */
    	      Can_Msg_Turn_On_TPDOX[id][0].cob_id = id_motor_no[id];
    	      Can_Msg_Turn_On_TPDOX[id][0].rtr = rtr_my;
    	      Can_Msg_Turn_On_TPDOX[id][0].len = len;
    	      for(i=0;i<Can_Msg_Turn_On_TPDOX[id][0].len;i++)
    	          Can_Msg_Turn_On_TPDOX[id][0].data[i] =data_cmd_Turn_On_TPDOX[0][i];  

    	       /* 填充 驱动器 id 打开TPDO2 结构体 */
    	      Can_Msg_Turn_On_TPDOX[id][1].cob_id = id_motor_no[id];
    	      Can_Msg_Turn_On_TPDOX[id][1].rtr = rtr_my;
    	      Can_Msg_Turn_On_TPDOX[id][1].len = len;
    	      for(i=0;i<Can_Msg_Turn_On_TPDOX[id][1].len;i++)
    	          Can_Msg_Turn_On_TPDOX[id][1].data[i] =data_cmd_Turn_On_TPDOX[1][i];   

    	      /* 填充 驱动器 id 打开TPDO2 结构体 */
    	      Can_Msg_SYNCMY.cob_id = 0X0080;
    	      Can_Msg_SYNCMY.rtr = rtr_my;
    	      Can_Msg_SYNCMY.len = 0;
    	 }

4. 通过CANSend函数发送出去




	     canSend(CAN1,&Can_Msg_ReadSwitch_NO[0]);
	        	  delay_ms(2);
	     canSend(CAN1,&Can_Msg_ReadSwitch_NO[1]);
	        	 delay_ms(2);

    	    canSend(CAN1,&Can_Msg_SwitchOn_NO[0]);
    	        delay_ms(2);
    	    canSend(CAN1,&Can_Msg_SwitchOn_NO[1]);
    	        delay_ms(2);
    	   canSend(CAN1,&Can_Msg_OperationEn_NO[1]);
    	       delay_ms(2);    
    	   canSend(CAN1,&Can_Msg_VelocityMode_NO[0]);
    	       delay_ms(2);   
    	   canSend(CAN1,&Can_Msg_VelocityMode_NO[1]);
    	       delay_ms(2);    
    	​
    	    canSend(CAN1,&Can_Msg_OperationEn_NO[0]);
    	        delay_ms(2);
    	    canSend(CAN1,&Can_Msg_OperationEn_NO[1]);
    	        delay_ms(2);    
    	    canSend(CAN1,&Can_Msg_VelocityMode_NO[0]);
    	        delay_ms(2);   
    	    canSend(CAN1,&Can_Msg_VelocityMode_NO[1]);
    	        delay_ms(2);    
    	    canSend(CAN1,&Can_Msg_SetAcc_NO[0]);
    	        delay_ms(2);
    	    canSend(CAN1,&Can_Msg_SetAcc_NO[1]);
    	        delay_ms(2);
    	    canSend(CAN1,&Can_Msg_SetDec_NO[0]);
    	        delay_ms(2);
    	    canSend(CAN1,&Can_Msg_SetDec_NO[1]);
    	        delay_ms(2);

    	    canSend(CAN1,&Can_Msg_TargetVelocity_NO[0]);
    	        delay_ms(2);
    	    canSend(CAN1,&Can_Msg_TargetVelocity_NO[1]);
    	        delay_ms(2);        
    	    canSend(CAN1,&Can_Msg_MotionStart_NO[0]);  //Start/Stop Motion
    	        delay_ms(2);
    	    canSend(CAN1,&Can_Msg_MotionStart_NO[1]);  //Start/Stop Motion
    	        delay_ms(2);


至此既可以在代码里实现CAN报文示例里的功能，但是驱动器上报的信息是需要主动去获取的，而且没有配置映射信息主机是无法解析出来的。第一步只是熟悉报文格式，第二步了解是如何将报文以代码一定的格式发送出去，这都是为第三步的整合代码优化作为铺垫。
### 3. 第三步：
设置从机主动上报信息主要分为两部分
1、主机通过SDO配置从机的PDO通讯参数和映射参数以及传输类型（主要是配置接收的SYNC报文生效个数），目的是讲寄存器信息通过映射将信息以通讯参数规定的报文形式发送出去。
2、主机配置本地的PDO通讯参数和映射参数，目的是解析出对应的报文信息
我们需要的信息有：编码器信息（4字节）、速度信息（2字节）、状态字信息（2字节）、电流值信息（2字节）。一个PDO报文最大为8字节，这样就可以将编码器与速度放在一个PDO报文里，状态字与电流值放在一个PDO报文里。这样就是需要配置1个驱动器的两个TPDO，2个驱动器共4个TPDO，主机就有4个RPDO接收这我们需要的四个信息。
这里有几个问题？

> 1、映射地址在哪？ 2、如何映射？

如何查找映射地址：
因为我们设置的是驱动器的TPDO通讯参数，我们在供应商的CANopen用户手册里找到对象字典里的TPDO communication parameter地址(0x1800~0x1803 )

![在这里插入图片描述](https://img-blog.csdnimg.cn/20190606140947341.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)

从手册可以看出从机有4个TPDO，4*8=32字节足以配置发送很多信息。
我们这里采用配置TPDO2（编码器&速度）和TPDO3（状态字&电流）。
我们需要配置的信息有索引0X1801的子索引01&02和索引0X1802的子索引01&02，其他子索引可以根据需要配置。
0X1801的子索引01的值改为0X280+NODEID（驱动器地址为1和2，这里就是0X281，0X282）
0X1801的子索引02的值改为1（即接收到1个SYNC及生效将发送该TPDO的信息：编码器与速度）
0X1802的子索引01的值改为0X380+NODEID（驱动器地址为1和2，这里就是0X281，0X382）
0X1801的子索引02的值改为2（即接收到2个SYNC及生效将发送该TPDO的信息：状态字与电流值）
将这两个驱动器每个各两条报文共计4条报文通过SDO报文发送出去


    /* 设置驱动器的SDO通讯参数中的传输类型 */
        canSend(CAN1,&Can_Msg_TPDOTRANTYPE_NO[0][0]);//0X1801 01
           delay_ms(2);
        canSend(CAN1,&Can_Msg_TPDOTRANTYPE_NO[0][1]);//0X1801 02
           delay_ms(2);
        canSend(CAN1,&Can_Msg_TPDOTRANTYPE_NO[1][0]);//0X1802 01
           delay_ms(2);
        canSend(CAN1,&Can_Msg_TPDOTRANTYPE_NO[1][1]);//0X1802 02
           delay_ms(2);

这样就配置好了驱动器的TPDO（TPDO2/TPDO3）通讯参数。

> 如何映射？

下面开始配置驱动器的TPDO映射参数
首先在用户手册里找到TPDO映射参数的地址（0x1A00 ~ 0x1A03 ）

![在这里插入图片描述](https://img-blog.csdnimg.cn/20190606141011393.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)

我们只有两条TPDO报文（TPDO2&TPDO3）,这样我们只要配置TPDO2(0X1A01)和TPDO3(0X1A02)。
TPDO2(0X1A01)：子索引00（映射对象个数）、子索引01（编码器&速度）；
TPDO3(0X1A02)：子索引00（映射对象个数）、子索引01（状态字&电流值）；

![在这里插入图片描述](https://img-blog.csdnimg.cn/20190606141034923.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)

通过手册的示例步骤我们知道，还需要找到编码器、速度、状态字、电流对应的Value
在手册里找到如下四条信息

![在这里插入图片描述](https://img-blog.csdnimg.cn/20190606141101361.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)

![在这里插入图片描述](https://img-blog.csdnimg.cn/20190606141107768.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)

![在这里插入图片描述](https://img-blog.csdnimg.cn/20190606141116580.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)

![在这里插入图片描述](https://img-blog.csdnimg.cn/20190606141123926.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)

这样我们就知道了对应的Value应该填写多少


     /* 设置驱动器映射参数Message */
    	 Message Can_Msg_Turn_Off_TPDOX[2][2];
    	 Message Can_Msg_Set_Number_Mapped_objectsTo0[2][2];
    	 Message Can_Msg_Map_Actual_value_to_TPDOX_objectX[2][4];
    	 Message Can_Msg_Set_Number_Mapped_objectsTo2[2][2];
    	 Message Can_Msg_Turn_On_TPDOX[2][2];
    	//驱动器TPDO映射参数设置
    	UNS8 data_cmd_Turn_Off_TPDOX[2][8] = {{0x23,0x01,0x18,0x01,0x80,0x02,0x00,0x80},//关闭TPDO2
    	                                      {0x23,0x02,0x18,0x01,0x80,0x03,0x00,0x80}//关闭TPDO3
    	};
    	UNS8 data_cmd_Set_Number_Mapped_objectsTo0[2][8] = {{0x2F,0x01,0x1A,0x00,0x00,0x00,0x00,0x00},//设置0X1A01映射对象个数为0
    	                                                    {0x2F,0x02,0x1A,0x00,0x00,0x00,0x00,0x00}//设置0X1A02映射对象个数为0
    	};
    	UNS8 data_cmd_Map_Actual_value_to_TPDOX_objectX[4][8] = {{0x23,0x01,0x1A,0x01,0x20,0x00,0x0A,0x70},//设置编码器值映射到TPDO1的子索引01              4Bytes
    	                                                         {0x23,0x01,0x1A,0x02,0x10,0x00,0x09,0x70},//设置速度值映射到TPDO1的子索引02                2Bytes
    	                                                         {0x23,0x02,0x1A,0x01,0x10,0x00,0x41,0x60},//设置状态字映射到TPDO2的子索引01                2Bytes
    	                                                         {0x23,0x02,0x1A,0x02,0x10,0x00,0x78,0x60}//设置电流值映射到TPDO2的子索引02                 2Bytes
    	  };
    	UNS8 data_cmd_Set_Number_Mapped_objectsTo2[2][8] = {{0x2F,0x01,0x1A,0x00,0x02,0x00,0x00,0x00},//设置0X1A01映射对象个数为2
    	                                                    {0x2F,0x02,0x1A,0x00,0x02,0x00,0x00,0x00}//设置0X1A02映射对象个数为2
    	};
    	UNS8 data_cmd_Turn_On_TPDOX[2][8] = {{0x23,0x01,0x18,0x01,0x80,0x02,0x00,0x00},//打开TPDO2
    	                                     {0x23,0x02,0x18,0x01,0x80,0x03,0x00,0x00}//打开TPDO3
    	};

通过之前实现的SDO设置电机信息的形式，将按照以下步骤配置TPDO通讯参数

![在这里插入图片描述](https://img-blog.csdnimg.cn/20190606141154871.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)

填充数据报文参考第一步
将填充好的报文发送出去，注意一定要在预操作模式下。在操作模式下是不能配置TPDO参数的.


    /* 设置驱动器的SDO映射参数 */
    	   for(i=0;i<2;i++)
    	   {
    	        j=0;
    	       canSend(CAN1,&Can_Msg_Turn_Off_TPDOX[i][j]);
    	       delay_ms(2);
    	       j++;
    	       canSend(CAN1,&Can_Msg_Turn_Off_TPDOX[i][j]);
    	       delay_ms(2);
    	   }
    	   for(i=0;i<2;i++)
    	   {
    	        j=0;
    	       canSend(CAN1,&Can_Msg_Set_Number_Mapped_objectsTo0[i][j]);
    	       delay_ms(2);
    	       j++;
    	       canSend(CAN1,&Can_Msg_Set_Number_Mapped_objectsTo0[i][j]);
    	       delay_ms(2);
    	   }
    	   for(i=0;i<2;i++)
    	   {
    	        j=0;
    	       canSend(CAN1,&Can_Msg_Map_Actual_value_to_TPDOX_objectX[i][j]);
    	       delay_ms(2);
    	       j++;
    	       canSend(CAN1,&Can_Msg_Map_Actual_value_to_TPDOX_objectX[i][j]);
    	       delay_ms(2);
    	       j++;
    	       canSend(CAN1,&Can_Msg_Map_Actual_value_to_TPDOX_objectX[i][j]);
    	       delay_ms(2);
    	       j++;
    	       canSend(CAN1,&Can_Msg_Map_Actual_value_to_TPDOX_objectX[i][j]);
    	       delay_ms(2);
    	   }
    	   for(i=0;i<2;i++)
    	   {
    	        j=0;
    	       canSend(CAN1,&Can_Msg_Set_Number_Mapped_objectsTo2[i][j]);
    	       delay_ms(2);
    	       j++;
    	       canSend(CAN1,&Can_Msg_Set_Number_Mapped_objectsTo2[i][j]);
    	       delay_ms(2);
    	   }
    	   for(i=0;i<2;i++)
    	   {
    	        j=0;
    	       canSend(CAN1,&Can_Msg_Turn_On_TPDOX[i][j]);
    	       delay_ms(2);
    	       j++;
    	       canSend(CAN1,&Can_Msg_Turn_On_TPDOX[i][j]);
    	       delay_ms(2);
    	   }

1、在驱动器的预操作状态下，首先发送关闭TPDO2的报文
2、设置TPDO2的映射参数对象个数为0
3、设置TPDO3的映射参数对象个数为0
4、设置TPDO2四个映射参数信息
5、设置TPDO3四个映射参数信息
6、设置TPDO2的对象个数为2
7、设置TPDO3的对象个数为2
8、打开TPDO2的报文
9、打开TPDO3的报文
配置主机本地的对象词典的RPDO用以匹配从机的对象词典中的TPDO

![在这里插入图片描述](https://img-blog.csdnimg.cn/20190606141217988.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)

我们可以看到
0-7bit表示数据长度
8-15bit表示对象词典的子索引
16-31bit表示对象词典的索引

> 例如： 编码器值(4字节)=32位等于十六进制0X20 索引为0X700A，子索引为0X00 则映射内容值为： 0x700A0020
>
> 速度(2字节)=16位等于十六进制0X10 索引为0X7009，子索引为0X00 则映射内容值为： 0x70090010
>
> 状态字(2字节)=16位等于十六进制0X10 索引为0X6041，子索引为0X00 则映射内容值为： 0x60410010
>
> 电流(2字节)=16位等于十六进制0X10 索引为0X6078，子索引为0X00 则映射内容值为： 0x60780020


代码结构如下：

    /* index 0x1600 :   Receive PDO 1 Mapping. */
                        UNS8 ObjDict_highestSubIndex_obj1600 = 2; /* number of subindex - 1*/
                        UNS32 ObjDict_obj1600[] =
                        {
                          0x70090010,   /* 536870913 */
                          0x700A0020,   /* 536936449 */
                        };
                        subindex ObjDict_Index1600[] =
                         {
                           { RW, uint8, sizeof (UNS8), (void*)&ObjDict_highestSubIndex_obj1600 },
                           { RW, uint32, sizeof (UNS32), (void*)&ObjDict_obj1600[0] },
                           { RW, uint32, sizeof (UNS32), (void*)&ObjDict_obj1600[1] },
                         };
    /* index 0x1601 :   Receive PDO 2 Mapping. */
                        UNS8 ObjDict_highestSubIndex_obj1601 = 2; /* number of subindex - 1*/
                        UNS32 ObjDict_obj1601[] =
                        {
                          0x60410010,   /* 536870913 */
                          0x60780010,   /* 536936449 */
                        };
                        subindex ObjDict_Index1601[] =
                         {
                           { RW, uint8, sizeof (UNS8), (void*)&ObjDict_highestSubIndex_obj1601 },
                           { RW, uint32, sizeof (UNS32), (void*)&ObjDict_obj1601[0] },
                           { RW, uint32, sizeof (UNS32), (void*)&ObjDict_obj1601[1] },
                         };

配置主机本地的RPDO映射参数
主机需要接受两个驱动器各2条TPDO共计4个TPDO，则需要4个RPDO来解析。
RPDO1解析ID为1的编码器&速度报文内容
RPDO2解析ID为1的状态字&电流值报文内容
RPDO3解析ID为2的编码器&速度报文内容
RPDO3解析ID为1的状态字&电流值报文内容

    void Master_initialisation(CO_Data* d)
    {
        UNS32 RPDO1_COBID = 0x0181; //NODEID+0X180H
        UNS32 RPDO2_COBID = 0x0281;//NODEID+0X200H
        UNS32 RPDO3_COBID = 0x0182;
        UNS32 RPDO4_COBID = 0x0282;
        UNS32 size = sizeof(UNS32);
        UNS8 unsize = sizeof(unsize);
        UNS8  TpdoTransmissionType = 0;//receiving is synchronous, process after next reception of SYNC object

        TpdoTransmissionType = TpdoTransmissionType;
        SEGGER_RTT_printf(0, "Master_initialisation\n");
    ​
        /*****************************************
         * Define RPDOs to match slave ID=1 TPDOs*
         *****************************************/
        writeLocalDict( d, /*CO_Data* d*/
                0x1400, /*UNS16 index*/
                0x01, /*UNS8 subind*/
                &RPDO3_COBID, /*void * pSourceData,*/
                &size, /* UNS8 * pExpectedSize*/
                RW);  /* UNS8 checkAccess */
        TpdoTransmissionType = TRANS_SYNC_ACYCLIC;  
        writeLocalDict( d, /*CO_Data* d*/
                0x1400, /*UNS16 index*/
                0x02, /*UNS8 subind*/
                &TpdoTransmissionType, /*void * pSourceData,*/
                (UNS32*)&unsize, /* UNS8 * pExpectedSize*/
                RW);  /* UNS8 checkAccess */

        writeLocalDict( d, /*CO_Data* d*/
                0x1401, /*UNS16 index*/
                0x01, /*UNS8 subind*/
                &RPDO4_COBID, /*void * pSourceData,*/
                &size, /* UNS8 * pExpectedSize*/
                RW);  /* UNS8 checkAccess */
        TpdoTransmissionType = TRANS_SYNC_ACYCLIC;  
        writeLocalDict( d, /*CO_Data* d*/
                0x1401, /*UNS16 index*/
                0x02, /*UNS8 subind*/
                &TpdoTransmissionType, /*void * pSourceData,*/
                (UNS32*)&unsize, /* UNS8 * pExpectedSize*/
                RW);  /* UNS8 checkAccess */
        /*****************************************
         * Define RPDOs to match slave ID=2 TPDOs*
         *****************************************/
        writeLocalDict( d, /*CO_Data* d*/
                0x1400, /*UNS16 index*/
                0x01, /*UNS8 subind*/
                &RPDO1_COBID, /*void * pSourceData,*/
                &size, /* UNS8 * pExpectedSize*/
                RW);  /* UNS8 checkAccess */
        TpdoTransmissionType = TRANS_SYNC_ACYCLIC;  
        writeLocalDict( d, /*CO_Data* d*/
                0x1400, /*UNS16 index*/
                0x02, /*UNS8 subind*/
                &TpdoTransmissionType, /*void * pSourceData,*/
                (UNS32*)&unsize, /* UNS8 * pExpectedSize*/
                RW);  /* UNS8 checkAccess */

        writeLocalDict( d, /*CO_Data* d*/
                0x1401, /*UNS16 index*/
                0x01, /*UNS8 subind*/
                &RPDO2_COBID, /*void * pSourceData,*/
                &size, /* UNS8 * pExpectedSize*/
                RW);  /* UNS8 checkAccess */
        TpdoTransmissionType = TRANS_SYNC_ACYCLIC;  
        writeLocalDict( d, /*CO_Data* d*/
                0x1401, /*UNS16 index*/
                0x02, /*UNS8 subind*/
                &TpdoTransmissionType, /*void * pSourceData,*/
                (UNS32*)&unsize, /* UNS8 * pExpectedSize*/
                RW);  /* UNS8 checkAccess */
    }

在主机主函数里将配置信息按照顺序即可实现主从机配置，在主机while循环里可以间隔一定时间发送SYNC报文，每发生一个就会收到0X281和0X282报文（包含编码器&速度信息），每发送两次SYNC报文就会收到0X381和0X382报文（包含状态字&电流值）

    void MotorInit(void)
    {
        u8 i,j;

        canSend(CAN1,&Can_Msg_ReadSwitch_NO[0]);
            delay_ms(2);
        canSend(CAN1,&Can_Msg_ReadSwitch_NO[1]);
            delay_ms(2);

        canSend(CAN1,&Can_Msg_SwitchOn_NO[0]);
            delay_ms(2);
        canSend(CAN1,&Can_Msg_SwitchOn_NO[1]);
            delay_ms(2);

        /* 设置驱动器的SDO通讯参数中的传输类型 */
        canSend(CAN1,&Can_Msg_TPDOTRANTYPE_NO[0][0]);
           delay_ms(2);
        canSend(CAN1,&Can_Msg_TPDOTRANTYPE_NO[0][1]);
           delay_ms(2);
        canSend(CAN1,&Can_Msg_TPDOTRANTYPE_NO[1][0]);
           delay_ms(2);
        canSend(CAN1,&Can_Msg_TPDOTRANTYPE_NO[1][1]);
           delay_ms(2);

        /* 设置驱动器的SDO映射参数 */
       for(i=0;i<2;i++)
       {
            j=0;
           canSend(CAN1,&Can_Msg_Turn_Off_TPDOX[i][j]);
           delay_ms(2);
           j++;
           canSend(CAN1,&Can_Msg_Turn_Off_TPDOX[i][j]);
           delay_ms(2);
       }
       for(i=0;i<2;i++)
       {
            j=0;
           canSend(CAN1,&Can_Msg_Set_Number_Mapped_objectsTo0[i][j]);
           delay_ms(2);
           j++;
           canSend(CAN1,&Can_Msg_Set_Number_Mapped_objectsTo0[i][j]);
           delay_ms(2);
       }
       for(i=0;i<2;i++)
       {
            j=0;
           canSend(CAN1,&Can_Msg_Map_Actual_value_to_TPDOX_objectX[i][j]);
           delay_ms(2);
           j++;
           canSend(CAN1,&Can_Msg_Map_Actual_value_to_TPDOX_objectX[i][j]);
           delay_ms(2);
           j++;
           canSend(CAN1,&Can_Msg_Map_Actual_value_to_TPDOX_objectX[i][j]);
           delay_ms(2);
           j++;
           canSend(CAN1,&Can_Msg_Map_Actual_value_to_TPDOX_objectX[i][j]);
           delay_ms(2);
       }
       for(i=0;i<2;i++)
       {
            j=0;
           canSend(CAN1,&Can_Msg_Set_Number_Mapped_objectsTo2[i][j]);
           delay_ms(2);
           j++;
           canSend(CAN1,&Can_Msg_Set_Number_Mapped_objectsTo2[i][j]);
           delay_ms(2);
       }
       for(i=0;i<2;i++)
       {
            j=0;
           canSend(CAN1,&Can_Msg_Turn_On_TPDOX[i][j]);
           delay_ms(2);
           j++;
           canSend(CAN1,&Can_Msg_Turn_On_TPDOX[i][j]);
           delay_ms(2);
       }
    ​
    ​
       canSend(CAN1,&Can_Msg_OperationEn_NO[1]);
           delay_ms(2);    
       canSend(CAN1,&Can_Msg_VelocityMode_NO[0]);
           delay_ms(2);   
       canSend(CAN1,&Can_Msg_VelocityMode_NO[1]);
           delay_ms(2);    
    ​
        canSend(CAN1,&Can_Msg_OperationEn_NO[0]);
            delay_ms(2);
        canSend(CAN1,&Can_Msg_OperationEn_NO[1]);
            delay_ms(2);    
        canSend(CAN1,&Can_Msg_VelocityMode_NO[0]);
            delay_ms(2);   
        canSend(CAN1,&Can_Msg_VelocityMode_NO[1]);
            delay_ms(2);    
        canSend(CAN1,&Can_Msg_SetAcc_NO[0]);
            delay_ms(2);
        canSend(CAN1,&Can_Msg_SetAcc_NO[1]);
            delay_ms(2);
        canSend(CAN1,&Can_Msg_SetDec_NO[0]);
            delay_ms(2);
        canSend(CAN1,&Can_Msg_SetDec_NO[1]);
            delay_ms(2);

        canSend(CAN1,&Can_Msg_TargetVelocity_NO[0]);
            delay_ms(2);
        canSend(CAN1,&Can_Msg_TargetVelocity_NO[1]);
            delay_ms(2);        
        canSend(CAN1,&Can_Msg_MotionStart_NO[0]);  //Start/Stop Motion
            delay_ms(2);
        canSend(CAN1,&Can_Msg_MotionStart_NO[1]);  //Start/Stop Motion
            delay_ms(2);
    ​
    ​

        canSend(CAN1,&Can_Msg_ChangeMotionStatusToOperation[0]);  //切换至1号电机由预操作至运行状态
            delay_ms(2);
        canSend(CAN1,&Can_Msg_ChangeMotionStatusToOperation[1]);  //切换至2号电机预操作至运行状态
            delay_ms(2);
    }

![在这里插入图片描述](https://img-blog.csdnimg.cn/20190606141311532.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)

[为知笔记原文链接](http://d5da2ffa.wiz03.com/share/s/3lSy_W0xl4SM2WTmit29CtcC1LEspk1W7AgG22pWhX3wP9uD)
