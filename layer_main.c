#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "SDL/SDL.h"
#include "scene.h"
#include "ctrlboard.h"
#include <signal.h>
#include "clife_lite/GUI_ClifeProtocol.h"
#include "clife_lite/GUI_DataConfig.h"

#define UNIT_MS     33
#define MIN_SPEED   1
#define MAX_SPEED   10


static ITUCircleProgressBar* mainMassageCircleProgressBar;
static ITUText *mianMassagePercentText;

static ITUCheckBox* mainMassageCheckBox;
static ITUCheckBox* mainSleepAidCheckBox;
static ITUCheckBox* mainLampCheckBox;
static ITUCheckBox* mainStopSnoreCheckBox;

static ITUSprite* mainAlarmSprite;

static ITURadioBox* mainSleepModeRadioBox;
static ITURadioBox* mainSmartModeRadioBox;
static ITURadioBox* mainReadModeRadioBox;
static ITURadioBox* mainMovieModeRadioBox;
static ITURadioBox* mainMemoryModeRadioBox;
static ITURadioBox* mainManualModeRadioBox;


static bool circleCtrlPlaying;
static int circleCtrlSpeed;
static int circleCtrlStep;

typedef struct
{
    bool  playing; //运行标志
    bool  update;  //更新标志
    int   speed;    //运行速度
    int   step;     //运行进度
    int   step_max; //最大进度 0~100
    int   init_val; //初始值
    int   left;     //
    uint32_t   last_tick;
    
} CIRCLE_CTRL;

CIRCLE_CTRL massageCircleCtrl;
CIRCLE_CTRL sleepaidCircleCtrl;

extern int GetMassageSetTime(void);

static pthread_t userProgressTaskID;
static pthread_attr_t attr;
static volatile bool userProgressQuit;
//int flag = 0;
//void MassageStop(void);
/* widgets:
mainLayer
mainBackground
mainMassageCircleProgressBar
mianMassagePercentText
mainMassagePregressPointIcon
mainAlarmContainer
mainAlarmButton
mainAlarmSprite
mainAlarmOffIcon
mainAlarmOnIcon
mainLampSettingButton
mainSleepAidSettingButton
mainMassageSettingButton
mainSettingCheckBox
mainStopSnoreCheckBox
mainLampCheckBox
mainSleepAidCheckBox
mainMassageCheckBox
mainManualModeRadioBox
mainMemoryModeRadioBox
mainMovieModeRadioBox
mainReadModeRadioBox
mainSmartModeRadioBox
mainSleepModeRadioBox
*/

int ProgressHandler(CIRCLE_CTRL* pCtrl)
{
    int time_set = 0;
    uint32_t factor = 0;
    int res = 0;
    uint32_t tick = 0;


    if (pCtrl->playing)
    {

        tick = SDL_GetTicks();

        time_set = pCtrl->init_val;//毫秒单位
        factor = time_set / pCtrl->step_max; //圆形进度条分成step_max等分,比如100等分

        if ((tick - pCtrl->last_tick) >= factor)
        {
            pCtrl->step++;
            pCtrl->last_tick = tick;
            pCtrl->left = time_set - (pCtrl->step * factor);
            res = 1;
        }
    }
    return res;


}

/*
* 函数名称 : UserProgressTask
* 功能描述 : 圆形进度条处理任务
* 参    数 : arg -- 任务参数
* 返回值   : 无
* 示    例 : 无
*/
/******************************************************************************/
static void* UserProgressTask(void* arg)
/******************************************************************************/
{
    CIRCLE_CTRL* pCtrl = &massageCircleCtrl;
    ITUText *pPercentText = mianMassagePercentText;
    ITUCircleProgressBar* pProgressBar = mainMassageCircleProgressBar;

    int res = 0;
    int time_left_temp = 0;

    while (!userProgressQuit)
    {
        res = ProgressHandler(&massageCircleCtrl);
        if (res)
        {
            massageCircleCtrl.update = 1;
            g_GUI_Data.massage_time_left = massageCircleCtrl.left;
            if (massageCircleCtrl.step >= massageCircleCtrl.step_max) //进度条到100%
            {
                g_GUI_Data.massage_on_off = 0;
                userProgressQuit = true;
                
                pthread_exit(0);
            }
        }

        usleep(10000);//10ms
    }
    pthread_exit(0);
    return NULL;
}


