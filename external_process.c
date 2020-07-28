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
/******************************************************************************\
                                 Includes
\******************************************************************************/
#include <assert.h>
#include <stdlib.h>
#include "SDL/SDL.h"
#include "ctrlboard.h"
#include "scene.h"
#include "ite/itp.h"
#include "clife_lite/GUI_ClifeProtocol.h"
#include "clife_lite/GUI_DataConfig.h"
/******************************************************************************\
                             Variables definitions
\******************************************************************************/
static TS_GUI_DATA GuiDataLast;
extern bool MainMassageCheckBoxOnPress(ITUWidget* widget, char* param);
/*
* 函数名称 : ExternalProcessUartUploadRx
* 功能描述 : 收到串口任务发来的消息，根据串口发来的消息更新控件
* 参    数 : ev -- 消息指针
* 返回值   : 无
* 示    例 : 无
*/
/******************************************************************************/
static void ExternalProcessUartUploadRx(ExternalEvent* ev)
/******************************************************************************/
{
    char str[20];
    TS_GUI_DATA* pThis = (TS_GUI_DATA*)ev->arg1;
    
    ITUWidget *pWidget = 0;
    
    //--------------------更新layer_main中的控件--------------------------

    switch(g_GUI_Data.mattress_mode)
    {
        case 1:
            pWidget = ituSceneFindWidget(&theScene, "mainSleepModeRadioBox");
            ituRadioBoxSetChecked((ITURadioBox *)pWidget, true);
        break;

        case 2:
            pWidget = ituSceneFindWidget(&theScene, "mainSmartModeRadioBox");
            ituRadioBoxSetChecked((ITURadioBox *)pWidget, true);
        break;

        case 3:
            pWidget = ituSceneFindWidget(&theScene, "mainReadModeRadioBox");
            ituRadioBoxSetChecked((ITURadioBox *)pWidget, true);
        break;

        case 4:
            pWidget = ituSceneFindWidget(&theScene, "mainMovieModeRadioBox");
            ituRadioBoxSetChecked((ITURadioBox *)pWidget, true);
        break;

        case 5:
            pWidget = ituSceneFindWidget(&theScene, "mainMemoryModeRadioBox");
            ituRadioBoxSetChecked((ITURadioBox *)pWidget, true);
        break;

        case 6:
            pWidget = ituSceneFindWidget(&theScene, "mainManualModeRadioBox");
            ituRadioBoxSetChecked((ITURadioBox *)pWidget, true);
        break;
    }


    //更新按摩开关显示
    pWidget = ituSceneFindWidget(&theScene, "mainMassageCheckBox");
    if(g_GUI_Data.massage_on_off)
    {
        ituCheckBoxSetChecked((ITUCheckBox *)pWidget, true);
    }
    else
    {
        ituCheckBoxSetChecked((ITUCheckBox *)pWidget, false);
    }
    MainMassageCheckBoxOnPress(pWidget, 0);
    
    //更新助眠开关显示
    pWidget = ituSceneFindWidget(&theScene, "mainSleepAidCheckBox");
    if(g_GUI_Data.sleep_aid_on_off)
    {
        ituCheckBoxSetChecked((ITUCheckBox *)pWidget, true);
    }
    else
    {
        ituCheckBoxSetChecked((ITUCheckBox *)pWidget, false);
    }

    //更新台灯开关显示
    pWidget = ituSceneFindWidget(&theScene, "mainLampCheckBox");
    if(g_GUI_Data.lamp_on_off)
    {
        ituCheckBoxSetChecked((ITUCheckBox *)pWidget, true);
    }
    else
    {
        ituCheckBoxSetChecked((ITUCheckBox *)pWidget, false);
    }

    //更新止鼾开关显示
    pWidget = ituSceneFindWidget(&theScene, "mainStopSnoreCheckBox");
    if(g_GUI_Data.lamp_on_off)
    {
        ituCheckBoxSetChecked((ITUCheckBox *)pWidget, true);
    }
    else
    {
        ituCheckBoxSetChecked((ITUCheckBox *)pWidget, false);
    }

    //--------------------更新layer_massage中的控件--------------------------

    //更新按摩模式显示
    switch(g_GUI_Data.massage_mode)
    {
        case 1:
            pWidget = ituSceneFindWidget(&theScene, "massageMode1RadioBox");
            ituRadioBoxSetChecked((ITURadioBox *)pWidget, true);
        break;

        case 2:
            pWidget = ituSceneFindWidget(&theScene, "massageMode2RadioBox");
            ituRadioBoxSetChecked((ITURadioBox *)pWidget, true);
        break;

        case 3:
            pWidget = ituSceneFindWidget(&theScene, "massageMode3RadioBox");
            ituRadioBoxSetChecked((ITURadioBox *)pWidget, true);
        break;

        case 4:
            pWidget = ituSceneFindWidget(&theScene, "massageMode4RadioBox");
            ituRadioBoxSetChecked((ITURadioBox *)pWidget, true);
        break;

        case 5:
            pWidget = ituSceneFindWidget(&theScene, "massageMode5RadioBox");
            ituRadioBoxSetChecked((ITURadioBox *)pWidget, true);
        break;

       default:
            pWidget = ituSceneFindWidget(&theScene, "massageMode1RadioBox");
            ituRadioBoxSetChecked((ITURadioBox *)pWidget, true);
        break;
    }
    
    //更新按摩强度显示
    switch(g_GUI_Data.massage_level)
    {
        case 1:
            pWidget = ituSceneFindWidget(&theScene, "massageLevel1RadioBox");
            ituRadioBoxSetChecked((ITURadioBox *)pWidget, true);
        break;

        case 2:
            pWidget = ituSceneFindWidget(&theScene, "massageLevel2RadioBox");
            ituRadioBoxSetChecked((ITURadioBox *)pWidget, true);
        break;

        case 3:
            pWidget = ituSceneFindWidget(&theScene, "massageLevel3RadioBox");
            ituRadioBoxSetChecked((ITURadioBox *)pWidget, true);
        break;

       default:
            pWidget = ituSceneFindWidget(&theScene, "massageLevel1RadioBox");
            ituRadioBoxSetChecked((ITURadioBox *)pWidget, true);
        break;
    }

    

    //更新按摩时间显示

    

    //更新当前温度显示
    memset(str, 0, sizeof(str));
    itoa(pThis->temperature, str, 10);
    pWidget = ituSceneFindWidget(&theScene, "standbyTempText");
    ituTextSetString((ITUText *)pWidget, str);


    //更新当前湿度显示

    memset(str, 0, sizeof(str));
    itoa(pThis->humidity, str, 10);
    pWidget = ituSceneFindWidget(&theScene, "standbyHumiText");
    ituTextSetString((ITUText *)pWidget, str);


    
    return;
}

