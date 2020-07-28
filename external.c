/******************************************************************************\
**  版    权 :  深圳市和而泰智能控制股份有限公司所有（2020）
**  文件名称 :  external.c
**  功能描述 :  基于C-LIFE简化版协议的串口收发处理任务
**  作    者 :  vincent
**  日    期 :  2020.06.29
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/2020.06.29
                1 首次创建   
\******************************************************************************/
// Example for UART frame reference
// header     funcode    total_length    data0~N      checksum
//  0XA5       0X07         0X0004      0X01020304       X

// checksum = funcode + total_length + data0~N
/******************************************************************************\
                                 Includes
\******************************************************************************/
#include <sys/ioctl.h>
#include <assert.h>
#include <mqueue.h>
#include <string.h>
#include <unistd.h>
#include "ite/itp.h"
#include "ctrlboard.h"
#include "scene.h"

#include "clife_lite/GUI_ClifeProtocol.h"
/******************************************************************************\
                             Variables definitions
\******************************************************************************/
#define CLIFE_LITE_EN               1
//------------------------------------------------------------------------------
#define EXT_MAX_QUEUE_SIZE          8
#define MAX_OUTDATA_SIZE            250 //CP_RECV_LEN

#define UartHeader     		        0xA5


#ifdef _WIN32
#define WIN32_COM                   MY_WIN32_COM
#endif
//------------------------------------------------------------------------------
typedef enum GET_UART_COMMAND_STATE_TAG
{
    GET_HEADER,
    GET_FUNCODE,
    GET_LENGTH,
    GET_PAYLOAD,
    GET_CHECKSUM,
} GET_UART_COMMAND_STATE;
//------------------------------------------------------------------------------
static mqd_t extInQueue = -1; //用户任务往UI发送消息，比如串口任务接收到数据，根据数据发送对应的消息到UI，对UI进行更新或控制
static mqd_t extOutQueue = -1;//UI往用户任务发送消息，比如按了一个按键，发一条消息，在用户任务中收到消息后，发出对应的串口数据
static pthread_t extTask;
static volatile bool extQuit;
static unsigned char inDataBuf[EXTERNAL_BUFFER_SIZE];
static unsigned char cmdBuf[EXTERNAL_BUFFER_SIZE];
static unsigned int cmdPos = 0;
static GET_UART_COMMAND_STATE gState = GET_HEADER;
static unsigned int payloadCount = 0;
static unsigned char checkSum = 0;

static unsigned int lengthCount = 0;
static unsigned int  frameLength= 0;

