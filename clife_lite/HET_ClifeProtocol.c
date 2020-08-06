/******************************************************************************\
**  版    权 :  深圳市和而泰智能控制股份有限公司所有（2020）
**  文件名称 :  HET_ClifeProtocol.c
**  功能描述 :  CP通讯相关驱动，包括正常通讯、历史数据交互以及在线升级
**  作    者 :  vincent
**  日    期 :  2020.07.01
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/2020.07.01
                1 首次创建  
				
\******************************************************************************/


/******************************************************************************\
                                 Includes
\******************************************************************************/
#include "HET_ClifeProtocol.h"
/******************************************************************************\
                             Variables definitions
\******************************************************************************/
//本地操作集合
typedef enum
{
    CMD_FLG_IDLE        = 0x00000000,		//空闲
    CMD_FLG_HEARTBEAT   = 0x00000001,       //心跳
    CMD_FLG_DEVICEINFO  = 0x00000002,		//查询产品信息
    CMD_FLG_GETDATE     = 0x00000004,		//获取时间信息
    CMD_FLG_BINDING     = 0x00000008,		//绑定
    CMD_FLG_TEST        = 0x00000010,		//产测
    CMD_FLG_TESTRESULT  = 0x00000020,		//产测结果返回
    CMD_FLG_DOWNLOAD    = 0x00000040,		//数据下发
    CMD_FLG_UPLOAD      = 0x00000080,		//数据上报
    CMD_FLG_DATASYNC    = 0x00000100,		//数据同步
    CMD_FLG_RESET       = 0x00000200,		//复位
    CMD_FLG_UNBIND      = 0x00000400,       //解除绑定
}TE_HET_LOCAL_CP_CMD;

//CP协议结构
typedef enum
{
    CP_INDEX_PACKHEAD,                                      //包头
    CP_INDEX_CMD,                                           //命令字高字节
    CP_INDEX_DATA_LEN_H,						            //数据长度高字节
    CP_INDEX_DATA_LEN_L,						            //数据长度低字节
    CP_INDEX_DATA,                                          //数据内容
}TE_HET_CPDATAINDEX;
//------------------------------------------------------------------------------
//CP串口接收状态
typedef enum
{
    HISTORY_SEND_IDLE,                                      //发送空闲
    HISTORY_SEND_BUSY,                                      //发送中
    HISTORY_SEND_SUCESS,                                    //发送成功
    HISTORY_SEND_FAIL,                                      //发送失败
}TE_CP_HISTORY_SEND_STATUS;

//------------------------------------------------------------------------------
static TS_HET_CP    *s_HetCP[HET_PLUGIN_CP_NUM] = { 0 };
static uint8_t      s_HetCPNum = 0;
//------------------------------------------------------------------------------
//设备信息
#pragma pack(1)
#if CP_MODE == MASTER_MODE
const TS_HET_CP_DEVICEINFO s_DeviceInfo =
{
    SOFT_VERSION,
    HARD_VERSION,
    { DEVICE_MODEL },
    { DEVICE_RESERVED }
};
#else
TS_HET_CP_DEVICEINFO s_DeviceInfo;
#endif

//系统时间
TS_HET_CP_TIME s_SysTime;

#pragma pack()

/******************************************************************************\
                             Functions definitions
\******************************************************************************/


/*
* 函数名称 : Het_CP_Drive_CheckSum
* 功能描述 : CP协议数据校验
* 参    数 : pBuf:数据指针
             Len:数据长度
             pChecksum:校验结果
* 返回值   : 空
* 示    例 : Het_CP_Drive_CheckSum(pBuf,Len,&pChecksum);
*/
/******************************************************************************/
static void Het_CP_Drive_CheckSum(uint8_t *pBuf,uint16_t Len,uint8_t *pChecksum)
/******************************************************************************/
{
	uint16_t i;
	uint8_t checksum = 0;
	for(i=0; i<Len; i++)
	{
		checksum += *pBuf++;
	}
	*pChecksum = (uint8_t)checksum;
}