void MassageStart(void)
{
    CIRCLE_CTRL* pCtrl = &massageCircleCtrl;
    ITUText *pPercentText = mianMassagePercentText;
    ITUCircleProgressBar* pProgressBar = mainMassageCircleProgressBar;

    pCtrl->playing = true;
    pCtrl->step = 0;
    pCtrl->step_max = pProgressBar->maxValue;
    pCtrl->init_val = GetMassageSetTime();
    ituWidgetSetVisible(pProgressBar, true);//设置圆形进度条可见状态

    userProgressQuit = false;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&userProgressTaskID, &attr, UserProgressTask, NULL);
}

void MassageStop(void)
{
    CIRCLE_CTRL* pCtrl = &massageCircleCtrl;
    ITUText *pPercentText = mianMassagePercentText;
    ITUCircleProgressBar* pProgressBar = mainMassageCircleProgressBar;

    pCtrl->playing = false;
    pCtrl->step = 0;
    pProgressBar->value = 0;
    ituWidgetSetVisible(pProgressBar, false);//设置圆形进度条可见状态

    userProgressQuit = true;
    //pthread_kill(userProgressTaskID, NULL); 
    //pthread_join(userProgressTaskID, NULL);
}



int MassageProgressHandler(void)
{
    CIRCLE_CTRL* pCtrl = &massageCircleCtrl;
    ITUText *pPercentText = mianMassagePercentText;
    ITUCircleProgressBar* pProgressBar = mainMassageCircleProgressBar;

    char c_value[20];
    int res = 0;
    int time_left_temp =0;


    if (pCtrl->update)
    {
        pCtrl->update = 0;
        time_left_temp = pCtrl->left / 1000;//转换为秒
        if (pCtrl->left % (1000))
        {
            time_left_temp = time_left_temp + 1;
        }

        ituCircleProgressBarSetValue(pProgressBar, pCtrl->step);
        sprintf(c_value, "%d", time_left_temp);
        ituTextSetString(pPercentText, c_value);


        if (pProgressBar->value >= pProgressBar->maxValue)
        {
            res = 1;
        }
    }

    return res;
}



bool MainLayerOnEnter(ITUWidget* widget, char* param)
{
    mainSleepModeRadioBox = ituSceneFindWidget(&theScene, "mainSleepModeRadioBox");
    assert(mainSleepModeRadioBox);
    
    mainSmartModeRadioBox = ituSceneFindWidget(&theScene, "mainSmartModeRadioBox");
    assert(mainSmartModeRadioBox);
    
    mainReadModeRadioBox = ituSceneFindWidget(&theScene, "mainReadModeRadioBox");
    assert(mainReadModeRadioBox);
    
    mainMovieModeRadioBox = ituSceneFindWidget(&theScene, "mainMovieModeRadioBox");
    assert(mainMovieModeRadioBox);
    
    mainMemoryModeRadioBox = ituSceneFindWidget(&theScene, "mainMemoryModeRadioBox");
    assert(mainMemoryModeRadioBox);
    
    mainManualModeRadioBox = ituSceneFindWidget(&theScene, "mainManualModeRadioBox");
    assert(mainManualModeRadioBox);

    mainMassageCircleProgressBar = ituSceneFindWidget(&theScene, "mainMassageCircleProgressBar");
    assert(mainMassageCircleProgressBar);

    mianMassagePercentText = ituSceneFindWidget(&theScene, "mianMassagePercentText");
    assert(mianMassagePercentText);

    mainMassageCheckBox = ituSceneFindWidget(&theScene, "mainMassageCheckBox");
    assert(mainMassageCheckBox);
    
    mainSleepAidCheckBox = ituSceneFindWidget(&theScene, "mainSleepAidCheckBox");
    assert(mainSleepAidCheckBox);
    
    mainLampCheckBox = ituSceneFindWidget(&theScene, "mainLampCheckBox");
    assert(mainLampCheckBox);
    
    mainStopSnoreCheckBox = ituSceneFindWidget(&theScene, "mainStopSnoreCheckBox");
    assert(mainStopSnoreCheckBox);
    
    mainAlarmSprite = ituSceneFindWidget(&theScene, "mainAlarmSprite");
    assert(mainAlarmSprite);

    //同步layer_main中的按摩开关的状态
    if (g_GUI_Data.massage_on_off)
    {
        ituCheckBoxSetChecked(mainMassageCheckBox, true);
        ituWidgetSetVisible(mainMassageCircleProgressBar, true);//设置圆形进度条可见状态
    }
    else
    {
        ituCheckBoxSetChecked(mainMassageCheckBox, false);
        ituWidgetSetVisible(mainMassageCircleProgressBar, false);//设置圆形进度条可见状态
    }

    return true;
}

