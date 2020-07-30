#include <assert.h>
#include "scene.h"

#include "clife_lite/GUI_ClifeProtocol.h"
#include "clife_lite/GUI_DataConfig.h"


static ITUCheckBox* setSysKeySoundOnOffCheckBox;
static ITUText* setSysBacklightPercentText;

extern void BuzzerInit(void);
extern void BuzzerExit(void);


/* widgets:
setSysLayer
setSysBackground
Text6
setSysBacklightPercentText
setSysBacklightTrackBar
setSysBacklightButton
setSysBacklightProgressBar
setSysBacklightText
setSysKeySoundOnOffCheckBox
setSysKeySoundText
setSysBackButton
*/

bool SetSysLayerOnEnter(ITUWidget* widget, char* param)
{
    setSysKeySoundOnOffCheckBox = ituSceneFindWidget(&theScene, "setSysKeySoundOnOffCheckBox");
    assert(setSysKeySoundOnOffCheckBox);

    setSysBacklightPercentText = ituSceneFindWidget(&theScene, "setSysBacklightPercentText");
    assert(setSysBacklightPercentText);
    

    if(g_GUI_Data.key_sound_on_off)
    {
        ituCheckBoxSetChecked(setSysKeySoundOnOffCheckBox, true);
    }
    else
    {
        ituCheckBoxSetChecked(setSysKeySoundOnOffCheckBox, false);
    }
    return true;
}

bool SetSysBacklightTrackBarChanged(ITUWidget* widget, char* param)
{
#ifdef _WIN32
    
#else
    int max_brightness = ScreenGetMaxBrightness();
    int set_brightness = atoi(ituTextGetString(setSysBacklightPercentText));
    set_brightness = set_brightness / 10;
    if (set_brightness > max_brightness) set_brightness = max_brightness;
    ScreenSetBrightness(set_brightness);
    HET_GUI_SendDataFrame(CMD_DOWNLOAD, ID_BACKLIGHT, LEN_BACKLIGHT, (uint8_t*)&g_GUI_Data.backlight);
#endif
    return true;
}

bool SetSysKeySoundOnOffCheckBoxOnPress(ITUWidget* widget, char* param)
{
    if (ituCheckBoxIsChecked(setSysKeySoundOnOffCheckBox))
    {
        g_GUI_Data.key_sound_on_off = 1;
    }
    else
    {
        g_GUI_Data.key_sound_on_off = 0;
    }
    HET_GUI_SendDataFrame(CMD_DOWNLOAD, ID_KEY_SOUND_ON_OFF, LEN_KEY_SOUND_ON_OFF, (uint8_t*)&g_GUI_Data.key_sound_on_off);
    return true;
}