/*
* 函数名称 : ExternalProcessUartTx
* 功能描述 : 收到UI任务发来的消息，根据UI发来的消息通过串口发送数据出去
* 参    数 : ev -- 消息指针
* 返回值   : 无
* 示    例 : 无
*/
/******************************************************************************/
static void ExternalProcessUartTx(ExternalEvent* ev)
/******************************************************************************/
{
	;
}

/*
* 函数名称 : ExternalInQueueProcessEvent
* 功能描述 : 收到外部任务发给UI的消息，执行对应操作
* 参    数 : ev -- 消息指针
* 返回值   : 无
* 示    例 : 无
*/
/******************************************************************************/
void ExternalInQueueProcessEvent(ExternalEvent* ev)
/******************************************************************************/
{
    //char buf[64];
    assert(ev);

    switch (ev->type)
    {
    case EXTERNAL_SHOW_MSG:
        printf("EXTERNAL_SHOW_MSG\n");
        
        break;

    case EXTERNAL_UART_UPLOAD_RX: //收到串口任务发过来的消息，对UI控件进行更新
        printf("EXTERNAL_UART_UPLOAD_RX\n");
        ExternalProcessUartUploadRx(ev);
        
        break;

    case EXTERNAL_UART_DEV_INFO_RX: //收到串口任务发过来的消息，对UI控件进行更新
        printf("EXTERNAL_UART_DEVINFO_RX\n");
        
        
        break;

    case EXTERNAL_UART_SYNC_DATA_RX: //收到串口任务发过来的消息，对UI控件进行更新
        printf("EXTERNAL_UART_SYNC_DATA_RX\n");
        
        
        break;

    case EXTERNAL_UART_FACTORY_TEST_RX: //收到串口任务发过来的消息，对UI控件进行更新
        printf("EXTERNAL_UART_FACTORY_TEST_RX\n");
        
        
        break;

    case EXTERNAL_UART_RESET_RX: //收到串口任务发过来的消息，对UI控件进行更新
        printf("EXTERNAL_UART_RESET_RX\n");
        
        
        break;
        
	case EXTERNAL_BEEP:
		printf("EXTERNAL_BEEP\n");
		break;
    
	case EXTERNAL_VOLUME:
		printf("EXTERNAL_VOLUME\n");
		theConfig.keylevel = ev->arg1*14;
		AudioSetVolume(theConfig.keylevel);
		break;
        
	case EXTERNAL_BRIGHTNESS:
		printf("EXTERNAL_BRIGHTNESS\n"); 
		theConfig.brightness = ev->arg1;
		ScreenSetBrightness(theConfig.brightness);
		break;
    }
}

/*
* 函数名称 : ExternalOutQueueProcessEvent
* 功能描述 : 收到UI任务发给外部任务的消息，执行对应操作
* 参    数 : ev -- 消息指针
* 返回值   : 无
* 示    例 : 无
*/
/******************************************************************************/
void ExternalOutQueueProcessEvent(ExternalEvent* ev)
/******************************************************************************/
{
//    char buf[64];
    assert(ev);

    switch (ev->type)
    {
        case EXTERNAL_UART_TX: //UI任务发过来的消息
            printf("EXTERNAL_UART_TX\n");
            ExternalProcessUartTx(ev);
            break;
            
    	default:break;
    }
}

/*
* 函数名称 : ExternalProcessInit
* 功能描述 : 外部任务初始化
* 参    数 : 无
* 返回值   : 无
* 示    例 : 无
*/
/******************************************************************************/
void ExternalProcessInit(void)
/******************************************************************************/
{
   // buttonLayer = ituSceneFindWidget(&theScene, "buttonLayer");
  //  assert(buttonLayer);

    //buttonCoverFlow = ituSceneFindWidget(&theScene, "buttonCoverFlow");
    //assert(buttonCoverFlow);
}