//------------------------------------------------------------------------------
//用户任务
static pthread_t extClifeTask;
static volatile bool extClifeQuit;
//------------------------------------------------------------------------------
int ExternalInQueueReceive(ExternalEvent* ev);
int ExternalOutQueueReceive(ExternalEvent* ev);
int ExternalInQueueSend(ExternalEvent* ev); 
int ExternalInOutQueueSend(ExternalEvent* ev);
//------------------------------------------------------------------------------
extern void ExternalOutQueueProcessEvent(ExternalEvent* ev);
/******************************************************************************\
                             Functions definitions
\******************************************************************************/ 
/*
* 函数名称 : ExternalTask
* 功能描述 : 外部任务，串口接收、队列接收等
* 参    数 : arg -- 任务参数
* 返回值   : 无
* 示    例 : 无
*/
/******************************************************************************/
static void* ExternalTask(void* arg)
/******************************************************************************/
{
    while (!extQuit)
    {
       	ExternalEvent ev;

		unsigned char readLen = 0;
//		unsigned char paramBuf[EXTERNAL_BUFFER_SIZE];
//		unsigned char outDataBuf[MAX_OUTDATA_SIZE];
		unsigned int  i = 0, count = 0;
		bool isToMc = false;
		bool isCmdCompleted = false;

#if defined(CFG_UART2_ENABLE) && !defined(CFG_DBG_UART2)
		// Receive UI command
		if(ExternalOutQueueReceive(&ev))
		{
            ExternalOutQueueProcessEvent(&ev);
        }
		/*
		if (mq_receive(extOutQueue, (char*)&evIn, sizeof(ExternalEvent), 0) > 0)
		{
			isToMc = true;
			memset(outDataBuf, 0, MAX_OUTDATA_SIZE);

			outDataBuf[0] = 0x00;
			outDataBuf[1] = 0x01;
		}

		// Check if need to send data to main controller
		if (isToMc)
		{
#ifdef _WIN32
            UartSend(WIN32_COM, outDataBuf, MAX_OUTDATA_SIZE);
#else
			write(ITP_DEVICE_UART2, outDataBuf, MAX_OUTDATA_SIZE);
#endif
		}

		memset(inDataBuf, 0, EXTERNAL_BUFFER_SIZE);
		
		*/
		// Read data from UART port
#ifdef _WIN32
        readLen = UartReceive(WIN32_COM, inDataBuf, EXTERNAL_BUFFER_SIZE);
#else
		readLen = read(ITP_DEVICE_UART2, inDataBuf, EXTERNAL_BUFFER_SIZE); //该函数带阻塞，读到串口数据才往下执行
#endif

		// Example for UART error correction to avoid data shift
		if (readLen)
		{
            #if 1
            printf("\r\n\r\nReceived: ");
            for (int i = 0; i < readLen; i++)
            {
                printf("%02X ", inDataBuf[i]);
            }
            //printf("\r\n");
            #endif
            
			while (readLen--)
			{
				switch (gState)
				{
					case GET_HEADER:
						if (UartHeader == inDataBuf[count])
						{
							cmdBuf[cmdPos++] = inDataBuf[count];
                            gState = GET_FUNCODE;
						}
						else
						{
							// printf("[GET_HEADER] Wrong, getstr=0x%x, len=%d\n", getstr[count], len);
							cmdPos = 0;
							memset(cmdBuf, 0, EXTERNAL_BUFFER_SIZE);
							gState = GET_HEADER;
						}
						break;
                        
                    case GET_FUNCODE:
						cmdBuf[cmdPos++] = inDataBuf[count];
						gState = GET_LENGTH;
						break;

					case GET_LENGTH:
                        if (lengthCount != (2 - 1)) //数据长度为2字节
						{
							gState = GET_LENGTH;
							lengthCount++;
						}
						else
						{
							gState = GET_PAYLOAD;
							lengthCount = 0;
						}
                        cmdBuf[cmdPos++] = inDataBuf[count];
                        if(gState == GET_PAYLOAD)
                        {
                            frameLength = ((uint16_t)cmdBuf[cmdPos-2] << 8) + cmdBuf[cmdPos-1]; //记录数据帧长度
                            if (frameLength == 0)
                            {
                                gState = GET_CHECKSUM;
                            }
                        }
						break;

					case GET_PAYLOAD:
						if (payloadCount != (frameLength - 1))
						{
							gState = GET_PAYLOAD;
							payloadCount++;
						}
						else
						{
							gState = GET_CHECKSUM;
							payloadCount = 0;
						}
						cmdBuf[cmdPos++] = inDataBuf[count];
						break;

					case GET_CHECKSUM:
					    // checkSum is sum of all payloads
						for (i=0; i<(frameLength+3); i++)
							checkSum += cmdBuf[1+i];

						if (checkSum == inDataBuf[count])
						{
							// Get one command
							cmdBuf[cmdPos++] = inDataBuf[count];
							isCmdCompleted = true;
						}
						else
						{
							printf("[GET_CHECKSUM] Wrong, checkSum=0x%x\n", checkSum);
						}
						checkSum = 0;
						cmdPos = 0;
						gState = GET_HEADER;
						break;
				}
				count++;
			}
		}

		// If read data is completed, start to parse data
		if (isCmdCompleted)
		{
            #if CLIFE_LITE_EN
            HET_GUI_UartRecvCallback(cmdBuf,frameLength+5);
            #endif
			memset(cmdBuf, 0, EXTERNAL_BUFFER_SIZE);
			isCmdCompleted = false;
		}

#endif

        usleep(10000);//10ms
    }
    mq_close(extInQueue);
	mq_close(extOutQueue);
    extInQueue = -1;
	extOutQueue = -1;

    return NULL;
}

/*
* 函数名称 : ClifeTask
* 功能描述 : clife简化版协议处理任务
* 参    数 : arg -- 任务参数
* 返回值   : 无
* 示    例 : 无
*/
/******************************************************************************/
static void* ClifeTask(void* arg)
/******************************************************************************/
{
    while (!extClifeQuit)
    {
        HET_GUI_Task();
        
        usleep(10000);//10ms
    }
    return NULL;
}