/*
* 函数名称 : HET_CP_Drive_UsartRecvISR
* 功能描述 : CP数据接收整理
* 参    数 : pThis- 组件指针
             pBuf - 数据指针
             Len  - 数据长度
* 返回值   : 无
* 示    例 : 无
*/
/******************************************************************************/
void HET_CP_Drive_UsartRecvISR(TS_HET_CP *pThis, uint8_t *pBuf, uint16_t Len)
/******************************************************************************/
{
	static uint16_t data_len = 0;
    // 接收数据打包
	if(Len == 1)                                    //单字节接收
	{
        switch (pThis->RX.ReceiveFlow)
		{
			//判断数据起始帧
			case FLOW_UART_RECV_BEGIN:
				if(*pBuf == CP_PACK_HEAD)           //接收到帧头
				{
                    pThis->RX.ReceiveBuf[pThis->RX.ReceiveLen++] = *pBuf;
                    pThis->RX.ReceiveFlow = FLOW_UART_RECV_FINISH;
				}
				break;
			//判断串口接收是否完成 
			case FLOW_UART_RECV_FINISH:
                pThis->RX.ReceiveBuf[pThis->RX.ReceiveLen++] = *pBuf;
                if (pThis->RX.ReceiveLen == CP_INDEX_DATA)    //获取到数据长度
				{
                    data_len = (((uint16_t)(pThis->RX.ReceiveBuf[CP_INDEX_DATA_LEN_H])) << 8) + *pBuf;
				}
                if (pThis->RX.ReceiveLen > CP_INDEX_DATA)
				{
                    if (pThis->RX.ReceiveLen >= data_len + 1)
					{
                        pThis->RX.ReceiveFlow = FLOW_UART_RECV_BEGIN;
                        pThis->RX.ReceiveF = true;          //接收完成
                        pThis->CpError = false;               //CP通讯正常
					}
				}
				break;
				
			default:break;	
		}
	}	
	else//整帧接收                                                        
	{
		if(pBuf[CP_INDEX_PACKHEAD] == CP_PACK_HEAD)         //接收到帧头
		{
            memcpy(pThis->RX.ReceiveBuf, pBuf, Len);     //拷贝内存中的数据
            pThis->RX.ReceiveLen = Len;                //取做CRC校验的数据长度
            pThis->RX.ReceiveF = true;                 //接收完成
		}
	}
}

/*
* 函数名称 : Het_CP_Drive_UpdateSendTempBuf
* 功能描述 : 发送数据更新，即将要发送的数据更新到缓存
* 参    数 : pThis    - 组件指针
             DataId   - 数据ID
             pDataBuf - 数据指针
             DataLen  - 数据长度
* 返回值   : 无
* 示    例 : 无
*/
/******************************************************************************/
static uint8_t Het_CP_Drive_UpdateSendTempBuf(TS_HET_CP *pThis, uint8_t DataId, uint8_t *pDataBuf, uint8_t DataLen)
/******************************************************************************/
{
    uint8_t Cmd = 0;
    #if (CP_MODE == MASTER_MODE)
    Cmd = CMD_UPLOAD;
    #else
    Cmd = CMD_DOWNLOAD;
    #endif

    if(pDataBuf == 0 || DataLen == 0) return false;             //数据为空
    if(pThis->TX.UpdateIndex >= CP_SEND_TEMP_LEN) return false; //二级缓存已满
    
    pThis->TX.SendTempBuff[pThis->TX.UpdateIndex++] = DataId;               //更新数据ID
    pThis->TX.SendTempBuff[pThis->TX.UpdateIndex++] = DataLen;              //更新数据长度
    memcpy(&pThis->TX.SendTempBuff[pThis->TX.UpdateIndex],pDataBuf,DataLen);//更新数据内容
    pThis->TX.UpdateIndex += DataLen;
    pThis->CmdFlag |= (uint32_t)1<<(Cmd-1); //记录CMD，进行串口发送分时处理，防止串口资源冲突，如上报数据的时候触发了绑定
    return true;
}


