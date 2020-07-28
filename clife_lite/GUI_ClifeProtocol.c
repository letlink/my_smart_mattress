/******************************************************************************\
**  版    权 :  深圳市和而泰智能控制股份有限公司所有（2020）
**  文件名称 :  GUI_ClifeProtocol.c
**  功能描述 :  基于C-LIFE简化版协议的GUI通信示例
**  作    者 :  vincent
**  日    期 :  2020.07.01
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/2020.07.01
                1 首次创建   
\******************************************************************************/


/******************************************************************************\
                                 Includes
\******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <assert.h>
#include <mqueue.h>
#include <string.h>
#include <unistd.h>
#include "ite/itp.h"
#include "../ctrlboard.h"
#include "../scene.h"
#include "SDL/SDL.h"

//#include "stm32f1xx.h"

#include "GUI_ClifeProtocol.h"
#include "GUI_DataConfig.h"

//#include "het_usart.h"
//#include "het_24cxx.h"
//#include "het_flash.h"
//#include "het_rgb.h"
//#include "het_w25qxx.h"


/******************************************************************************\
                             Variables definitions
\******************************************************************************/
static TS_HET_CP   s_GUI_CP = { 0 };
TS_HET_DATE g_Time;

//------------------------------------------------------------------------------
extern int ExternalInQueueSend(ExternalEvent* ev); 
/******************************************************************************\
                             Functions definitions
\******************************************************************************/ 
/*
* 函数名称 : reverse_byte
* 功能描述 : 大小端转换
* 参    数 : pBuf:数据指针
             num:数据长度
* 返回值   : 无
* 示    例 : 无
*/
/******************************************************************************/
static unsigned int reverse_byte(unsigned char *c, unsigned char num)
/******************************************************************************/
{
    unsigned int r = 0;
    int i;
    for (i = 0; i<num; i++)
    {
        r |= (*(c + i) << (((num - 1) * 8) - 8 * i));
    }
    return r;

}


#if CP_MODE == MASTER_MODE

/*
* 函数名称 : HET_ReportData_Decode_CallBack
* 功能描述 : MCU接收数据下发处理回调
* 参    数 : pBuf - 数据指针
             Len  - 数据长度
* 返回值   : 无
* 示    例 : HET_ReportData_Decode_CallBack(&pBuf,Len);
*/
/******************************************************************************/
static void HET_GUI_DownloadRxCallback(uint8_t *pBuf, uint16_t Len)
/******************************************************************************/  
{
    

    uint16_t TotalLen = 0;
    uint8_t *pRead = 0;
    uint8_t DataId = 0;
    uint8_t DataLen = 0;
    
    uint16_t Index = 0;
    TS_GUI_DATA *pGuiData = &g_GUI_Data;
    uint8_t *data = 0;
    uint32_t Temp = 0;
    //用户添加接收到的数据解析代码//  
    pRead = pBuf;
    TotalLen = Len;

    while (TotalLen)
    {
        DataId = *pRead++;
        DataLen = *pRead++;
        Temp = 0;
        switch (DataId)
        {
            case ID_ALL:
            
                break;

            
            case ID_DISPLAY_ON_OFF:
                memcpy((uint8_t*)&Temp, pRead, DataLen);
                //Temp = reverse_byte((uint8_t*)&Temp, DataLen);
                pGuiData->AirConOnOff = Temp;
                
                //UseDataToDoSomething();
                break;

            case ID_TIME_STAMP:
                memcpy((uint8_t*)&Temp, pRead, DataLen);
                //Temp = reverse_byte((uint8_t*)&Temp, DataLen);
                pGuiData->TimeStamp = Temp;

                //UseDataToDoSomething();
                break;

            case ID_AIR_CON_ON_OFF:
                memcpy((uint8_t*)&Temp, pRead, DataLen);
                //Temp = reverse_byte((uint8_t*)&Temp, DataLen);
                pGuiData->AirConOnOff = Temp;

                //UseDataToDoSomething();
                break;

            case ID_AIR_CON_MODE:
                memcpy((uint8_t*)&Temp, pRead, DataLen);
                //Temp = reverse_byte((uint8_t*)&Temp, DataLen);
                pGuiData->AirConMode = Temp;

                
                //UseDataToDoSomething();
                break;

            case ID_AIR_CON_FAN:
                memcpy((uint8_t*)&Temp, pRead, DataLen);
                //Temp = reverse_byte((uint8_t*)&Temp, DataLen);
                pGuiData->AirConFan = Temp;


                //UseDataToDoSomething();
                break;

            case ID_AIR_CON_SET_TEMP:
                memcpy((uint8_t*)&Temp, pRead, DataLen);
                //Temp = reverse_byte((uint8_t*)&Temp, DataLen);
                pGuiData->AirConSetTemp = Temp;


                //UseDataToDoSomething();
                break;

            case ID_AIR_CON_CURR_TEMP:
                memcpy((uint8_t*)&Temp, pRead, DataLen);
                //Temp = reverse_byte((uint8_t*)&Temp, DataLen);
                pGuiData->AirConCurrTemp = Temp;

                
                //UseDataToDoSomething();
                break;

            case ID_AIR_CON_ERR_CODE:
                memcpy((uint8_t*)&Temp, pRead, DataLen);
                //Temp = reverse_byte((uint8_t*)&Temp, DataLen);
                pGuiData->AirConErrCode = Temp;


                //UseDataToDoSomething();
                break;

            case ID_TEST_NUM_CNT:
                memcpy((uint8_t*)&Temp, pRead, DataLen);
                //Temp = reverse_byte((uint8_t*)&Temp, DataLen);
                pGuiData->TestNumCnt = Temp;


                //UseDataToDoSomething();
                break;

            case ID_TEST_METER_DEGREE:
                memcpy((uint8_t*)&Temp, pRead, DataLen);
                //Temp = reverse_byte((uint8_t*)&Temp, DataLen);
                pGuiData->TestMeterDegree = Temp;


                //UseDataToDoSomething();
                break;

            case ID_TEST_PROGRESS_BAR:
                memcpy((uint8_t*)&Temp, pRead, DataLen);
                //Temp = reverse_byte((uint8_t*)&Temp, DataLen);
                pGuiData->TestProgressBar = Temp;


                //UseDataToDoSomething();
                break;

            default:break;
        }
        pRead += DataLen;
        TotalLen = TotalLen - DataLen - 2;
    }
}

