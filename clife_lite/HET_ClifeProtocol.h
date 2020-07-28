/******************************************************************************\
**  版    权 :  深圳市和而泰智能控制股份有限公司所有（2020）
**  文件名称 :  HET_ClifeProtocol.h
**  功能描述 :  CP通讯相关驱动，包括正常通讯，以及在线升级
**  作    者 :  vincent
**  日    期 :  2020.07.01
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/2020.07.01
                1 首次创建                 
\******************************************************************************/

#ifndef _HET_CLIFEPROTOCOL_H_
#define _HET_CLIFEPROTOCOL_H_

/******************************************************************************\
                             Includes
\******************************************************************************/	
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
/******************************************************************************\
                          Macro definitions
\******************************************************************************/
#define CP_PACK_HEAD                0xA5                    //协议包头

#define CP_RECV_LEN				    250                     //接收缓存数组长度
#define CP_SEND_LEN				    512                     //发送缓存数组长度
#define CP_SEND_TEMP_LEN			512                     //发送缓存数组长度


#define BIT_CP_BIND				    0x10					//WiFi状态字绑定标志位
#define BIT_CP_ROUTER				0x20                    //WiFi状态字路由器连接标志位
#define BIT_CP_SERVICE				0x40					//WiFi状态字服务器连接标志位
#define BIT_CP_ALL					0x70					//WiFi状态字全部连接标志位

#define CP_TEST_TIMEOUT			    3                       //产测命令发送超时次数
#define CP_TEST_MAX_TIMES			60						//产测超时时间(1分钟)
#define COMMAND_ERROR_RESET			6						//通讯异常计数

#define HET_CP_DELAY_MS(x)			(x)                     //毫秒延迟
#define HET_CP_DELAY_S(x)			((uint32_t)100*(x))     //秒延迟

#define	HET_CP_SEND_IDLE			0                       //发送空闲
#define	HET_CP_SEND_CYCLE			10                     //发送间隔,间隔时间为：HET_CP_SEND_CYCLE*10ms

//设备信息
#define DEVICE_INFO_LEN             32                          //设备信息字节长度
#define SOFT_VERSION                0x01                        //软件版本
#define HARD_VERSION                0x01                        //驱动板硬件版本
#define DEVICE_MODEL                "NULL"                      //整机型号
#define DEVICE_RESERVED             "NULL"                      //预留

#define CP_MASTER_MODE                 0 //主机通常指主控MCU
#define CP_SLAVE_MODE                  1 //从机通常指WiFi模组、GUI板等
#define CP_MODE                     CP_SLAVE_MODE//MASTER_MODE//

#ifndef HET_PLUGIN_CP_NUM
#define HET_PLUGIN_CP_NUM 1
#endif 

#define DBG_LOG_EN 1

#if DBG_LOG_EN
#define DEBUG(format, ...) printf (format, ##__VA_ARGS__)
#else
#define DEBUG
#endif


/******************************************************************************\
                         Typedef definitions
\******************************************************************************/
#pragma pack(1)
//------------------------------------------------------------------------------

//CMD集合
typedef enum
{
    CMD_HEARTBEAT  = 0x01,          	//心跳
    CMD_DEVICEINFO = 0x02,          	//查询产品信息
    CMD_GETDATE    = 0x03,          	//获取时间信息
    CMD_BINDING    = 0x04,          	//绑定
    CMD_TEST       = 0x05,          	//产测
    CMD_TESTRESULT = 0x06,             //产测结果返回
    CMD_DOWNLOAD   = 0x07,             //数据下发
    CMD_UPLOAD     = 0x08,             //数据上报
    CMD_DATASYNC   = 0x09,             //数据同步
    CMD_RESET      = 0x0A,             //复位
    CMD_UNBIND     = 0x0B,             //解除绑定

}TE_HET_CP_CMD;

//CP网络状态
typedef enum
{
    HET_ONLINE,                                     //在线
    HET_OFFLINE,                                    //离线
}TE_HET_NET_STATUS;

//产测结果
typedef enum
{
    FACTORY_IDLE,                                   //未产测
    FACTORY_TESTING,                                //产测中
    FACTORY_FAIL,                                   //产测失败
    FACTORY_PASS,                                   //产测成功
}TE_HET_FACTORYTEST_RESULT;

//串口数据发送状态
typedef enum
{
    HET_SEND_IDLE,                                  //空闲状态
    HET_SEND_BUSY,                                  //发送周期中
}TE_HET_SENDSTATUS;
//------------------------------------------------------------------------------
typedef struct
{
    uint8_t 				    Id;				//数据ID
    uint8_t 				    Len;			//数据长度
    uint8_t* 				    pData;           //数据指针
}TS_HET_DATA_UNIT;

//------------------------------------------------------------------------------

//设备信息
typedef struct
{
    uint8_t 				    SoftwareVer;				//软件版本信息
    uint8_t 				    HardwareVer;				//硬件版本信息
    uint8_t 				    DeviceProductModel[15];     //产品型号
    uint8_t 				    Reserve2[15];               //保留字
}TS_HET_CP_DEVICEINFO;

//系统时间
typedef struct
{
    uint8_t 				    Year;				//年
    uint8_t 				    Month;				//月
    uint8_t 				    Date;               //日
    uint8_t 				    Hours;              //时
    uint8_t 				    Minutes;            //分
    uint8_t 				    Seconds;            //秒
    uint8_t 				    Weekday;            //星期
}TS_HET_CP_TIME;

//------------------------------------------------------------------------------
//CP串口接收状态
typedef enum
{
    FLOW_UART_RECV_BEGIN,                                   //接收开始
    FLOW_UART_RECV_FINISH,                                  //接收完成
}TE_CP_RECV_FLOW;
    