/*
* 函数名称 : Het_CP_Drive_DataSync
* 功能描述 : 数据同步，即一次性发送全部数据，共用ID
* 参    数 : pThis    - 组件指针
             Cmd      - 功能码
             DataId   - 数据ID
             pDataBuf - 数据指针
             DataLen  - 数据长度
* 返回值   : 无
* 示    例 : 无
*/
/******************************************************************************/
static uint8_t Het_CP_Drive_DataSync(TS_HET_CP *pThis, TE_HET_CP_CMD Cmd, uint8_t DataId, uint8_t *pDataBuf, uint8_t DataLen)
/******************************************************************************/
{
    uint16_t send_len = 0;
	uint8_t checksum = 0;

    

    if (pThis->TX.SendDelay == HISTORY_SEND_IDLE)
    {
        memset(pThis->TX.SendBuff, 0, CP_SEND_LEN);                    //发送组数缓存清零
        //包头
        pThis->TX.SendBuff[send_len++] = CP_PACK_HEAD;
        //功能码
        pThis->TX.SendBuff[send_len++] = Cmd;
        //数据总长度
        pThis->TX.SendBuff[send_len++] = (uint8_t)((DataLen+2) >> 8);
        pThis->TX.SendBuff[send_len++] = (uint8_t)((DataLen+2));

        //数据ID
        pThis->TX.SendBuff[send_len++] = 0;
        //数据长度
        pThis->TX.SendBuff[send_len++] = DataLen;
        //数据内容
        memcpy(pThis->TX.SendBuff + send_len, pDataBuf, DataLen);
        send_len += DataLen;
        

        //计算 checksum
        Het_CP_Drive_CheckSum(pThis->TX.SendBuff + 1, send_len - 1, &checksum);
        pThis->TX.SendBuff[send_len++] = checksum;
        //发送数据
        pThis->TX.SendDelay = HET_CP_SEND_CYCLE;              //保证每帧发送数据之间有间隔,避免连续发送数据  

        if(Cmd == CMD_DATASYNC)
        {
            pThis->CmdFlag &= ~(uint32_t)1<<(Cmd-1);              //清除CMD标志
        }
        
        
        pThis->Func.UartSendCallback(pThis->TX.SendBuff, send_len); //调用串口底层发送函数
        
        return true; 
    }   
    else
    {
        return false;
    }
}

/*
* 函数名称 : Het_CP_Drive_DataSend
* 功能描述 : CP协议数据整理,并发送
* 参    数 : pThis    - 组件指针
             Cmd      - 功能码
             pDataBuf - 数据指针
             DataLen  - 数据长度
* 返回值   : 无
* 示    例 : 无
*/
/******************************************************************************/
static uint8_t Het_CP_Drive_DataSend(TS_HET_CP *pThis,TE_HET_CP_CMD Cmd, uint8_t *pBuf, uint16_t Len)
/******************************************************************************/
{
	uint16_t send_len = 0;
	uint8_t checksum = 0;
    
    if (pThis->TX.SendDelay == HISTORY_SEND_IDLE)
    {
        memset(pThis->TX.SendBuff, 0, CP_SEND_LEN);                    //发送组数缓存清零
        //包头
        pThis->TX.SendBuff[send_len++] = CP_PACK_HEAD;
        //功能码
        pThis->TX.SendBuff[send_len++] = Cmd;
        //数据总长度
        pThis->TX.SendBuff[send_len++] = (uint8_t)(Len >> 8);
        pThis->TX.SendBuff[send_len++] = (uint8_t)(Len);
         
        if (pBuf!=0 && Len>0)
        {
            memcpy(pThis->TX.SendBuff+send_len, pBuf, Len);
            if(Cmd == CMD_UPLOAD) 
            {
                memset(pThis->TX.SendTempBuff,0,CP_SEND_TEMP_LEN);//数据上报命名，拷贝完二级缓存后清零
                pThis->TX.UpdateIndex = 0;
            }
            
            send_len += Len;
        }

        //计算 checksum
        Het_CP_Drive_CheckSum(pThis->TX.SendBuff + 1, send_len - 1, &checksum);
        pThis->TX.SendBuff[send_len++] = checksum;
        //发送数据
        pThis->TX.SendDelay = HET_CP_SEND_CYCLE;              //保证每帧发送数据之间有间隔,避免连续发送数据  
        pThis->CmdFlag &= ~(uint32_t)1<<(Cmd-1);              //清除CMD标志
        
        pThis->Func.UartSendCallback(pThis->TX.SendBuff, send_len); //调用串口底层发送函数
        
        return true; 
    } 
    else
    {
        return false;
    }    
	 
}