/*
* 函数名称 : HET_GUI_SyncTimeRxCallback
* 功能描述 : MCU接收网络时间处理回调
* 参    数 : pBuf - 数据指针
             Len  - 数据长度
* 返回值   : 无
* 示    例 : HET_Sync_Data_Decode_CallBack(&pBuf,Len);
*/
/******************************************************************************/
static void HET_GUI_SyncTimeRxCallback(uint8_t *pBuf, uint16_t Len)
/******************************************************************************/  
{
    //用户代码，添加同步时钟数据解析代码//
    /*
    g_Date.Type;	//时间格式(0:格林威治时间,1:北京时间)
    g_Date.Year;	//年
    g_Date.Month;	//月
    g_Date.Day;		//日
    g_Date.Hour;	//时
    g_Date.Min;		//分
    g_Date.Sec;		//秒
    g_Date.Week;	//星期
    */
    memcpy(&g_Time, pBuf, sizeof(TS_HET_DATE));
}

/*
* 函数名称 : HET_GUI_SyncDataTxCallback
* 功能描述 : MCU发送数据同步处理回调
* 参    数 : 无
* 返回值   : 无
* 示    例 : 无
*/
/******************************************************************************/
static void HET_GUI_SyncDataTxCallback(void)
/******************************************************************************/  
{
    uint8_t res;
    res = HET_GUI_SendDataFrame(CMD_DATASYNC, ID_ALL, LEN_ALL, (uint8_t*)&g_GUI_Data);
}

/*
* 函数名称 : HET_GUI_UartSendCallback
* 功能描述 : 串口发送回调
* 参    数 : pBuf:数据指针
             Len:数据长度
* 返回值   : 无
* 示    例 : 无
*/
/******************************************************************************/
static void HET_GUI_UartSendCallback(uint8_t *pBuf, uint16_t Len)
/******************************************************************************/
{
    //根据单片机型号对此串口发送函数进行调整//

    //Uart1Send(pBuf, Len);							//调用串口数据发送函数
    #if 1
    if((pBuf == 0) || (Len == 0)) return;
    for(int i=0;i<Len;i++)
    {
        printf("%02X ", pBuf[i]);
    }
    printf("\r\n");
    #endif
    //--------------------------------------//
}


