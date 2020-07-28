#include <assert.h>
#include "scene.h"
#include "clife_lite/GUI_DataConfig.h"
/* widgets:
screensaverLayer
screensaverBackground
screensaverWeekText
screensaverMonthText
screensaverslashText
screensaverDayText
screensaverDigitalClock
screensaverColonText
screensaverMinuteText
screensaverHourText
screensaverTempHumiBackground
standbyHumiText
standbyTempText
screensaverModeSprite
modeManualIcon
modeMemoryIcon
modeMovieIcon
modeReadIcon
modeSmartIcon
modeSleepIcon
screensaverStatusBackground
screensaverStatus5Sprite
Icon97
Icon98
Icon99
Icon100
Icon101
screensaverStatus4Sprite
Icon91
Icon92
Icon93
Icon94
Icon95
screensaverStatus3Sprite
Icon85
Icon86
Icon87
Icon88
Icon89
screensaverStatus2Sprite
Icon79
Icon80
Icon81
Icon82
Icon83
screensaverStatus1Sprite
statusMassageIcon
statusSleepAidIcon
statusAlarmIcon
statusLampIcon
statusSnoreIcon
screensaverWifiSprite
wifiFullIcon
wifiMediumIcon
wifiWeakIcon
wifiBreakIcon
screensaverButton
*/




static ITUSprite* screensaverWifiSprite;
static ITUSprite* screensaverModeSprite;

static ITUSprite* screensaverStatus1Sprite;
static ITUSprite* screensaverStatus2Sprite;
static ITUSprite* screensaverStatus3Sprite;
static ITUSprite* screensaverStatus4Sprite;
static ITUSprite* screensaverStatus5Sprite;

static ITUText* standbyHumiText;
static ITUText* standbyTempText;


#define FUN_STATUS_NUM 5

typedef struct
{
    bool       FlagTab[FUN_STATUS_NUM];//依次对应screensaverStatusSprite中的功能开关
    ITUSprite* SpriteTab[FUN_STATUS_NUM];//依次对应screensaverStatusSprite中的功能开关
}TS_FUN_STATUS;

TS_FUN_STATUS FunStatus;
TS_GUI_DATA *pGuiData = &g_GUI_Data;

void ShowWifiStatus(int frame)
{
    ituSpriteGoto(screensaverWifiSprite, frame);
}

void ShowModeStatus(int frame)
{
    ituSpriteGoto(screensaverModeSprite, frame);
}


void ShowFunStatus(TS_FUN_STATUS *FunStatus)
{
    int show_cnt = 0;
    int i = 0;
    
    FunStatus->SpriteTab[0] = screensaverStatus1Sprite;
    FunStatus->SpriteTab[1] = screensaverStatus2Sprite;
    FunStatus->SpriteTab[2] = screensaverStatus3Sprite;
    FunStatus->SpriteTab[3] = screensaverStatus4Sprite;
    FunStatus->SpriteTab[4] = screensaverStatus5Sprite;

    for(i = 0; i<FUN_STATUS_NUM;i++)
    {
        ituWidgetSetVisible(FunStatus->SpriteTab[i], 0);
    }
    
    for(i = 0;i<FUN_STATUS_NUM;i++)
    {
        if(FunStatus->FlagTab[i])
        {
            ituWidgetSetVisible(FunStatus->SpriteTab[show_cnt], 1);
            ituSpriteGoto(FunStatus->SpriteTab[show_cnt], i);
            show_cnt++;
        }
    }
}

bool ScreensaverOnEnter(ITUWidget* widget, char* param)
{
    int temp = 0;
    char str[10];
    int temperature = 0;
    int humidity = 0;
    
    screensaverModeSprite = ituSceneFindWidget(&theScene, "screensaverModeSprite");
    assert(screensaverModeSprite);
    
    screensaverWifiSprite = ituSceneFindWidget(&theScene, "screensaverWifiSprite");
    assert(screensaverWifiSprite);

    screensaverStatus1Sprite = ituSceneFindWidget(&theScene, "screensaverStatus1Sprite");
    assert(screensaverStatus1Sprite);
    screensaverStatus2Sprite = ituSceneFindWidget(&theScene, "screensaverStatus2Sprite");
    assert(screensaverStatus2Sprite);
    screensaverStatus3Sprite = ituSceneFindWidget(&theScene, "screensaverStatus3Sprite");
    assert(screensaverStatus3Sprite);
    screensaverStatus4Sprite = ituSceneFindWidget(&theScene, "screensaverStatus4Sprite");
    assert(screensaverStatus4Sprite);
    screensaverStatus5Sprite = ituSceneFindWidget(&theScene, "screensaverStatus5Sprite");
    assert(screensaverStatus5Sprite);

    standbyHumiText = ituSceneFindWidget(&theScene, "standbyHumiText");
    assert(standbyHumiText);
    standbyTempText = ituSceneFindWidget(&theScene, "standbyTempText");
    assert(standbyTempText);

    //获取WiFi型号强度并映射为显示索引
    temp = 3;
    ShowWifiStatus(temp);

    //获取当前模式并映射为显示索引
    temp = 2;
    ShowModeStatus(temp);

    //获取状态图标的标志并显示
    FunStatus.FlagTab[0] = 1;
    FunStatus.FlagTab[1] = 0;
    FunStatus.FlagTab[2] = 1;
    FunStatus.FlagTab[3] = 0;
    FunStatus.FlagTab[4] = 1;
    ShowFunStatus(&FunStatus);

    //温度显示
    temp = pGuiData->temperature;
    memset(str,0,sizeof(str));
    itoa(temp, str, 10);
    ituTextSetString(standbyTempText, str);

    //湿度显示
    temp = 60;
    memset(str,0,sizeof(str));
    itoa(temp, str, 10);
    ituTextSetString(standbyHumiText, str);
    return true;
}