/*
* 函数名称 : Het_CP_Drive_Process
* 功能描述 : CP通讯流程控制状态机
* 参    数 : pThis    - 组件指针
* 返回值   : 空
* 示    例 : Het_CP_Drive_Process();
*/
/******************************************************************************/
static void Het_CP_Drive_Process(TS_HET_CP *pThis)
/******************************************************************************/
{
    uint8_t checksum;
    uint8_t cmd;
//    uint8_t *pbuf;
    uint16_t len;
    
    if(pThis->RX.ReceiveF == true)
    {
        pThis->RX.ReceiveF = false;

        
        //进行除去包头的数据校验
        Het_CP_Drive_CheckSum(pThis->RX.ReceiveBuf + 1, pThis->RX.ReceiveLen - 2, &checksum);

        if (checksum == pThis->RX.ReceiveBuf[pThis->RX.ReceiveLen - 1])
        {
            //获取功能码ID
            cmd = pThis->RX.ReceiveBuf[CP_INDEX_CMD];
            //获取数据长度
            len = ((uint16_t)pThis->RX.ReceiveBuf[CP_INDEX_DATA_LEN_H] << 8) 
                | pThis->RX.ReceiveBuf[CP_INDEX_DATA_LEN_L];
            //检查长度
            if (pThis->RX.ReceiveLen != (len + 5)) return; 

            switch(cmd)
            {
                //default之前的case因为无需带数据内容，所以做成自动应答方式
                //心跳
                case CMD_HEARTBEAT:
                    
                    #if CP_MODE == MASTER_MODE
                    //MCU收到模组心跳
                    pThis->CmdFlag |= CMD_FLG_HEARTBEAT;
                    pThis->CpStatus = pThis->RX.ReceiveBuf[CP_INDEX_DATA];//获取CP的连接状态
                    #else
                    //模组收到MCU应答
                    DEBUG("Received heartbeat ACK!\n");
                    #endif
                    
                break;
                
                //查询产品信息
                case CMD_DEVICEINFO:
                    
                    #if CP_MODE == MASTER_MODE
                    //MCU收到模组查询产品信息指令
                    pThis->CmdFlag |= CMD_FLG_DEVICEINFO;
                    #else
                     //模组收到MCU发来的设备信息
                    memcpy((uint8_t*)&s_DeviceInfo, &pThis->RX.ReceiveBuf[CP_INDEX_DATA], sizeof(TS_HET_CP_DEVICEINFO));
                    pThis->Func.DevInfoRxCallback(&s_DeviceInfo);
                    #endif
                    
                    break;  
                    
                //同步时间
                case CMD_GETDATE:
                    
                    #if CP_MODE == MASTER_MODE
                   //MCU收到模组应答的时间信息
                    pThis->Func.SyncTimeRxCallback(&pThis->RX.ReceiveBuf[CP_INDEX_DATA],len);
                    #else
                    //模组收到MCU发来的获取时间信息指令
                    pThis->CmdFlag |= CMD_FLG_GETDATE;
                    #endif

                break;
                
                //绑定
                case CMD_BINDING:
                     
                    #if CP_MODE == MASTER_MODE
                    
                    #else
                    //模组收到MCU发来的绑定指令
                    //DoBindProcess()
                    pThis->CmdFlag |= CMD_FLG_BINDING;  
                    #endif
                    
                    break;
                
                //产测
                case CMD_TEST:
                    
                    #if CP_MODE == MASTER_MODE
                    
                    #else
                    //模组收到MCU发来的产测指令
                    //DoFactoryTestProcess()
                    pThis->CmdFlag |= CMD_FLG_TEST;  
                    #endif
                    
                    break;
                
                //产测结果返回
                case CMD_TESTRESULT:
                    
                    #if CP_MODE == MASTER_MODE
                    //获取产测结果
                    pThis->CpFactoryTestStatus = pThis->RX.ReceiveBuf[CP_INDEX_DATA];
                    pThis->CmdFlag |= CMD_FLG_TESTRESULT;
                    #else

                    
                    #endif
                    
                    break;

                //数据下发
                case CMD_DOWNLOAD:
                    
                    #if CP_MODE == MASTER_MODE
                    //MCU收到模组下发数据处理
                    pThis->Func.DownloadRxCallback(&pThis->RX.ReceiveBuf[CP_INDEX_DATA], len);
                    pThis->CmdFlag |= CMD_FLG_DOWNLOAD;
                    #else

                    #endif

                    break;
                
                //数据上报
                case CMD_UPLOAD:
                    
                    #if CP_MODE == MASTER_MODE
                    
                    #else
                    //模组收到MCU数据上报指令
                    pThis->Func.UploadRxCallback(&pThis->RX.ReceiveBuf[CP_INDEX_DATA], len);
                    pThis->CmdFlag |= CMD_FLG_UPLOAD;
                    #endif
                    
                    break;
                
                //数据同步
                case CMD_DATASYNC:
                    
                    #if CP_MODE == MASTER_MODE
                    //MCU收到模组发来的数据同步指令
                    pThis->CmdFlag |= CMD_FLG_DATASYNC;
                    #else
                    //模组收到MCU发来的数据同步内容
                    pThis->Func.SyncDataRxCallback(&pThis->RX.ReceiveBuf[CP_INDEX_DATA], len);
                    #endif
                    
                    break;
                
                //复位
                case CMD_RESET:
                    
                    #if CP_MODE == MASTER_MODE
                    
                    #else
                    //模组收到MCU发来的复位模组指令
                    pThis->CmdFlag |= CMD_FLG_RESET;
                    #endif
                    
                    break;
                
                default:
                    
                break;
            }    
        }    
        memset(pThis->RX.ReceiveBuf,0,CP_RECV_LEN);
    }
    else
    {
        //数据上报
        if ((pThis->CmdFlag & CMD_FLG_UPLOAD) == CMD_FLG_UPLOAD)
        {
            #if CP_MODE == MASTER_MODE
            Het_CP_Drive_DataSend(pThis, CMD_UPLOAD, pThis->TX.SendTempBuff, pThis->TX.UpdateIndex);        
            #else
            
            //模组应答MCU数据上报
            Het_CP_Drive_DataSend(pThis,CMD_UPLOAD,0,0);
            #endif
            
        }
        //心跳应答
        else if ((pThis->CmdFlag & CMD_FLG_HEARTBEAT) == CMD_FLG_HEARTBEAT)
        {
            
            #if CP_MODE == MASTER_MODE
            //mcu应答
            Het_CP_Drive_DataSend(pThis,CMD_HEARTBEAT,0,0);
            #else

            #endif
        }
        //应答产品信息
        else if ((pThis->CmdFlag & CMD_FLG_DEVICEINFO) == CMD_FLG_DEVICEINFO)
        {
            
            #if CP_MODE == MASTER_MODE
            //mcu应答
            Het_CP_Drive_DataSend(pThis,CMD_DEVICEINFO, (uint8_t*)&s_DeviceInfo, sizeof(s_DeviceInfo));
             #else

            #endif
        }
        //获取时间信息
        else if ((pThis->CmdFlag & CMD_FLG_GETDATE) == CMD_FLG_GETDATE)
        {
            
            #if CP_MODE == MASTER_MODE
            
            
            #else
            //WiFi模组应答MCU的获取时间信息指令
            if(pThis->TX.SendDelay == HISTORY_SEND_IDLE)
            {
                pThis->Func.GetSysTimeInfo(&s_SysTime);
                Het_CP_Drive_DataSend(pThis, CMD_GETDATE, (uint8_t*)&s_SysTime, sizeof(TS_HET_CP_TIME));
            }
            
            
            #endif
        }
        //绑定
        else if ((pThis->CmdFlag & CMD_FLG_BINDING) == CMD_FLG_BINDING)
        {
            #if CP_MODE == MASTER_MODE
                
            #else
            //模组应答MCU的绑定指令
            Het_CP_Drive_DataSend(pThis, CMD_BINDING, 0, 0);
            
            #endif
            
        }
        //产测
        else if ((pThis->CmdFlag & CMD_FLG_TEST) == CMD_FLG_TEST)
        {
            #if CP_MODE == MASTER_MODE

            #else
            //模组应答MCU的产测指令
            if(pThis->TX.SendDelay == HISTORY_SEND_IDLE)
            {
                Het_CP_Drive_DataSend(pThis, CMD_TEST, 0, 0);
                pThis->Func.FactoryTest();//执行产测功能
            }
            
            #endif
            
        }
        //应答测试结果
        else if ((pThis->CmdFlag & CMD_FLG_TESTRESULT) == CMD_FLG_TESTRESULT)
        {
            #if CP_MODE == MASTER_MODE
            //MCU应答模组的产测结果
            Het_CP_Drive_DataSend(pThis, CMD_TESTRESULT, 0, 0);      
            #else

            #endif
        }
        //应答数据下发
        else if ((pThis->CmdFlag & CMD_FLG_DOWNLOAD) == CMD_FLG_DOWNLOAD)
        {
            
            #if CP_MODE == MASTER_MODE
            //MCU应答模组数据下发指令
            Het_CP_Drive_DataSend(pThis, CMD_DOWNLOAD, 0, 0);     
            #else
            Het_CP_Drive_DataSend(pThis, CMD_UPLOAD, pThis->TX.SendTempBuff, pThis->TX.UpdateIndex);        
            #endif
        }
        
        //数据同步
        else if ((pThis->CmdFlag & CMD_FLG_DATASYNC) == CMD_FLG_DATASYNC)
        {
            
            #if CP_MODE == MASTER_MODE
            //MCU应答模组数据同步指令
            pThis->Func.SyncDataTxCallback();        
             #else
            
            #endif
        }
        //复位
        else if ((pThis->CmdFlag & CMD_FLG_RESET) == CMD_FLG_RESET)
        {
            #if CP_MODE == MASTER_MODE
                    
            #else
            if(pThis->TX.SendDelay == HISTORY_SEND_IDLE)
            {
                Het_CP_Drive_DataSend(pThis, CMD_RESET, 0, 0);
                if(pThis->Func.ResetProcess)
                {
                    pThis->Func.ResetProcess();
                }
            }
            
            #endif
        }
    }        
}