#else
/*
* 函数名称 : HET_GUI_UploadRxCallback
* 功能描述 : 模组收到数据上传处理回调
* 参    数 : pBuf - 数据指针
             Len  - 数据长度
* 返回值   : 无
* 示    例 : 无
*/
/******************************************************************************/
static void HET_GUI_UploadRxCallback(uint8_t *pBuf, uint16_t Len)
/******************************************************************************/
{
    ExternalEvent ev = {0};

    uint16_t TotalLen = 0;
    uint8_t *pRead = 0;
    uint8_t DataId = 0;
    uint8_t DataLen = 0;
    
    uint16_t Index = 0;
    
    uint8_t *data = 0;
    uint32_t Temp = 0;
    TS_GUI_DATA *pThis = &g_GUI_Data;
    //用户添加接收到的数据解析代码//  
    pRead = pBuf;
    TotalLen = Len;

    DEBUG("\r\n%s()", __func__);

    while (TotalLen)
    {
        Temp = 0;
        DataId = *pRead++;
        if(DataId == 0) //ID为0用于数据同步指令
        {
            DEBUG("\r\nReceived error data id: %d", DataId);
            return;
        }
        
        DataLen = *pRead++;
        if(DataLen <= 4) //数据长度应小于4
        {
            memcpy((uint8_t*)&Temp, pRead, DataLen);//单个数据只支持<=4字节的数据
            if ((DATA_ENDIAN == 1) && (DataLen > 1)) //大端模式
            {
                Temp = reverse_byte((uint8_t*)&Temp, DataLen);
            }
        }
        else
        {
            DEBUG("\r\nReceived error data id: %d", DataLen);
        }

        switch (DataId)
        {
            case ID_ALL:
            
                break;

            
            case ID_MATTRESS_MODE:
                pThis->mattress_mode = Temp;
                DEBUG("\r\npThis->mattress_mode: 0X%02X", pThis->mattress_mode);
                //UseDataToDoSomething();
                break;

            case ID_MASSAGE_ON_OFF:
                pThis->massage_on_off = Temp;
                DEBUG("\r\npThis->massage_on_off: 0X%02X", pThis->massage_on_off);
                //UseDataToDoSomething();
                break;

            case ID_MASSAGE_MODE:
                pThis->massage_mode = Temp;
                DEBUG("\r\npThis->massage_mode: 0X%02X", pThis->massage_mode);
                //UseDataToDoSomething();
                break;

            case ID_MASSAGE_LEVEL:
                pThis->massage_level = Temp;
                DEBUG("\r\npThis->massage_level: 0X%02X", pThis->massage_level);
                //UseDataToDoSomething();
                break;

            case ID_MASSAGE_TIME_SET:
                pThis->massage_time_set = Temp;
                DEBUG("\r\npThis->massage_time: 0X%02X", pThis->massage_time_set);
                //UseDataToDoSomething();
                break;

            case ID_SLEEP_AID_ON_OFF:
                pThis->sleep_aid_on_off = Temp;
                DEBUG("\r\npThis->sleep_aid_on_off: 0X%02X", pThis->sleep_aid_on_off);

                //UseDataToDoSomething();
                break;

            case ID_SLEEP_AID_MUSIC_ON_OFF:
                pThis->sleep_aid_music_on_off = Temp;
                DEBUG("\r\npThis->sleep_aid_music_on_off: 0X%02X", pThis->sleep_aid_music_on_off);
            
                //UseDataToDoSomething();
                break;

            case ID_SLEEP_AID_MUSIC_INDEX:
                pThis->sleep_aid_music_index = Temp;
                DEBUG("\r\npThis->sleep_aid_music_index: 0X%02X", pThis->sleep_aid_music_index);

                //UseDataToDoSomething();
                break;

            case ID_SLEEP_AID_LIGHT_LEVEL:
                pThis->sleep_aid_light_level = Temp;
                DEBUG("\r\npThis->sleep_aid_light_level: 0X%02X", pThis->sleep_aid_light_level);

                //UseDataToDoSomething();
                break;

            case ID_SLEEP_AID_TIME:
                pThis->sleep_aid_time = Temp;
                DEBUG("\r\npThis->sleep_aid_time: 0X%02X", pThis->sleep_aid_time);

                //UseDataToDoSomething();
                break;

            case ID_SLEEP_AID_MODE:
                pThis->sleep_aid_mode = Temp;
                DEBUG("\r\npThis->sleep_aid_mode: 0X%02X", pThis->sleep_aid_mode);

                //UseDataToDoSomething();
                break;

            case ID_LAMP_ON_OFF:
                pThis->lamp_on_off = Temp;
                DEBUG("\r\npThis->lamp_on_off: 0X%02X", pThis->lamp_on_off);

                //UseDataToDoSomething();
                break;

            case ID_LAMP_LEVEL:
                pThis->lamp_level = Temp;
                DEBUG("\r\npThis->lamp_level: 0X%02X", pThis->lamp_level);

                //UseDataToDoSomething();
                break;

            case ID_LAMP_COLOUR_TEMPERATURE:
                pThis->lamp_colour_temperature = Temp;
                DEBUG("\r\npThis->lamp_colour_temperature: 0X%02X", pThis->lamp_colour_temperature);

                //UseDataToDoSomething();
                break;

            case ID_ALARM_MUSIC_ON_OFF:
                pThis->alarm_music_on_off = Temp;
                DEBUG("\r\npThis->alarm_music_on_off: 0X%02X", pThis->alarm_music_on_off);

                //UseDataToDoSomething();
                break;

            case ID_ALARM_MUSIC_INDEX:
                pThis->alarm_music_index = Temp;
                DEBUG("\r\npThis->alarm_music_index: 0X%02X", pThis->alarm_music_index);

                //UseDataToDoSomething();
                break;

            case ID_ALARM_OTHER_WAYS:
                pThis->alarm_other_ways = Temp;
                DEBUG("\r\npThis->alarm_other_ways: 0X%02X", pThis->alarm_other_ways);

                //UseDataToDoSomething();
                break;

            case ID_ALARM1_ON_OFF:
                pThis->alarm1_on_off = Temp;
                DEBUG("\r\npThis->alarm1_on_off: 0X%02X", pThis->alarm1_on_off);

                //UseDataToDoSomething();
                break;

            case ID_ALARM1_HOUR:
                pThis->alarm1_hour = Temp;
                DEBUG("\r\npThis->alarm1_hour: 0X%02X", pThis->alarm1_hour);

                //UseDataToDoSomething();
                break;

            case ID_ALARM1_MINUTE:
                pThis->alarm1_minute = Temp;
                DEBUG("\r\npThis->alarm1_minute: 0X%02X", pThis->alarm1_minute);

                //UseDataToDoSomething();
                break;

            case ID_ALARM1_WEEKDAY:
                pThis->alarm1_weekday = Temp;
                DEBUG("\r\npThis->alarm1_weekday: 0X%02X", pThis->alarm1_weekday);

                //UseDataToDoSomething();
                break;

            case ID_ALARM2_ON_OFF:
                pThis->alarm2_on_off = Temp;
                DEBUG("\r\npThis->alarm2_on_off: 0X%02X", pThis->alarm2_on_off);

                //UseDataToDoSomething();
                break;

            case ID_ALARM2_HOUR:
                pThis->alarm2_hour = Temp;
                DEBUG("\r\npThis->alarm2_hour: 0X%02X", pThis->alarm2_hour);

                //UseDataToDoSomething();
                break;

            case ID_ALARM2_MINUTE:
                pThis->alarm2_minute = Temp;
                DEBUG("\r\npThis->alarm2_minute: 0X%02X", pThis->alarm2_minute);

                //UseDataToDoSomething();
                break;

            case ID_ALARM2_WEEKDAY:
                pThis->alarm2_weekday = Temp;
                DEBUG("\r\npThis->alarm2_weekday: 0X%02X", pThis->alarm2_weekday);

                //UseDataToDoSomething();
                break;

            case ID_ALARM3_ON_OFF:
                pThis->alarm3_on_off = Temp;
                DEBUG("\r\npThis->alarm3_on_off: 0X%02X", pThis->alarm3_on_off);

                //UseDataToDoSomething();
                break;

            case ID_ALARM3_HOUR:
                pThis->alarm3_hour = Temp;
                DEBUG("\r\npThis->alarm3_hour: 0X%02X", pThis->alarm3_hour);

                //UseDataToDoSomething();
                break;

            case ID_ALARM3_MINUTE:
                pThis->alarm3_minute = Temp;
                DEBUG("\r\npThis->alarm3_minute: 0X%02X", pThis->alarm3_minute);

                //UseDataToDoSomething();
                break;

            case ID_ALARM3_WEEKDAY:
                pThis->alarm3_weekday = Temp;
                DEBUG("\r\npThis->alarm3_weekday: 0X%02X", pThis->alarm3_weekday);

                //UseDataToDoSomething();
                break;

            case ID_STOP_SNORE_ON_OFF:
                pThis->stop_snore_on_off = Temp;
                DEBUG("\r\npThis->stop_snore_on_off: 0X%02X", pThis->stop_snore_on_off);

                //UseDataToDoSomething();
                break;

            case ID_SMART_SET_IN_BED:
                pThis->smart_set_in_bed = Temp;
                DEBUG("\r\npThis->sleep_aid_mode: 0X%02X", pThis->sleep_aid_mode);

                //UseDataToDoSomething();
                break;

            case ID_SMART_SET_AFTER_SLEEP:
                pThis->smart_set_after_sleep = Temp;
                DEBUG("\r\npThis->smart_set_after_sleep: 0X%02X", pThis->smart_set_after_sleep);

                //UseDataToDoSomething();
                break;

            case ID_SMART_SET_OUT_BED:
                pThis->smart_set_out_bed = Temp;
                DEBUG("\r\npThis->smart_set_out_bed: 0X%02X", pThis->smart_set_out_bed);

                //UseDataToDoSomething();
                break;

            case ID_TEMPERATURE:
                pThis->temperature = Temp;
                DEBUG("\r\npThis->temperature: 0X%02X", pThis->temperature);

                //UseDataToDoSomething();
                break;

            case ID_HUMIDITY:
                pThis->humidity = Temp;
                DEBUG("\r\npThis->humidity: 0X%02X", pThis->humidity);

                //UseDataToDoSomething();
                break;

            case ID_BACKLIGHT:
                pThis->backlight = Temp;
                DEBUG("\r\npThis->backlight: 0X%02X", pThis->backlight);

                //UseDataToDoSomething();
                break;

            case ID_TIME_STAMP:
                pThis->time_stamp = Temp;
                DEBUG("\r\npThis->time_stamp: 0X%02X", pThis->time_stamp);

                //UseDataToDoSomething();
                break;

            case ID_ERROR_CODE:
                pThis->error_code = Temp;
                DEBUG("\r\npThis->error_code: 0X%02X", pThis->error_code);

                //UseDataToDoSomething();
                break;

            default:break;
        }
        pRead += DataLen;
        TotalLen = TotalLen - DataLen - 2;
    }

    //发送消息到UI任务，更新UI控件
    ev.type = EXTERNAL_UART_UPLOAD_RX;
    ev.arg1 = (int)&g_GUI_Data;
    ExternalInQueueSend(&ev);
    return;
}