bool MainLayerOnTimer(ITUWidget* widget, char* param)
{
    bool updated = false;

    if (MassageProgressHandler()) //进度条走完
    {
        MassageStop();
        ituCheckBoxSetChecked(mainMassageCheckBox, false);

        //发送数据到串口
        g_GUI_Data.massage_on_off = 0;
        HET_GUI_SendDataFrame(CMD_DOWNLOAD, ID_MASSAGE_ON_OFF, LEN_MASSAGE_ON_OFF, (uint8_t*)&g_GUI_Data.massage_on_off);
    }

    return updated;
}

bool MainMattressModeRadioBoxOnPress(ITUWidget* widget, char* param)
{
    uint8_t temp = 0;
    
    if(ituRadioBoxIsChecked(mainSleepModeRadioBox))
    {
        temp = 1;
    }
    else if(ituRadioBoxIsChecked(mainSmartModeRadioBox))
    {
        temp = 2;
    }
    else if(ituRadioBoxIsChecked(mainReadModeRadioBox))
    {
        temp = 3;
    }
    else if(ituRadioBoxIsChecked(mainMovieModeRadioBox))
    {
        temp = 4;
    }
    else if(ituRadioBoxIsChecked(mainMemoryModeRadioBox))
    {
        temp = 5;
    }
    else if(ituRadioBoxIsChecked(mainManualModeRadioBox))
    {
        temp = 6;
    }

    //发送数据到串口
    g_GUI_Data.mattress_mode = temp;
    HET_GUI_SendDataFrame(CMD_DOWNLOAD, ID_MATTRESS_MODE, LEN_MATTRESS_MODE, (uint8_t*)&g_GUI_Data.mattress_mode);
    
    return true;
}


bool MainMassageCheckBoxOnPress(ITUWidget* widget, char* param)
{

    if (ituCheckBoxIsChecked(mainMassageCheckBox))
    {
        MassageStart();
        g_GUI_Data.massage_on_off = 1;
    }
    else
    {
        MassageStop();
        g_GUI_Data.massage_on_off = 0;
    }
    //发送数据到串口
    HET_GUI_SendDataFrame(CMD_DOWNLOAD, ID_MASSAGE_ON_OFF, LEN_MASSAGE_ON_OFF, (uint8_t*)&g_GUI_Data.massage_on_off);

    return true;
}

bool MainSleepAidCheckBoxOnPress(ITUWidget* widget, char* param)
{
    if (ituCheckBoxIsChecked(mainSleepAidCheckBox))
    {
        g_GUI_Data.sleep_aid_on_off = 1;
    }
    else
    {
        g_GUI_Data.sleep_aid_on_off = 0;
    }
    //发送数据到串口
    HET_GUI_SendDataFrame(CMD_DOWNLOAD, ID_SLEEP_AID_ON_OFF, LEN_SLEEP_AID_ON_OFF, (uint8_t*)&g_GUI_Data.sleep_aid_on_off);

    return true;
}



bool MainLampCheckBoxOnPress(ITUWidget* widget, char* param)
{

    if (ituCheckBoxIsChecked(mainLampCheckBox))
    {
        g_GUI_Data.lamp_on_off = 1;
    }
    else
    {
        g_GUI_Data.lamp_on_off = 0;
    }
    //发送数据到串口
    HET_GUI_SendDataFrame(CMD_DOWNLOAD, ID_LAMP_ON_OFF, LEN_LAMP_ON_OFF, (uint8_t*)&g_GUI_Data.lamp_on_off);

    return true;
}


bool MainStopSnoreCheckBoxOnPress(ITUWidget* widget, char* param)
{

    if (ituCheckBoxIsChecked(mainStopSnoreCheckBox))
    {
        g_GUI_Data.stop_snore_on_off = 1;
    }
    else
    {
        g_GUI_Data.stop_snore_on_off = 0;
    }
    //发送数据到串口
    HET_GUI_SendDataFrame(CMD_DOWNLOAD, ID_STOP_SNORE_ON_OFF, LEN_STOP_SNORE_ON_OFF, (uint8_t*)&g_GUI_Data.stop_snore_on_off);

    return true;
}