/*
* 函数名称 : ExternalInit
* 功能描述 : 外部任务初始化
* 参    数 : 无
* 返回值   : 无
* 示    例 : 无
*/
/******************************************************************************/
void ExternalInit(void)
/******************************************************************************/
{
    struct mq_attr qattr;

    qattr.mq_flags = 0;
    qattr.mq_maxmsg = EXT_MAX_QUEUE_SIZE;
    qattr.mq_msgsize = sizeof(ExternalEvent);

    extInQueue = mq_open("external_in", O_CREAT | O_NONBLOCK, 0644, &qattr);
    assert(extInQueue != -1);

    extOutQueue = mq_open("external_out", O_CREAT | O_NONBLOCK, 0644, &qattr);
    assert(extOutQueue != -1);

    extQuit = false;
    
#if defined(CFG_UART2_ENABLE) && !defined(CFG_DBG_UART2)
    #ifdef _WIN32
	if(ComInit(WIN32_COM, CFG_UART2_BAUDRATE)) {
		//ComInit Error
		mq_close(extInQueue);
		mq_close(extOutQueue);
		extInQueue = -1;
		extOutQueue = -1;
		return;
	}
    #else
	itpRegisterDevice(ITP_DEVICE_UART2, &itpDeviceUart2);
    ioctl(ITP_DEVICE_UART2, ITP_IOCTL_INIT, NULL);
    ioctl(ITP_DEVICE_UART2, ITP_IOCTL_RESET, CFG_UART2_BAUDRATE);	
    #endif
#endif

    pthread_create(&extTask, NULL, ExternalTask, NULL);

    //创建用户任务
    #if CLIFE_LITE_EN
    extClifeQuit = false;
    HET_GUI_Init();
    pthread_create(&extClifeTask, NULL, ClifeTask, NULL);
    #endif
}

/*
* 函数名称 : ExternalExit
* 功能描述 : 外部退出处理
* 参    数 : 无
* 返回值   : 无
* 示    例 : 无
*/
/******************************************************************************/
void ExternalExit(void)
/******************************************************************************/
{
    extQuit = true;
    pthread_join(extTask, NULL);

    #if CLIFE_LITE_EN
    extClifeQuit = true;
    pthread_join(extClifeTask, NULL);
    #endif
}

/*
* 函数名称 : ExternalInQueueReceive
* 功能描述 : 查询队列中是否有收到消息,如查询队列中是否有用户任务往UI发送的消息
* 参    数 : 无
* 返回值   : 无
* 示    例 : 无
*/
/******************************************************************************/
int ExternalInQueueReceive(ExternalEvent* ev)
/******************************************************************************/
{
    assert(ev);

    if (extQuit)
        return 0;

    if (mq_receive(extInQueue, (char*)ev, sizeof(ExternalEvent), 0) > 0)
        return 1;

    return 0;
}

/*
* 函数名称 : ExternalOutQueueReceive
* 功能描述 : 查询队列中是否有收到消息，如查询队列中是否有UI往用户任务发送的消息
* 参    数 : 无
* 返回值   : 无
* 示    例 : 无
*/
/******************************************************************************/
int ExternalOutQueueReceive(ExternalEvent* ev)
/******************************************************************************/
{
    assert(ev);

    if (extQuit)
        return 0;

    if (mq_receive(extOutQueue, (char*)ev, sizeof(ExternalEvent), 0) > 0)
        return 1;

    return 0;
}

/*
* 函数名称 : ExternalInQueueSend
* 功能描述 : 发送消息到队列中,用户任务往UI发送消息，
             比如串口任务接收到数据，根据数据发送对应的消息到UI，对UI进行更新或控制
* 参    数 : ev - 消息指针
* 返回值   : 无
* 示    例 : 无
*/
/******************************************************************************/
int ExternalInQueueSend(ExternalEvent* ev)
/******************************************************************************/
{
    assert(ev);

    if (extQuit)
        return -1;

    return mq_send(extInQueue, (char*)ev, sizeof(ExternalEvent), 0);
}

/*
* 函数名称 : ExternalOutQueueSend
* 功能描述 : 发送消息到队列中,UI往用户任务发送消息，
             比如按了一个按键，发一条消息，在用户任务中收到消息后，发出对应的串口数据
* 参    数 : ev - 消息指针
* 返回值   : 无
* 示    例 : 无
*/
/******************************************************************************/
int ExternalOutQueueSend(ExternalEvent* ev)
/******************************************************************************/
{
    assert(ev);

    if (extQuit)
        return -1;

    return mq_send(extOutQueue, (char*)ev, sizeof(ExternalEvent), 0);
}
/******************************* End of File (C) ******************************/