/*
* 函数名称 : HET_GUI_DevInfoRxCallback
* 功能描述 : 模组收到设备信息处理回调
* 参    数 : pThis - 数据指针
* 返回值   : 无
* 示    例 : 无
*/
/******************************************************************************/
static void HET_GUI_DevInfoRxCallback(TS_HET_CP_DEVICEINFO* pThis)
/******************************************************************************/
{
    ExternalEvent ev = {0};
    //DoDevInfoProcess();

    DEBUG("\r\n%s()", __func__);
    DEBUG("\r\nSoftware Ver:%d", pThis->SoftwareVer);
    DEBUG("\r\nHardware Ver:%d", pThis->HardwareVer);
    DEBUG("\r\nProduct model:%s", pThis->DeviceProductModel);
    DEBUG("\r\nReseved:%s", pThis->Reserve2);

    //发送消息到UI任务，更新UI控件
    ev.type = EXTERNAL_UART_DEV_INFO_RX;
    ExternalInQueueSend(&ev);
    
    return;
}

/*
* 函数名称 : HET_GUI_SyncDataRxCallback
* 功能描述 : 模组收到数据同步处理回调
* 参    数 : pBuf - 数据指针
Len  - 数据长度
* 返回值   : 无
* 示    例 : 无
*/
/******************************************************************************/
static void HET_GUI_SyncDataRxCallback(uint8_t *pBuf, uint16_t Len)
/******************************************************************************/
{
    ExternalEvent ev = {0};
    //DoSyncDataProcess();

    DEBUG("\r\n%s()", __func__);

    //发送消息到UI任务，更新UI控件
    ev.type = EXTERNAL_UART_SYNC_DATA_RX;
    ExternalInQueueSend(&ev);
    
    return;
}