/*
* 函数名称 : HET_CP_Task
* 功能描述 : CP任务，每10ms运行一次
* 参    数 : 无
* 返回值   : 无
* 示    例 : HET_CP_Task();
*/
/******************************************************************************/
void HET_CP_Task(TS_HET_CP *pThis)
/******************************************************************************/
{
    
    // 控制发送间隔
    if(pThis->TX.SendDelay)
    {
        pThis->TX.SendDelay--;          //串口连续发送间隔控制,避免连续发送数据
    }

    // 运行通讯协议状态机
    Het_CP_Drive_Process(pThis);
    
}

/*
* 函数名称 : HET_CP_SendDataFrame
* 功能描述 : 用户数据发送函数
* 参    数 : pThis    - 组件指针
             Cmd      - 功能码
             DataId   - 数据ID
             pDataBuf - 数据指针
             DataLen  - 数据长度
* 返回值   : 无
* 示    例 : 无
*/
/******************************************************************************/
uint8_t HET_CP_SendDataFrame(TS_HET_CP *pThis, TE_HET_CP_CMD Cmd, uint8_t DataId, uint8_t* pDataBuf, uint8_t DataLen)
/******************************************************************************/
{
    uint8_t res = 0;

    if(Cmd == CMD_DOWNLOAD || Cmd == CMD_UPLOAD || Cmd == CMD_DATASYNC) //数据上报
    {
        if(DataId == 0) //全部数据模式,直接更新到一级缓存并立即触发一次串口发送
        {
            res = Het_CP_Drive_DataSync(pThis, Cmd, DataId, pDataBuf, DataLen);
        }
        else //单数据模式，更新数据到二级缓存，置标记位，非立即发送
        {
            res = Het_CP_Drive_UpdateSendTempBuf(pThis, DataId, pDataBuf, DataLen);
        }
    }
    else
    {
        res = Het_CP_Drive_DataSend(pThis, Cmd, pDataBuf, DataLen);
    }
    return res;
}

