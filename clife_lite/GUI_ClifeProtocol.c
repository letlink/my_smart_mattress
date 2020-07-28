/******************************************************************************\
**  ��    Ȩ :  �����кͶ�̩���ܿ��ƹɷ����޹�˾���У�2020��
**  �ļ����� :  GUI_ClifeProtocol.c
**  �������� :  ����C-LIFE�򻯰�Э���GUIͨ��ʾ��
**  ��    �� :  vincent
**  ��    �� :  2020.07.01
**  ��    �� :  V0.0.1
**  �����¼ :  V0.0.1/2020.07.01
                1 �״δ���   
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
* �������� : reverse_byte
* �������� : ��С��ת��
* ��    �� : pBuf:����ָ��
             num:���ݳ���
* ����ֵ   : ��
* ʾ    �� : ��
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
* �������� : HET_ReportData_Decode_CallBack
* �������� : MCU���������·�����ص�
* ��    �� : pBuf - ����ָ��
             Len  - ���ݳ���
* ����ֵ   : ��
* ʾ    �� : HET_ReportData_Decode_CallBack(&pBuf,Len);
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
    //�û���ӽ��յ������ݽ�������//  
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
* �������� : HET_GUI_SyncTimeRxCallback
* �������� : MCU��������ʱ�䴦��ص�
* ��    �� : pBuf - ����ָ��
             Len  - ���ݳ���
* ����ֵ   : ��
* ʾ    �� : HET_Sync_Data_Decode_CallBack(&pBuf,Len);
*/
/******************************************************************************/
static void HET_GUI_SyncTimeRxCallback(uint8_t *pBuf, uint16_t Len)
/******************************************************************************/  
{
    //�û����룬���ͬ��ʱ�����ݽ�������//
    /*
    g_Date.Type;	//ʱ���ʽ(0:��������ʱ��,1:����ʱ��)
    g_Date.Year;	//��
    g_Date.Month;	//��
    g_Date.Day;		//��
    g_Date.Hour;	//ʱ
    g_Date.Min;		//��
    g_Date.Sec;		//��
    g_Date.Week;	//����
    */
    memcpy(&g_Time, pBuf, sizeof(TS_HET_DATE));
}

/*
* �������� : HET_GUI_SyncDataTxCallback
* �������� : MCU��������ͬ������ص�
* ��    �� : ��
* ����ֵ   : ��
* ʾ    �� : ��
*/
/******************************************************************************/
static void HET_GUI_SyncDataTxCallback(void)
/******************************************************************************/  
{
    uint8_t res;
    res = HET_GUI_SendDataFrame(CMD_DATASYNC, ID_ALL, LEN_ALL, (uint8_t*)&g_GUI_Data);
}

