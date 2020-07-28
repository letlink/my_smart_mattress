#include <assert.h>
#include "scene.h"

#include "clife_lite/GUI_ClifeProtocol.h"
#include "clife_lite/GUI_DataConfig.h"

static ITURadioBox* massageMode1RadioBox;
static ITURadioBox* massageMode2RadioBox;
static ITURadioBox* massageMode3RadioBox;
static ITURadioBox* massageMode4RadioBox;
static ITURadioBox* massageMode5RadioBox;

static ITURadioBox* massageLevel1RadioBox;
static ITURadioBox* massageLevel2RadioBox;
static ITURadioBox* massageLevel3RadioBox;

static ITUCheckBox* massageStartCheckBox;

static ITUText*  massageRemainingTimeText;

extern void MassageStart(void);
extern void MassageStop(void);
/* widgets:
massageLayer
massageBackground
massageRemainingTimeText
massageModeContainer
massageMode3RadioBox
massageMode1RadioBox
massageMode2RadioBox
massageMode4RadioBox
massageMode5RadioBox
massageLevel3RadioBox
massageLevel2RadioBox
massageLevel1RadioBox
massageStartCheckBox
massageBackButton
*/

bool MassageLayerOnEnter(ITUWidget* widget, char* param)
{
    
    massageStartCheckBox = ituSceneFindWidget(&theScene, "massageStartCheckBox");
    assert(massageStartCheckBox);
    
    massageMode1RadioBox = ituSceneFindWidget(&theScene, "massageMode1RadioBox");
    assert(massageMode1RadioBox);
    
    massageMode2RadioBox = ituSceneFindWidget(&theScene, "massageMode2RadioBox");
    assert(massageMode2RadioBox);
    
    massageMode3RadioBox = ituSceneFindWidget(&theScene, "massageMode3RadioBox");
    assert(massageMode3RadioBox);
    
    massageMode4RadioBox = ituSceneFindWidget(&theScene, "massageMode4RadioBox");
    assert(massageMode4RadioBox);
    
    massageMode5RadioBox = ituSceneFindWidget(&theScene, "massageMode5RadioBox");
    assert(massageMode5RadioBox);

    massageLevel1RadioBox = ituSceneFindWidget(&theScene, "massageLevel1RadioBox");
    assert(massageLevel1RadioBox);
    
    massageLevel2RadioBox = ituSceneFindWidget(&theScene, "massageLevel2RadioBox");
    assert(massageLevel2RadioBox);
    
    massageLevel3RadioBox = ituSceneFindWidget(&theScene, "massageLevel3RadioBox");
    assert(massageLevel3RadioBox);

    massageRemainingTimeText = ituSceneFindWidget(&theScene, "massageRemainingTimeText");
    assert(massageRemainingTimeText);

    

    //同步layer_main中的按摩开关的状态
    if(g_GUI_Data.massage_on_off)
    {
        ituCheckBoxSetChecked(massageStartCheckBox, true);
    }
    else
    {
        ituCheckBoxSetChecked(massageStartCheckBox, false);
    }
    
    return true;
}



bool MassageLayerOnTimer(ITUWidget* widget, char* param)
{
    static uint32_t time_left_last;
    char c_value[20];
    uint32_t temp = 0;

    if (g_GUI_Data.massage_on_off)
    {
        if (time_left_last != g_GUI_Data.massage_time_left)
        {
            time_left_last = g_GUI_Data.massage_time_left;

            temp = g_GUI_Data.massage_time_left / 1000;
            sprintf(c_value, "%d", temp);

            ituTextSetString(massageRemainingTimeText, c_value);

            if (temp == 0)
            {
                ituCheckBoxSetChecked(massageStartCheckBox, false);
            }
        }
    }
    else
    {
        ituTextSetString(massageRemainingTimeText, "0");
    }
    
    return true;
}

bool MassageModeRadioBox(ITUWidget* widget, char* param)
{
    uint8_t temp = 0;
    
    if(ituRadioBoxIsChecked(massageMode1RadioBox)) 
    {
        temp = 1;
    }
    else if(ituRadioBoxIsChecked(massageMode2RadioBox))
    {
        temp = 2;
    }
    else if(ituRadioBoxIsChecked(massageMode3RadioBox))
    {
        temp = 3;
    }
    else if(ituRadioBoxIsChecked(massageMode4RadioBox))
    {
        temp = 4;
    }
    else if(ituRadioBoxIsChecked(massageMode5RadioBox))
    {
        temp = 5;
    }

    //发送数据到串口
    g_GUI_Data.massage_mode = temp;
    HET_GUI_SendDataFrame(CMD_DOWNLOAD, ID_MASSAGE_MODE, LEN_MASSAGE_MODE, (uint8_t*)&g_GUI_Data.massage_mode);
    
    return true;
}

bool MassageLevelRadioBox(ITUWidget* widget, char* param)
{
    uint8_t temp = 0;
    
    if(ituRadioBoxIsChecked(massageLevel1RadioBox))
    {
        temp = 1;
    }
    else if(ituRadioBoxIsChecked(massageLevel2RadioBox))
    {
        temp = 2;
    }
    else if(ituRadioBoxIsChecked(massageLevel3RadioBox))
    {
        temp = 3;
    }

    //发送数据到串口
    g_GUI_Data.massage_level = temp;
    HET_GUI_SendDataFrame(CMD_DOWNLOAD, ID_MASSAGE_LEVEL, LEN_MASSAGE_LEVEL, (uint8_t*)&g_GUI_Data.massage_level);
    return true;
}

bool MassageStartCheckBoxOnPress(ITUWidget* widget, char* param)
{
    if (ituCheckBoxIsChecked(massageStartCheckBox))
    {
        g_GUI_Data.massage_on_off = 1;
        MassageStart();
    }
    else
    {
        g_GUI_Data.massage_on_off = 0;
        MassageStop();
        //ITUText* pWidget = ituSceneFindWidget(&theScene, "massageRemainingTimeText");
        //ituTextSetString(pWidget, "0");
    }
    HET_GUI_SendDataFrame(CMD_DOWNLOAD, ID_MASSAGE_ON_OFF, LEN_MASSAGE_ON_OFF, (uint8_t*)&g_GUI_Data.massage_on_off);
    
    return true;
}