/*
* 函数名称 : HET_GUI_GetSysTimeInfo
* 功能描述 : 模组获取系统时间回调
* 参    数 : pBuf - 数据指针
* 返回值   : 无
* 示    例 : 无
*/
/******************************************************************************/
static void HET_GUI_GetSysTimeInfo(TS_HET_CP_TIME* pTime)
/******************************************************************************/
{
    //DoUpdateTimeProcess(); //for example as follows
    pTime->Year = 20;
    pTime->Month = 1;
    pTime->Date = 1;
    pTime->Hours = 12;
    pTime->Minutes = 0;
    pTime->Seconds = 0;
    pTime->Weekday = 3;
    
    DEBUG("\r\n%s()", __func__);
    DEBUG("\r\n%d-%02d-%02d %02d:%02d:%02d weekday:%d", 
        pTime->Year, pTime->Month, pTime->Date, pTime->Hours, pTime->Minutes, pTime->Seconds, pTime->Weekday
        );
    return;
}

/*
* 函数名称 : HET_GUI_FactoryTest
* 功能描述 : 模组产测回调
* 参    数 : 无
* 返回值   : 无
* 示    例 : 无
*/
/******************************************************************************/
static void HET_GUI_FactoryTest(void)
/******************************************************************************/
{
    ExternalEvent ev = {0};
    //DoFactoryTest();
    static uint8_t test = 0;
    DEBUG("\r\n%s()", __func__);
    DEBUG("%d", test++);

    //发送消息到UI任务，更新UI控件
    ev.type = EXTERNAL_UART_SYNC_DATA_RX;
    ExternalInQueueSend(&ev);
    
    return;
}