/*
* �������� : HET_GUI_UartSendCallback
* �������� : ���ڷ��ͻص�
* ��    �� : pBuf:����ָ��
             Len:���ݳ���
* ����ֵ   : ��
* ʾ    �� : ��
*/
/******************************************************************************/
static void HET_GUI_UartSendCallback(uint8_t *pBuf, uint16_t Len)
/******************************************************************************/
{
    //���ݵ�Ƭ���ͺŶԴ˴��ڷ��ͺ������е���//

    //Uart1Send(pBuf, Len);							//���ô������ݷ��ͺ���
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
* �������� : HET_GUI_UploadRxCallback
* �������� : ģ���յ������ϴ�����ص�
* ��    �� : pBuf - ����ָ��
             Len  - ���ݳ���
* ����ֵ   : ��
* ʾ    �� : ��
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
    //�û���ӽ��յ������ݽ�������//  
    pRead = pBuf;
    TotalLen = Len;

    DEBUG("\r\n%s()", __func__);

    while (TotalLen)
    {
        Temp = 0;
        DataId = *pRead++;
        if(DataId == 0) //IDΪ0��������ͬ��ָ��
        {
            DEBUG("\r\nReceived error data id: %d", DataId);
            return;
        }
        
        DataLen = *pRead++;
        if(DataLen <= 4) //���ݳ���ӦС��4
        {
            memcpy((uint8_t*)&Temp, pRead, DataLen);//��������ֻ֧��<=4�ֽڵ�����
            if ((DATA_ENDIAN == 1) && (DataLen > 1)) //���ģʽ
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

    //������Ϣ��UI���񣬸���UI�ؼ�
    ev.type = EXTERNAL_UART_UPLOAD_RX;
    ev.arg1 = (int)&g_GUI_Data;
    ExternalInQueueSend(&ev);
    return;
}

/*
* �������� : HET_GUI_DevInfoRxCallback
* �������� : ģ���յ��豸��Ϣ����ص�
* ��    �� : pThis - ����ָ��
* ����ֵ   : ��
* ʾ    �� : ��
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

    //������Ϣ��UI���񣬸���UI�ؼ�
    ev.type = EXTERNAL_UART_DEV_INFO_RX;
    ExternalInQueueSend(&ev);
    
    return;
}

/*
* �������� : HET_GUI_SyncDataRxCallback
* �������� : ģ���յ�����ͬ������ص�
* ��    �� : pBuf - ����ָ��
Len  - ���ݳ���
* ����ֵ   : ��
* ʾ    �� : ��
*/
/******************************************************************************/
static void HET_GUI_SyncDataRxCallback(uint8_t *pBuf, uint16_t Len)
/******************************************************************************/
{
    ExternalEvent ev = {0};
    //DoSyncDataProcess();

    DEBUG("\r\n%s()", __func__);

    //������Ϣ��UI���񣬸���UI�ؼ�
    ev.type = EXTERNAL_UART_SYNC_DATA_RX;
    ExternalInQueueSend(&ev);
    
    return;
}

/*
* �������� : HET_GUI_GetSysTimeInfo
* �������� : ģ���ȡϵͳʱ��ص�
* ��    �� : pBuf - ����ָ��
* ����ֵ   : ��
* ʾ    �� : ��
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
* �������� : HET_GUI_FactoryTest
* �������� : ģ�����ص�
* ��    �� : ��
* ����ֵ   : ��
* ʾ    �� : ��
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

    //������Ϣ��UI���񣬸���UI�ؼ�
    ev.type = EXTERNAL_UART_SYNC_DATA_RX;
    ExternalInQueueSend(&ev);
    
    return;
}

/*
* �������� : HET_GUI_ResetProcess
* �������� : ģ�鸴λ����
* ��    �� : ��
* ����ֵ   : ��
* ʾ    �� : ��
*/
/******************************************************************************/
static void HET_GUI_ResetProcess(void)
/******************************************************************************/
{
    ExternalEvent ev = {0};
    //DoResetProcess();

    DEBUG("\r\n%s()", __func__);

    //������Ϣ��UI���񣬸���UI�ؼ�
    ev.type = EXTERNAL_UART_SYNC_DATA_RX;
    ExternalInQueueSend(&ev);
    
    return;
}

/*
* �������� : HET_GUI_UartSendCallback
* �������� : ���ڷ��ͻص�
* ��    �� : pBuf:����ָ��
             Len:���ݳ���
* ����ֵ   : ��
* ʾ    �� : ��
*/
/******************************************************************************/
static void HET_GUI_UartSendCallback(uint8_t *pBuf, uint16_t Len)
/******************************************************************************/
{
    //���ݵ�Ƭ���ͺŶԴ˴��ڷ��ͺ������е���//

    //Uart1Send(pBuf, Len);							//���ô������ݷ��ͺ���
    #ifdef _WIN32
    UartSend(MY_WIN32_COM, (char*)pBuf, Len);
    #else
	write(ITP_DEVICE_UART2, pBuf, Len);
    #endif

    //--------------------------------------//
}
#endif


/*
* �������� : HET_GUI_UartRecvCallback
* �������� : ���ڽ��ջص����û��轫������ڴ��ڽ�����ɴ�
* ��    �� : pBuf:����ָ��
             Len:���ݳ���
* ����ֵ   : ��
* ʾ    �� : ��
*/
/******************************************************************************/
void HET_GUI_UartRecvCallback(uint8_t *pBuf, uint16_t Len)
/******************************************************************************/
{
    HET_CP_Drive_UsartRecvISR(&s_GUI_CP, pBuf, Len);	//���ڽ������ݴ���
} 

/*
* �������� : HET_GUI_SendDataFrame
* �������� : Э�����ݷ��ͣ��û�������Ҫ�������ݵĵط�����
* ��    �� : Cmd      - ������
             DataId   - ����ID
             DataLen  - ���ݳ���
             pDataBuf - ����ָ��
* ����ֵ   : ��
* ʾ    �� : ��
*/
/******************************************************************************/
uint8_t HET_GUI_SendDataFrame(TE_HET_CP_CMD Cmd, uint8_t DataId, uint8_t DataLen, uint8_t* pDataBuf)
/******************************************************************************/
{
    TS_HET_CP*   pThis = &s_GUI_CP;
    uint32_t Tmp = 0;
    uint8_t res = 0;
    
    #if CP_MODE == MASTER_MODE //����ģʽ��MCU�˲��ÿ��Ǵ�С��
    res = HET_CP_SendDataFrame(pThis, Cmd, DataId, pDataBuf, DataLen);
    #else
    if(DATA_ENDIAN) //�豸��Ϊ���ģʽ������ȥ��������ת��
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
* �������� : HET_GUI_GetDataHandle
* �������� : ��ȡЭ������ָ�룬�û�������Ҫ��ȡ���ݵĵط�����
* ��    �� : ��
* ����ֵ   : ��
* ʾ    �� : ��
*/
/******************************************************************************/
TS_GUI_DATA* HET_GUI_GetDataHandle(void)
/******************************************************************************/
{
    return &g_GUI_Data;
}


/*
* �������� : HET_GUI_Init
* �������� : ͨ��Э�������ʼ�����û��轫������ڴ��ڳ�ʼ����
* ��    �� : ��
* ����ֵ   : ��
* ʾ    �� : ��
*/
/******************************************************************************/
void HET_GUI_Init(void)
/******************************************************************************/
{
	////����ʹ�õ�MCU�ͺ�����WiFiͨѶ��Ҫ�Ĵ��ڼ���λ������Դ//
 //   GPIO_InitTypeDef GPIO_InitStruct;

 //   //WiFiģ�鸴λ�ų�ʼ��
	//WIFI_RESET_RCC_CLK_ENABLE();                                   
	//GPIO_InitStruct.Pin   = WIFI_RESET_PIN;             
 //   GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
 //   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
 //   HAL_GPIO_Init(WIFI_RESET_PORT, &GPIO_InitStruct);

 //   //WiFi���ڳ�ʼ��
 //   UART3_Init(WIFI_BAUDRATE);			
 //   //WiFi���մ�����ָ��
 //   g_pUart3RxCallback = HET_Wifi_RecvCB;
	////------------------------------------------------------//
    /* ����ָ���ʼ�� */

    s_GUI_CP.Func.UartSendCallback = HET_GUI_UartSendCallback;    // ���ڷ���
#if CP_MODE == MASTER_MODE
    s_GUI_CP.Func.DownloadRxCallback   = HET_GUI_DownloadRxCallback;      // MCU���������·�����ص�
    s_GUI_CP.Func.SyncTimeRxCallback   = HET_GUI_SyncTimeRxCallback;      // MCU��������ʱ�䴦��ص�
    s_GUI_CP.Func.SyncDataTxCallback   = HET_GUI_SyncDataTxCallback;      // MCU��������ͬ������ص�
#else
    s_GUI_CP.Func.UploadRxCallback     = HET_GUI_UploadRxCallback;        // ģ���յ������ϴ�����ص�
    s_GUI_CP.Func.DevInfoRxCallback    = HET_GUI_DevInfoRxCallback;       // ģ���յ��豸��Ϣ����ص�
    s_GUI_CP.Func.SyncDataRxCallback   = HET_GUI_SyncDataRxCallback;      // ģ���յ�����ͬ������ص�
    s_GUI_CP.Func.GetSysTimeInfo       = HET_GUI_GetSysTimeInfo;          // ģ���յ�ʱ��ͬ������ص�
    s_GUI_CP.Func.FactoryTest          = HET_GUI_FactoryTest;             // ģ���յ����⴦��ص�
    s_GUI_CP.Func.ResetProcess         = HET_GUI_ResetProcess;          // ģ�鸴λ����ص�
#endif
    /* �����ʼ�� */
    if (HET_CP_Open(&s_GUI_CP))
    {
        // ��ʼ��ʧ�ܴ���

    }
}  


/*
* �������� : HET_GUI_Task
* �������� : GUI�����û��轫����õ�10msѭ����
* ��    �� : ��
* ����ֵ   : ��
* ʾ    �� : ��
*/
/******************************************************************************/
void HET_GUI_Task(void)
{
    HET_CP_Task(&s_GUI_CP);
}
/******************************* End of File (C) ******************************/