/*
* 函数名称 : HET_CP_GetConnectStatus
* 功能描述 : 获取连接状态字节
* 参    数 : pThis    - 组件指针
* 返回值   : Status - 连接状态字节
* 示    例 : HET_CP_GetConnectStatus(&Status);
*/
/******************************************************************************/
uint8_t HET_CP_GetConnectStatus(TS_HET_CP *pThis)
/******************************************************************************/
{
    return pThis->CpStatus;
}

/*
* 函数名称 : HET_CP_GetNetStatus
* 功能描述 : 获取网络连接状态
* 参    数 : 无
* 返回值   : TE_HET_NET_STATUS - HET_ONLINE:在线
                                 HET_OFFLINE:离线
* 示    例 : InterStatus = HET_CP_GetNetStatus();
*/
/******************************************************************************/
TE_HET_NET_STATUS HET_CP_GetNetStatus(TS_HET_CP *pThis)
/******************************************************************************/
{     
    if ((pThis->CpStatus & 0x70) == 0x70)      //判断是否在线
    {
        return HET_ONLINE;         //在线    
    }
    else
    {
        return HET_OFFLINE;        //离线
    }    
}
/*
* 函数名称 : HET_CP_GetSignalStrength
* 功能描述 : 获取Wifi信号强度
* 参    数 : 无
* 返回值   : WiFi信号强度0~10
* 示    例 : CPSignalStrength = HET_CP_GetSignalStrength();
*/
/******************************************************************************/
uint8_t HET_CP_GetSignalStrength(TS_HET_CP *pThis)
/******************************************************************************/
{   
    return (pThis->CpStatus & 0x0f);
}
/*
* 函数名称 : HET_CP_GetData
* 功能描述 : 获取数据发送状态
* 参    数 : 无
* 返回值   : HET_SEND_IDLE - 空闲
             HET_SEND_BUSY - 发送周期中
* 示    例 : Status = HET_CP_SendStatus();
*/
/******************************************************************************/
TE_HET_SENDSTATUS HET_CP_SendStatus(TS_HET_CP *pThis)
/******************************************************************************/
{
    //判断是否处于发送周期中
    if (pThis->TX.SendDelay)
    {
        return HET_SEND_BUSY;
    }    
    else
    {
        return HET_SEND_IDLE;
    }    
}
/*
* 函数名称 : HET_CP_GetFactoryTestStatus
* 功能描述 : 获取产测状态
* 参    数 : 空
* 返回值   : FACTORY_IDLE(未产测)
             FACTORY_TESTING(产测中)
             FACTORY_FAIL(产测失败)
             FACTORY_PASS(产测成功)
* 示    例 : Status = HET_CP_GetFactoryTestStatus();
*/
/******************************************************************************/
uint8_t HET_CP_GetFactoryTestStatus(TS_HET_CP* pThis)
/******************************************************************************/
{   
    return pThis->CpFactoryTestStatus;    
}