/*
* 函数名称 : HET_GUI_ResetProcess
* 功能描述 : 模组复位处理
* 参    数 : 无
* 返回值   : 无
* 示    例 : 无
*/
/******************************************************************************/
static void HET_GUI_ResetProcess(void)
/******************************************************************************/
{
    ExternalEvent ev = {0};
    //DoResetProcess();

    DEBUG("\r\n%s()", __func__);

    //发送消息到UI任务，更新UI控件
    ev.type = EXTERNAL_UART_SYNC_DATA_RX;
    ExternalInQueueSend(&ev);
    
    return;
}

/*
* 函数名称 : HET_GUI_UartSendCallback
* 功能描述 : 串口发送回调
* 参    数 : pBuf:数据指针
             Len:数据长度
* 返回值   : 无
* 示    例 : 无
*/
/******************************************************************************/
static void HET_GUI_UartSendCallback(uint8_t *pBuf, uint16_t Len)
/******************************************************************************/
{
    //根据单片机型号对此串口发送函数进行调整//

    //Uart1Send(pBuf, Len);							//调用串口数据发送函数
    #ifdef _WIN32
    UartSend(MY_WIN32_COM, (char*)pBuf, Len);
    #else
	write(ITP_DEVICE_UART2, pBuf, Len);
    #endif

    //--------------------------------------//
}
#endif


/*
* 函数名称 : HET_GUI_UartRecvCallback
* 功能描述 : 串口接收回调，用户需将其放置于串口接收完成处
* 参    数 : pBuf:数据指针
             Len:数据长度
* 返回值   : 无
* 示    例 : 无
*/
/******************************************************************************/
void HET_GUI_UartRecvCallback(uint8_t *pBuf, uint16_t Len)
/******************************************************************************/
{
    HET_CP_Drive_UsartRecvISR(&s_GUI_CP, pBuf, Len);	//串口接收数据处理
} 

