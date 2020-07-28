#include <assert.h>
#include "scene.h"


#include "clife_lite/GUI_ClifeProtocol.h"
#include "clife_lite/GUI_DataConfig.h"


static ITUCheckBox* sleepAidStartCheckBox;
static ITUCheckBox* sleepAidMusicOnOffCheckBox;
static ITUCheckBox* sleepAidLampOnOffCheckBox;

static ITURadioBox* sleepAidMusic1RadioBox;
static ITURadioBox* sleepAidMusic2RadioBox;
static ITURadioBox* sleepAidMusic3RadioBox;
static ITURadioBox* sleepAidMusic4RadioBox;
static ITURadioBox* sleepAidMusic5RadioBox;

static ITURadioBox* sleepAidLevel1RadioBox;
static ITURadioBox* sleepAidLevel2RadioBox;
static ITURadioBox* sleepAidLevel3RadioBox;




/* widgets:
sleepAidLayer
sleepAidBackground
sleepAidLampOnOffCheckBox
sleepAidMusicOnOffCheckBox
sleepAideModeContainer
sleepAidMusic3RadioBox
sleepAidMusic1RadioBox
sleepAidMusic2RadioBox
sleepAidMusic4RadioBox
sleepAidMusic5RadioBox
sleepAidLevel3RadioBox
sleepAidLevel2RadioBox
sleepAidLevel1RadioBox
sleepAidStartCheckBox
sleepAidBackButton
*/


bool SleepAidLayerOnEnter(ITUWidget* widget, char* param)
{
    
    sleepAidStartCheckBox = ituSceneFindWidget(&theScene, "sleepAidStartCheckBox");
    assert(sleepAidStartCheckBox);
    
    sleepAidMusicOnOffCheckBox = ituSceneFindWidget(&theScene, "sleepAidMusicOnOffCheckBox");
    assert(sleepAidMusicOnOffCheckBox);
    
    sleepAidLampOnOffCheckBox = ituSceneFindWidget(&theScene, "sleepAidLampOnOffCheckBox");
    assert(sleepAidLampOnOffCheckBox);
    
    sleepAidMusic1RadioBox = ituSceneFindWidget(&theScene, "sleepAidMusic1RadioBox");
    assert(sleepAidMusic1RadioBox);
    
    sleepAidMusic2RadioBox = ituSceneFindWidget(&theScene, "sleepAidMusic2RadioBox");
    assert(sleepAidMusic2RadioBox);
    
    sleepAidMusic3RadioBox = ituSceneFindWidget(&theScene, "sleepAidMusic3RadioBox");
    assert(sleepAidMusic3RadioBox);

    sleepAidMusic4RadioBox = ituSceneFindWidget(&theScene, "sleepAidMusic4RadioBox");
    assert(sleepAidMusic4RadioBox);
    
    sleepAidMusic5RadioBox = ituSceneFindWidget(&theScene, "sleepAidMusic5RadioBox");
    assert(sleepAidMusic5RadioBox);
    
    sleepAidLevel1RadioBox = ituSceneFindWidget(&theScene, "sleepAidLevel1RadioBox");
    assert(sleepAidLevel1RadioBox);

    sleepAidLevel2RadioBox = ituSceneFindWidget(&theScene, "sleepAidLevel2RadioBox");
    assert(sleepAidLevel2RadioBox);

    sleepAidLevel3RadioBox = ituSceneFindWidget(&theScene, "sleepAidLevel3RadioBox");
    assert(sleepAidLevel3RadioBox);

    //同步layer_main中的助眠开关的状态
    if(g_GUI_Data.sleep_aid_on_off)
    {
        ituCheckBoxSetChecked(sleepAidStartCheckBox, true);
    }
    else
    {
        ituCheckBoxSetChecked(sleepAidStartCheckBox, false);
    }

    
    return true;
}

bool SleepAidLampOnOffCheckBoxOnPress(ITUWidget* widget, char* param)
{
    return true;
}

bool SleepAidMusicOnOffCheckBoxOnPress(ITUWidget* widget, char* param)
{
    return true;
}

bool SleepAidMusicRadioBoxOnPress(ITUWidget* widget, char* param)
{
    return true;
}

bool SleepAidLevelRadioBoxOnPress(ITUWidget* widget, char* param)
{
    return true;
}

bool SleepAidStartCheckBoxOnPress(ITUWidget* widget, char* param)
{
    return true;
}