/*
* 函数名称 : HET_CP_Open
* 功能描述 : 使能组件
* 参    数 : pThis - 组件指针
* 返回值   : TE_HET_STA：错误代码
*/
/******************************************************************************/
uint8_t HET_CP_Open(TS_HET_CP* pThis)
/******************************************************************************/
{
    uint8_t i;

    // 判断是否还有空指针使用
    for (i = 0; i < HET_PLUGIN_CP_NUM; i++)
    {
        if (0 == s_HetCP[i])
        {
            break;
        }
    }

    // 超出组件最大个数
    if (i >= HET_PLUGIN_CP_NUM)
    {
        //HET_ASSERT_PERROR(HET_ERROR_MAX);
        return 1;
    }

    /* 参数检查 */
    if ((pThis == 0)
        || (pThis->Func.UartSendCallback == 0)
#if CP_MODE == MASTER_MODE
        || (pThis->Func.DownloadRxCallback == 0)
        || (pThis->Func.SyncTimeRxCallback == 0)
        || (pThis->Func.SyncDataTxCallback == 0)
#else
        || (pThis->Func.UploadRxCallback == 0)
        || (pThis->Func.DevInfoRxCallback == 0)
        || (pThis->Func.SyncDataRxCallback == 0)
        || (pThis->Func.GetSysTimeInfo == 0)
#endif
        )
    {
        while (1);
    }


    // 指针赋值
    s_HetCP[i] = pThis;

    
    // 创建任务
    //Thread_Create(HET_CP_Task);

    s_HetCPNum++;

    //SYS_LOG_INFO("ID %d: Init OK!", i);

    return 0;
}

/******************************* End of File (C) ******************************/