/*
* 函数名称 : HET_GUI_SendDataFrame
* 功能描述 : 协议数据发送，用户可在需要发送数据的地方调用
* 参    数 : Cmd      - 功能码
             DataId   - 数据ID
             DataLen  - 数据长度
             pDataBuf - 数据指针
* 返回值   : 无
* 示    例 : 无
*/
/******************************************************************************/
uint8_t HET_GUI_SendDataFrame(TE_HET_CP_CMD Cmd, uint8_t DataId, uint8_t DataLen, uint8_t* pDataBuf)
/******************************************************************************/
{
    TS_HET_CP*   pThis = &s_GUI_CP;
    uint32_t Tmp = 0;
    uint8_t res = 0;
    
    #if CP_MODE == MASTER_MODE //主机模式即MCU端不用考虑大小端
    res = HET_CP_SendDataFrame(pThis, Cmd, DataId, pDataBuf, DataLen);
    #else
    if(DATA_ENDIAN) //设备端为大端模式，发出去的数据需转换
    {
        if((Cmd = CMD_DOWNLOAD) && (DataId != 0))
        {
            Tmp = reverse_byte(pDataBuf, DataLen);
            res = HET_CP_SendDataFrame(pThis, Cmd, DataId, (uint8_t*)&Tmp, DataLen);
        }
    }
    else
    {
        res = HET_CP_SendDataFrame(pThis, Cmd, DataId, pDataBuf, DataLen);
    }
    
    #endif
    return res;
}

/*
* 函数名称 : HET_GUI_GetDataHandle
* 功能描述 : 获取协议数据指针，用户可在需要获取数据的地方调用
* 参    数 : 无
* 返回值   : 无
* 示    例 : 无
*/
/******************************************************************************/
TS_GUI_DATA* HET_GUI_GetDataHandle(void)
/******************************************************************************/
{
    return &g_GUI_Data;
}


/*
* 函数名称 : HET_GUI_Init
* 功能描述 : 通信协议组件初始化，用户需将其放置于串口初始化处
* 参    数 : 无
* 返回值   : 无
* 示    例 : 无
*/
/******************************************************************************/
void HET_GUI_Init(void)
/******************************************************************************/
{
	////根据使用的MCU型号配置WiFi通讯需要的串口及复位引脚资源//
 //   GPIO_InitTypeDef GPIO_InitStruct;

 //   //WiFi模块复位脚初始化
	//WIFI_RESET_RCC_CLK_ENABLE();                                   
	//GPIO_InitStruct.Pin   = WIFI_RESET_PIN;             
 //   GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
 //   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
 //   HAL_GPIO_Init(WIFI_RESET_PORT, &GPIO_InitStruct);

 //   //WiFi串口初始化
 //   UART3_Init(WIFI_BAUDRATE);			
 //   //WiFi接收处理函数指针
 //   g_pUart3RxCallback = HET_Wifi_RecvCB;
	////------------------------------------------------------//
    /* 函数指针初始化 */

    s_GUI_CP.Func.UartSendCallback = HET_GUI_UartSendCallback;    // 串口发送
#if CP_MODE == MASTER_MODE
    s_GUI_CP.Func.DownloadRxCallback   = HET_GUI_DownloadRxCallback;      // MCU接收数据下发处理回调
    s_GUI_CP.Func.SyncTimeRxCallback   = HET_GUI_SyncTimeRxCallback;      // MCU接收网络时间处理回调
    s_GUI_CP.Func.SyncDataTxCallback   = HET_GUI_SyncDataTxCallback;      // MCU发送数据同步处理回调
#else
    s_GUI_CP.Func.UploadRxCallback     = HET_GUI_UploadRxCallback;        // 模组收到数据上传处理回调
    s_GUI_CP.Func.DevInfoRxCallback    = HET_GUI_DevInfoRxCallback;       // 模组收到设备信息处理回调
    s_GUI_CP.Func.SyncDataRxCallback   = HET_GUI_SyncDataRxCallback;      // 模组收到数据同步处理回调
    s_GUI_CP.Func.GetSysTimeInfo       = HET_GUI_GetSysTimeInfo;          // 模组收到时间同步处理回调
    s_GUI_CP.Func.FactoryTest          = HET_GUI_FactoryTest;             // 模组收到产测处理回调
    s_GUI_CP.Func.ResetProcess         = HET_GUI_ResetProcess;          // 模组复位处理回调
#endif
    /* 组件初始化 */
    if (HET_CP_Open(&s_GUI_CP))
    {
        // 初始化失败处理

    }
}  


/*
* 函数名称 : HET_GUI_Task
* 功能描述 : GUI任务，用户需将其放置到10ms循环中
* 参    数 : 无
* 返回值   : 无
* 示    例 : 无
*/
/******************************************************************************/
void HET_GUI_Task(void)
{
    HET_CP_Task(&s_GUI_CP);
}
/******************************* End of File (C) ******************************/

