#include <assert.h>
#include "scene.h"

#include "clife_lite/GUI_ClifeProtocol.h"
#include "clife_lite/GUI_DataConfig.h"

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
    return true;
}

bool SetSysBacklightTrackBarChanged(ITUWidget* widget, char* param)
{
    return true;
}

bool SetSysKeySoundOnOffCheckBoxOnPress(ITUWidget* widget, char* param)
{
    return true;
}