//发送数据结构体
typedef struct
{
    uint8_t					    ReceiveBuf[CP_RECV_LEN];	//MCU接收WIFI数据的缓存	
    uint8_t					    ReceiveF;                   //接收标志位
    uint16_t             	    ReceiveLen;                 //接收WIFI数据的长度
    TE_CP_RECV_FLOW		        ReceiveFlow;                //接收WIFI数据的流程枚举
    
}TS_CPCOMM_RECV;

//接收数据结构体
typedef struct
{
    uint8_t					    SendTempBuff[CP_SEND_TEMP_LEN];//发送临时缓存
    uint8_t					    SendBuff[CP_SEND_LEN];      //发送缓存
    uint16_t                    UpdateIndex;                //发送更新索引，发送完成后清零
    uint16_t                    SendDelay;                  //发送间隔
}TS_CPCOMM_SEND;
//------------------------------------------------------------------------------
//函数指针结构体
typedef struct
{
    void (*UartSendCallback) (uint8_t *pBuf, uint16_t Len);           // 串口发送回调

    #if CP_MODE == MASTER_MODE
    void (*DownloadRxCallback)(uint8_t *pBuf, uint16_t Len);            // MCU接收数据下发处理回调
    void (*SyncTimeRxCallback)(uint8_t *pBuf, uint16_t Len);            // MCU接收网络时间处理回调
    void (*SyncDataTxCallback)(void);                                   // MCU发送数据同步处理回调
    #else
    void (*UploadRxCallback)(uint8_t *pBuf, uint16_t Len);              // 模组收到数据上传处理回调
    void (*DevInfoRxCallback)(TS_HET_CP_DEVICEINFO* pThis);             // 模组收到设备信息处理回调
    void (*SyncDataRxCallback)(uint8_t *pBuf, uint16_t Len);            // 模组收到数据同步处理回调
    void (*GetSysTimeInfo)(TS_HET_CP_TIME* pTime);                      // 模组获取系统时间回调
    void (*FactoryTest)(void);                                          // 模组产测回调
    void (*ResetProcess)(void);                                         // 模组复位处理
    
    #endif
}TS_CPCOMM_FUNC;
//------------------------------------------------------------------------------
//组件结构体
typedef struct
{
    TS_CPCOMM_RECV              RX;
    TS_CPCOMM_SEND              TX;
    TS_CPCOMM_FUNC              Func;                       // 函数指针

    uint8_t 				    CpStatus;					//网络状态
    uint8_t                     CpError;                    //通讯异常
    uint8_t                     CpFactoryTestStatus;        //产测状态
    uint32_t                    LocalDataType;              //本地上报数据类型
    uint32_t                    CmdFlag;

}TS_HET_CP;

#pragma pack()
/******************************************************************************\
                    Global variables and functions
\******************************************************************************/
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
extern void HET_CP_Drive_UsartRecvISR(TS_HET_CP *pThis, uint8_t *pBuf, uint16_t Len);
/******************************************************************************/


/*
* 函数名称 : HET_CP_Task
* 功能描述 : CP任务，每10ms运行一次
* 参    数 : 无
* 返回值   : 无
* 示    例 : HET_CP_Task();
*/
/******************************************************************************/
extern void HET_CP_Task(TS_HET_CP *pThis);
/******************************************************************************/


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
extern uint8_t HET_CP_SendDataFrame(TS_HET_CP *pThis, TE_HET_CP_CMD Cmd, uint8_t DataId, uint8_t* pDataBuf, uint8_t DataLen);
/******************************************************************************/


/*
* 函数名称 : HET_CP_GetConnectStatus
* 功能描述 : 获取连接状态字节
* 参    数 : pThis    - 组件指针
* 返回值   : Status - 连接状态字节
* 示    例 : HET_CP_GetConnectStatus(&Status);
*/
/******************************************************************************/
extern uint8_t HET_CP_GetConnectStatus(TS_HET_CP *pThis);
/******************************************************************************/


/*
* 函数名称 : HET_CP_GetNetStatus
* 功能描述 : 获取网络连接状态
* 参    数 : 无
* 返回值   : TE_HET_NET_STATUS - HET_ONLINE:在线
                                 HET_OFFLINE:离线
* 示    例 : InterStatus = HET_CP_GetNetStatus();
*/
/******************************************************************************/
extern TE_HET_NET_STATUS HET_CP_GetNetStatus(TS_HET_CP *pThis);
/******************************************************************************/

/*
* 函数名称 : HET_CP_GetSignalStrength
* 功能描述 : 获取Wifi信号强度
* 参    数 : 无
* 返回值   : WiFi信号强度0~10
* 示    例 : CPSignalStrength = HET_CP_GetSignalStrength();
*/
/******************************************************************************/
extern uint8_t HET_CP_GetSignalStrength(TS_HET_CP *pThis);
/******************************************************************************/

/*
* 函数名称 : HET_CP_GetData
* 功能描述 : 获取数据发送状态
* 参    数 : 无
* 返回值   : HET_SEND_IDLE - 空闲
             HET_SEND_BUSY - 发送周期中
* 示    例 : Status = HET_CP_SendStatus();
*/
/******************************************************************************/
extern TE_HET_SENDSTATUS HET_CP_SendStatus(TS_HET_CP *pThis);
/******************************************************************************/

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
extern uint8_t HET_CP_GetFactoryTestStatus(TS_HET_CP* pThis);
/******************************************************************************/


/*
* 函数名称 : HET_CP_Open
* 功能描述 : 使能组件
* 参    数 : pThis - 组件指针
* 返回值   : TE_HET_STA：错误代码
*/
/******************************************************************************/
extern uint8_t HET_CP_Open(TS_HET_CP* pThis);
/******************************************************************************/


#endif
/******************************* End of File (H) ******************************/
