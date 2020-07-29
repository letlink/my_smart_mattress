#include <assert.h>
#include "scene.h"

#include "clife_lite/GUI_ClifeProtocol.h"
#include "clife_lite/GUI_DataConfig.h"


static ITUCheckBox* setSysKeySoundOnOffCheckBox;

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

